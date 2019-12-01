/*
License for nix-busybox-bootstrapper, originally found here:
https://github.com/parkerhoyes/nix-busybox-bootstrapper

Copyright (C) 2019 Parker Hoyes <contact@parkerhoyes.com>

This software is provided "as-is", without any express or implied warranty. In
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject to
the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim
   that you wrote the original software. If you use this software in a product,
   an acknowledgment in the product documentation would be appreciated but is
   not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define BUFF_SIZE 8192
#define PATH_SIZE 8192

#define UNUSED(x) (void)(x)

extern char **environ;

static char buff[BUFF_SIZE];
static char target_path[PATH_SIZE];

static const char *errormsg;

static int dump_envvars(void);
static const char *eval_envvar(const char *name);
static const char *get_envvar(const char *name);
static int joinpath(char *dest, const char *abs, const char *rel);
static int create_target(const char *bin, const char *source);
static void exec_target(const char *const argv[]);
static void handle_error(void);

int main(int argc, char *argv[]) {
    const char *source, *bin, *target;
    UNUSED(argc); UNUSED(argv);

    errormsg = NULL;

    // Dump environment, helpful for debugging
    if (dump_envvars()) {
        errormsg = "error dumping environment";
        goto bail;
    }

    // Parse args
    if (argc < 4) {
        errormsg = "not enough args";
        goto bail;
    }
    source = eval_envvar(argv[1]);
    bin =    eval_envvar(argv[2]);
    target = eval_envvar(argv[3]);
    if (bin == NULL || target == NULL) {
        errormsg = "bin or target path is empty or unset";
        goto bail;
    }
    if (target[0] == '/') {
        errormsg = "target path must be relative";
        goto bail;
    }

    // Set `target_path` to `bin`/`target`
    if (joinpath(target_path, bin, target)) {
        errormsg = "paths too large";
        goto bail;
    }

    // If `source` is set, create `bin` and `target_path` by copying from
    // `source`.
    if (source != NULL && create_target(bin, source)) {
        goto bail;
    }

    // Exec `target_path` with remaining args (first remaining arg becomes
    // argv[0]!), replacing current process.
    exec_target((void*) (argv + 4)); // No return unless error
    goto bail;

bail:
    handle_error(); // No return
    return 4; // To make the compiler happy
}

/*
 * Print all environment variables to stdout. Return 0 on success.
 */
int dump_envvars(void) {
    char **entryptr;
    for (entryptr = environ; *entryptr != NULL; entryptr++) {
        if (puts(*entryptr) < 0) {
            return 1;
        }
    }
    return 0;
}

/*
 * While `name` is not `NULL` and it begins with "$", get the value of the
 * environment variable with that name, and repeat. Return `NULL` if the result
 * is not set or empty.
 */
const char *eval_envvar(const char *name) {
    if (name != NULL && name[0] == 0) {
        return NULL;
    }
    while (name != NULL && name[0] == '$') {
        name = get_envvar(name + 1);
    }
    return name;
}

/*
 * Get the value of an environment variable. Return `NULL` if not set or empty.
 */
const char *get_envvar(const char *name) {
    const char *value = getenv(name);
    if (value != NULL && value[0] == 0) {
        value = NULL;
    }
    return value;
}

/*
 * Set `dest` to the path `abs`/`rel`. Return non-zero on failure due to the
 * resulting path being larger than PATH_SIZE (including null terminator).
 */
int joinpath(char *dest, const char *abs, const char *rel) {
    size_t size = strlen(abs);
    if (size + 1 >= PATH_SIZE) {
        return 1;
    }
    strncpy(dest, abs, PATH_SIZE);
    if (dest[size - 1] == '/') {
        size -= 1;
        dest[size] = 0;
    }
    if (PATH_SIZE - size - strlen(rel) < 2) {
        return 1;
    }
    strcat(dest, "/");
    strcat(dest, rel);
    return 0;
}

/*
 * Create the directory `bin` and copy `source` to `target_path` and set the new
 * file's mode to 0755. Return 0 on success.
 */
int create_target(const char *bin, const char *source) {
    size_t size;
    FILE *fsource, *fdest;

    if (mkdir(bin, 0777)) {
        return 1;
    }

    fsource = fopen(source, "rb");
    if (fsource == NULL) {
        return 1;
    }
    fdest = fopen(target_path, "wb");
    if (fdest == NULL) {
        return 1;
    }

    if (fchmod(fileno(fdest), 0755)) {
        return 1;
    }

    while (!feof(fsource)) {
        clearerr(fsource);
        size = fread(buff, 1, sizeof(buff), fsource);
        if (size != 0) {
            clearerr(fdest);
            fwrite(buff, 1, size, fdest);
            if (ferror(fdest)) {
                return 1;
            }
        }
        if (ferror(fsource)) {
            return 1;
        }
    }

    if (fclose(fsource) || fclose(fdest)) {
        return 1;
    }

    return 0;
}

/*
 * Exec `target_path` with the provided args (first arg is argv[0]!), replacing
 * the current process. Only return on failure.
 */
void exec_target(const char *const argv[]) {
    execv(target_path, (void*) argv); // No return unless error
}

/*
 * Print error message to stdout and exit with non-zero status code. No return.
 */
void handle_error(void) {
    printf("fatal error: %s\n", errormsg != NULL ? errormsg : strerror(errno));
    exit(errormsg != NULL ? 2 : 3);
    for (;;);
}
