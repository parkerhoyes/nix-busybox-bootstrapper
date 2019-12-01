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

PATH="$BUSYBOOTSTRAP_STDENV/bin"
export PATH

BUSYBOOTSTRAP_VERSION="$(cat $BUSYBOOTSTRAP_STDENV/busybootstrap-version)"
export BUSYBOOTSTRAP_VERSION

BUSYBOOTSTRAP_VERSION_MAJOR="${BUSYBOOTSTRAP_VERSION%.*}"
BUSYBOOTSTRAP_VERSION_MAJOR="${BUSYBOOTSTRAP_VERSION_MAJOR%.*}"
export BUSYBOOTSTRAP_VERSION_MAJOR
BUSYBOOTSTRAP_VERSION_MINOR="${BUSYBOOTSTRAP_VERSION%.*}"
BUSYBOOTSTRAP_VERSION_MINOR="${BUSYBOOTSTRAP_VERSION_MINOR#*.}"
export BUSYBOOTSTRAP_VERSION_MINOR
BUSYBOOTSTRAP_VERSION_PATCH="${BUSYBOOTSTRAP_VERSION#*.}"
BUSYBOOTSTRAP_VERSION_PATCH="${BUSYBOOTSTRAP_VERSION_PATCH#*.}"
export BUSYBOOTSTRAP_VERSION_PATCH

if [[ "$1" == sh ]]; then
    shift
    exec sh "$@"
fi

builder="$1"
export builder
shift
exec sh "$builder" "$@"
