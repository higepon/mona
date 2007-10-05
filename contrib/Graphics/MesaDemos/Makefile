MONADIR = ../../../../mona

all: check
	$(MAKE) $@ -C glut
	$(MAKE) $@ -C demos
	$(MAKE) $@ -C redbook
	$(MAKE) $@ -C samples

install: check
	$(MAKE) $@ -C glut
	$(MAKE) $@ -C demos
	$(MAKE) $@ -C redbook
	$(MAKE) $@ -C samples
	mkdir -p $(MONADIR)/bin/MESA
	cp images/*.rgb $(MONADIR)/bin/MESA

clean: check
	$(MAKE) $@ -C glut
	$(MAKE) $@ -C demos
	$(MAKE) $@ -C redbook
	$(MAKE) $@ -C samples

check:
	@if test "$(MONADIR)" = "" ; \
	then \
	    echo "Please set MONADIR!"; \
	    exit 1; \
	fi
