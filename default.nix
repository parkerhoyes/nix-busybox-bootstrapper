# License for nix-busybox-bootstrapper, originally found here:
# https://github.com/parkerhoyes/nix-busybox-bootstrapper
#
# Copyright (C) 2019 Parker Hoyes <contact@parkerhoyes.com>
#
# This software is provided "as-is", without any express or implied warranty. In
# no event will the authors be held liable for any damages arising from the use of
# this software.
#
# Permission is granted to anyone to use this software for any purpose, including
# commercial applications, and to alter it and redistribute it freely, subject to
# the following restrictions:
#
# 1. The origin of this software must not be misrepresented; you must not claim
#    that you wrote the original software. If you use this software in a product,
#    an acknowledgment in the product documentation would be appreciated but is
#    not required.
# 2. Altered source versions must be plainly marked as such, and must not be
#    misrepresented as being the original software.
# 3. This notice may not be removed or altered from any source distribution.

with import ./version.nix;

rec {
    stdenv = builtins.derivation {
        system = builtins.currentSystem;
        name = "busybootstrap-stdenv";
        builder = ./bootstrapper/bin/bootstrapper;
        args = [
            ./busybox/busybox # source (copied to the target)
            "$out"            # bin (evaluated as env var by bootstrapper)
            "busybox"         # target (relative to bin)

            # Args to pass to busybox (first is argv[0], which tells busybox
            # which applet to run)
            "sh"
            ./bootstrap.sh
        ];
        inherit BUSYBOOTSTRAP_VERSION;
    };

    derivation = deriv_args@{
        builder,   # Will be run with busybox's shell
        args ? [], # Arguments to builder script
        ...        # Remaining params are the same as for builtins.derivation
    }: builtins.derivation (deriv_args // {
        builder = "${stdenv}/bin/bootstrapper";
        args = [
            ""              # source (empty means don't copy)
            "${stdenv}/bin" # bin
            "busybox"       # target (relative to bin)

            # Args to pass to busybox (first is argv[0], which tells busybox
            # which applet to run)
            "sh"
            ./launcher.sh
            builder
        ] ++ args;
        BUSYBOOTSTRAP_STDENV = stdenv;
    });
}
