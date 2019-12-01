let busybootstrap = import ../.; in

busybootstrap.derivation {
    system = builtins.currentSystem;
    name = "busybootstrap-test";
    builder = ./builder.sh;
    args = [ "Hello, world!" ];
}
