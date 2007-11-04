libcont/cont.o: libcont/cont.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

libcont/mysetjmp_asm.o: libcont/mysetjmp.asm
	nasm -I . -I ./libcont $(NFLAGS) $< -o $@

ifdef GOSH
$(REGISTERS_INC): $(PROCEDURE_SOURCES) ./test/generate.scm
	$(GOSH) ./test/generate.scm --register $(PROCEDURE_SOURCES) > $@

$(PROCEDURES_H): $(PROCEDURE_SOURCES) ./test/generate.scm
	$(GOSH) ./test/generate.scm --header $(PROCEDURE_SOURCES) > $@
endif

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

