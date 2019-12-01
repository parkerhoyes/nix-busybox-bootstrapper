NIXOPTS :=

all:
	cp --no-clobber busybox-config.cfg busybox/.config
	$(MAKE) -C busybox
	$(MAKE) -C bootstrapper

clean:
	$(MAKE) -C busybox distclean
	$(MAKE) -C bootstrapper clean

test: all
	nix $(NIXOPTS) build -f tests -o busybootstrap-test

.PHONY: all clean test
