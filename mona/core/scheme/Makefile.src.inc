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
Macro.cpp\
MacroFilter.cpp\
Number.cpp\
Continuation.cpp\
Object.cpp\
Pair.cpp\
OutputPort.cpp\
InputPort.cpp\
Values.cpp\
Parser.cpp\
Procedure.cpp\
RiteralConstant.cpp\
Nil.cpp\
QuoteFilter.cpp\
SExp.cpp\
Kernel.cpp\
SpecialIf.cpp\
SString.cpp\
Tokenizer.cpp\
Translator.cpp\
Variable.cpp\
Vector.cpp\
Scanner.cpp\
SToken.cpp\
StringReader.cpp\
ExtRepParser.cpp\
Eof.cpp\
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
procedures/False.cpp\
procedures/IOProcedures.cpp\
procedures/NumberProcedures.cpp\
procedures/PairProcedures.cpp\
procedures/Set.cpp\
procedures/StringProcedures.cpp\
procedures/RegexpProcedures.cpp\
procedures/SymbolProcedures.cpp\
procedures/True.cpp\
procedures/Undef.cpp\
procedures/VectorProcedures.cpp\
procedures/MiscProcedures.cpp

LIBCONT_SOURCES = libcont/cont.c libcont/mysetjmp.c
LIBCONT_OBJECTS = $(LIBCONT_SOURCES:.c=.o) libcont/mysetjmp_asm.o
