include $(MONADIR)/share/configs/Makefile.inc
include $(MONADIR)/share/configs/dirnames.inc

all:
	@echo "Nothing to do. Please install."

install: check
	mkdir -p $(INCDIR)/stlport
	mkdir -p $(INCDIR)/stlport/config
	mkdir -p $(INCDIR)/stlport/stl
	$(INSTALL) -m 0644 \
	./stlport/algorithm \
	./stlport/deque \
	./stlport/functional \
	./stlport/hash_map \
	./stlport/hash_set \
	./stlport/list \
	./stlport/map \
	./stlport/memory \
	./stlport/new \
	./stlport/numeric \
	./stlport/queue \
	./stlport/set \
	./stlport/slist \
	./stlport/stack \
	./stlport/stl_user_config.h \
	./stlport/string \
	./stlport/vector \
	$(INCDIR)/stlport
	$(INSTALL) -m 0644 ./stlport/config/* $(INCDIR)/stlport/config
	$(INSTALL) -m 0644 ./stlport/stl/* $(INCDIR)/stlport/stl

uninstall: check
	rm -r $(INCDIR)/stlport/stl
	rm -r $(INCDIR)/stlport/config
	rm -r $(INCDIR)/stlport

check:
	@if test "$(MONADIR)" = "" ; \
	then \
	    echo "Please set MONADIR!"; \
	    exit 1; \
	fi

clean: uninstall

distclean:

