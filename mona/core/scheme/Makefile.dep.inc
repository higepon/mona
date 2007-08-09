libcont/cont.o: libcont/cont.c
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $@

libcont/mysetjmp_asm.o: libcont/mysetjmp.asm
	nasm -I . -I ./libcont $(NFLAGS) $< -o $@

$(REGISTERS_INC): $(PROCEDURE_SOURCES) ./test/generate.scm
	gosh ./test/generate.scm --register $(PROCEDURE_SOURCES) > $@

$(PROCEDURES_H): $(PROCEDURE_SOURCES) ./test/generate.scm
	gosh ./test/generate.scm --header $(PROCEDURE_SOURCES) > $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

