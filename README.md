# Nix Busybox Bootstrapper

This repository contains basic tools to bootstrap a
[Nix](https://github.com/NixOS/nix) build environment **without
[nixpkgs](https://github.com/NixOS/nixpkgs)**.

The bootstrapped environment only contains the core utilities offered by
[BusyBox](https://www.busybox.net/) (`sh`, `gzip`, `cat`, `wget`, and [various
other](https://busybox.net/downloads/BusyBox.html) Unix core utils). It contains
no C toolchain or other builders whatsoever.

So, if you'd like to build something using a language other than Bourne shell,
this tool is pretty much useless. (Unless you plan to use it to bootstrap an
environment with a C toolchain.)

The implementation of this tools is quite simple. In total it is less than 300
lines of C, shell, and Nix (excluding the BusyBox source of course). Users are
encouraged to review the source.

This tool is third-party software which is not affiliated with Nix or BusyBox in
any way.

## Usage

To use this tool, start out by cloning and building it like so:

```shell
git clone --recurse-submodules https://github.com/parkerhoyes/nix-busybox-bootstrapper
cd nix-busybox-bootstrapper/
make
```

If you get warnings during the `bootstrapper` build, they will cause the build
to fail. You can ignore them and proceed with the build anyway like so:

```shell
NO_WERROR=1 make
```

Running `make` will build both BusyBox in the `busybox/` submodule and the
`bootstrapper` C program (see
[`bootstrapper/src/main.c`](bootstrapper/src/main.c)). This build step does not
involve Nix. These builds should be bit-for-bit reproducible, but this will
depend on which toolchain you use. The build will copy the BusyBox configuration
file included in this repo (`busybox-config.cfg`) into the BusyBox repository;
this is simply the default config with statically linked builds enabled.

Once compiled, simply import the `busybootstrap` library into your Nix
expression like so:

```Nix
let busybootstrap = import ./path-to-repo-root; in

busybootstrap.derivation {
    builder = ./path-to-my-shell-script.sh;
    args = [ "args" "for" "my" "shell" "script" ];
    # The rest of the arguments are the same as for Nix's builtin derivation
    # function. Don't forget to set `name` and `system`.
}
```

When it is run, the builder shell script will have all of BusyBox's utilities
available on the `PATH`. There's no need to include a shebang line or set the
executable bit on your builder shell script - it is passed directly to the shell
interpreter.

This Nix library does not need a network connection to work, as all of the
binaries it uses will be built in this repository.

The bash script `./shell` will build the `busybootstrapper` standard environment
and launch its shell. The environment variables set will be very similar to the
environment when running a Nix build, but it will not be in a `chroot`'d
environment and the CWD will not be changed. You must run the build with `make`
before this script can be used.

There is a test Nix expression which demonstrates using this library in
`tests/`. You can build it using `make test`.

The `busybox/` submodule points to BusyBox version 1.31.1.

## Bootstrap Process

This tool contains a Nix library (in `default.nix`) which defines a wrapper
around Nix's `builtins.derivation` function. This wrapper will allow you to
specify a shell script as the builder. The derivation it produces will
automatically launch your shell script with the shell installed into the
`busybootstrap` environment (which it depends on). The `busybootstrap`
environment is defined by a Nix derivation whose builder is a simple C program
(in `bootstrapper/`) which simply installs BusyBox into the environment.

The `busybootstrap` Nix library also contains an `stdenv` attribute which is the
derivation of the standard environment created by this tool. This is simply a
directory containing the file `busybootstrap-version` (with the version of this
tool) and a `bin/` directory which contains all of the BusyBox tools, `busybox`
itself, and the `bootstrapper` binary.

Unfortunately, due to the way BusyBox works, you cannot run these tools directly
from Nix by setting them as the builder (for example, `builder =
"${busybootstrap.stdenv}/bin/sh"`). This is because the `busybox` binary uses
`argv[0]` to determine which applet (command) to run, but since Nix copies the
builder into the store, the `argv[0]` will always end up being a mangled name
that BusyBox doesn't recognize. This is why the `bootstrapper` C program is
necessary; the `busybootstrap.derivation` function uses `bootstrapper` as the
builder which will, in turn, invoke BusyBox's shell which, in turn, launches
your builder.

## License

This software is licensed under the terms of the very permissive [Zlib
License](https://opensource.org/licenses/Zlib). The exact text of this license
is reproduced in the [`LICENSE.txt`](LICENSE.txt) file.

This repository contains a git submodule (at `busybox/`) which links to the
BusyBox repository, to which this project's license does not apply. BusyBox is
licensed under GPL-2.0.
