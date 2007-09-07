REGISTERS_INC = ./register.inc
PROCEDURES_H  = ./primitive_procedures.h

SOURCES = $(PROCEDURE_SOURCES) \
Application.cpp\
Assignment.cpp\
Begin.cpp\
Charcter.cpp\
Cond.cpp\
Definition.cpp\
Environment.cpp\
Error.cpp\
Frame.cpp\
Lambda.cpp\
Let.cpp\
NamedLet.cpp\
LetAsterisk.cpp\
Number.cpp\
Continuation.cpp\
Object.cpp\
Cons.cpp\
OutputPort.cpp\
InputPort.cpp\
Values.cpp\
Set.cpp \
Procedure.cpp\
Identifier.cpp\
Kernel.cpp\
SIf.cpp\
And.cpp\
Or.cpp\
QuasiQuote.cpp\
Unquote.cpp\
UnquoteSplicing.cpp\
SString.cpp\
Translator.cpp\
TraditionalMacro.cpp\
Variable.cpp\
Vector.cpp\
Scanner.cpp\
Parser.cpp\
Token.cpp\
StringReader.cpp\
Variant.cpp\
load.cpp\
Interaction.cpp\
SRegexp.cpp\
SRegMatch.cpp\
MonaTerminal.cpp\
histories.cpp\
scheme.cpp

PROCEDURE_SOURCES =\
procedures/CharcterProcedures.cpp\
procedures/EqualProcedures.cpp\
procedures/IOProcedures.cpp\
procedures/NumberProcedures.cpp\
procedures/PairProcedures.cpp\
procedures/StringProcedures.cpp\
procedures/RegexpProcedures.cpp\
procedures/SymbolProcedures.cpp\
procedures/VectorProcedures.cpp\
procedures/MiscProcedures.cpp

LIBCONT_SOURCES = libcont/cont.c libcont/mysetjmp.c
LIBCONT_OBJECTS = $(LIBCONT_SOURCES:.c=.o) libcont/mysetjmp_asm.o
