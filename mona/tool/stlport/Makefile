SRCTOP=../..
include $(SRCTOP)/Makefile.inc

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
	$(INSTALL) -m 0644 ./stlport/config/*.h $(INCDIR)/stlport/config
	$(INSTALL) -m 0644 ./stlport/stl/*.h $(INCDIR)/stlport/stl
	$(INSTALL) -m 0644 ./stlport/stl/*.c $(INCDIR)/stlport/stl

uninstall: check
	rm -rf $(INCDIR)/stlport/stl
	rm -rf $(INCDIR)/stlport/config
	rm -rf $(INCDIR)/stlport

check:
	@if test "$(MONADIR)" = "" ; \
	then \
	    echo "Please set MONADIR!"; \
	    exit 1; \
	fi

clean: uninstall

distclean:

