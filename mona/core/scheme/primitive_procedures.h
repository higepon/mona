#ifndef __PRIMITIVE_PROCEDURES_H__
#define __PRIMITIVE_PROCEDURES_H__
#include "PrimitiveProcedure.h"
namespace monash { 
class CharcterP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterP()  : procedureName_("char?") {}
    virtual ~CharcterP() {}

    virtual ::util::String toString() {    return "#<closure ""char?"">";}
    virtual ::util::String typeString() const { return "char?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CharcterEqualP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterEqualP()  : procedureName_("char=?") {}
    virtual ~CharcterEqualP() {}

    virtual ::util::String toString() {    return "#<closure ""char=?"">";}
    virtual ::util::String typeString() const { return "char=?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CharcterGtP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterGtP()  : procedureName_("char>?") {}
    virtual ~CharcterGtP() {}

    virtual ::util::String toString() {    return "#<closure ""char>?"">";}
    virtual ::util::String typeString() const { return "char>?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CharcterLtP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterLtP()  : procedureName_("char<?") {}
    virtual ~CharcterLtP() {}

    virtual ::util::String toString() {    return "#<closure ""char<?"">";}
    virtual ::util::String typeString() const { return "char<?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CharcterGeP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterGeP()  : procedureName_("char>=?") {}
    virtual ~CharcterGeP() {}

    virtual ::util::String toString() {    return "#<closure ""char>=?"">";}
    virtual ::util::String typeString() const { return "char>=?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CharcterLeP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterLeP()  : procedureName_("char<=?") {}
    virtual ~CharcterLeP() {}

    virtual ::util::String toString() {    return "#<closure ""char<=?"">";}
    virtual ::util::String typeString() const { return "char<=?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CharcterToInteger : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterToInteger()  : procedureName_("char->integer") {}
    virtual ~CharcterToInteger() {}

    virtual ::util::String toString() {    return "#<closure ""char->integer"">";}
    virtual ::util::String typeString() const { return "char->integer"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class IntegerToCharcter : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    IntegerToCharcter()  : procedureName_("integer->char") {}
    virtual ~IntegerToCharcter() {}

    virtual ::util::String toString() {    return "#<closure ""integer->char"">";}
    virtual ::util::String typeString() const { return "integer->char"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class EqvP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    EqvP()  : procedureName_("eqv?") {}
    virtual ~EqvP() {}

    virtual ::util::String toString() {    return "#<closure ""eqv?"">";}
    virtual ::util::String typeString() const { return "eqv?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class EqP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    EqP()  : procedureName_("eq?") {}
    virtual ~EqP() {}

    virtual ::util::String toString() {    return "#<closure ""eq?"">";}
    virtual ::util::String typeString() const { return "eq?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaDirOpen : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaDirOpen()  : procedureName_("mona-dir-open") {}
    virtual ~MonaDirOpen() {}

    virtual ::util::String toString() {    return "#<closure ""mona-dir-open"">";}
    virtual ::util::String typeString() const { return "mona-dir-open"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaDirRead : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaDirRead()  : procedureName_("mona-dir-read") {}
    virtual ~MonaDirRead() {}

    virtual ::util::String toString() {    return "#<closure ""mona-dir-read"">";}
    virtual ::util::String typeString() const { return "mona-dir-read"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaDirClose : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaDirClose()  : procedureName_("mona-dir-close") {}
    virtual ~MonaDirClose() {}

    virtual ::util::String toString() {    return "#<closure ""mona-dir-close"">";}
    virtual ::util::String typeString() const { return "mona-dir-close"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class TranscriptOn : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    TranscriptOn()  : procedureName_("transcript-on") {}
    virtual ~TranscriptOn() {}

    virtual ::util::String toString() {    return "#<closure ""transcript-on"">";}
    virtual ::util::String typeString() const { return "transcript-on"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class TranscriptOff : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    TranscriptOff()  : procedureName_("transcript-off") {}
    virtual ~TranscriptOff() {}

    virtual ::util::String toString() {    return "#<closure ""transcript-off"">";}
    virtual ::util::String typeString() const { return "transcript-off"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class FileExistsP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    FileExistsP()  : procedureName_("file-exists?") {}
    virtual ~FileExistsP() {}

    virtual ::util::String toString() {    return "#<closure ""file-exists?"">";}
    virtual ::util::String typeString() const { return "file-exists?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CharReadyP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharReadyP()  : procedureName_("char-ready?") {}
    virtual ~CharReadyP() {}

    virtual ::util::String toString() {    return "#<closure ""char-ready?"">";}
    virtual ::util::String typeString() const { return "char-ready?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class SetCurrentOutputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SetCurrentOutputPort()  : procedureName_("set-current-output-port!") {}
    virtual ~SetCurrentOutputPort() {}

    virtual ::util::String toString() {    return "#<closure ""set-current-output-port!"">";}
    virtual ::util::String typeString() const { return "set-current-output-port!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class SetCurrentInputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SetCurrentInputPort()  : procedureName_("set-current-input-port!") {}
    virtual ~SetCurrentInputPort() {}

    virtual ::util::String toString() {    return "#<closure ""set-current-input-port!"">";}
    virtual ::util::String typeString() const { return "set-current-input-port!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class InputPortP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    InputPortP()  : procedureName_("input-port?") {}
    virtual ~InputPortP() {}

    virtual ::util::String toString() {    return "#<closure ""input-port?"">";}
    virtual ::util::String typeString() const { return "input-port?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class OutputPortP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    OutputPortP()  : procedureName_("output-port?") {}
    virtual ~OutputPortP() {}

    virtual ::util::String toString() {    return "#<closure ""output-port?"">";}
    virtual ::util::String typeString() const { return "output-port?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class EOFObjectP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    EOFObjectP()  : procedureName_("eof-object?") {}
    virtual ~EOFObjectP() {}

    virtual ::util::String toString() {    return "#<closure ""eof-object?"">";}
    virtual ::util::String typeString() const { return "eof-object?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CloseInputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CloseInputPort()  : procedureName_("close-input-port") {}
    virtual ~CloseInputPort() {}

    virtual ::util::String toString() {    return "#<closure ""close-input-port"">";}
    virtual ::util::String typeString() const { return "close-input-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CloseOutputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CloseOutputPort()  : procedureName_("close-output-port") {}
    virtual ~CloseOutputPort() {}

    virtual ::util::String toString() {    return "#<closure ""close-output-port"">";}
    virtual ::util::String typeString() const { return "close-output-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class OpenOutputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    OpenOutputPort()  : procedureName_("open-output-port") {}
    virtual ~OpenOutputPort() {}

    virtual ::util::String toString() {    return "#<closure ""open-output-port"">";}
    virtual ::util::String typeString() const { return "open-output-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class OpenInputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    OpenInputPort()  : procedureName_("open-input-port") {}
    virtual ~OpenInputPort() {}

    virtual ::util::String toString() {    return "#<closure ""open-input-port"">";}
    virtual ::util::String typeString() const { return "open-input-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CurrentOutputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CurrentOutputPort()  : procedureName_("current-output-port") {}
    virtual ~CurrentOutputPort() {}

    virtual ::util::String toString() {    return "#<closure ""current-output-port"">";}
    virtual ::util::String typeString() const { return "current-output-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CurrentInputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CurrentInputPort()  : procedureName_("current-input-port") {}
    virtual ~CurrentInputPort() {}

    virtual ::util::String toString() {    return "#<closure ""current-input-port"">";}
    virtual ::util::String typeString() const { return "current-input-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class WriteChar : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    WriteChar()  : procedureName_("write-char") {}
    virtual ~WriteChar() {}

    virtual ::util::String toString() {    return "#<closure ""write-char"">";}
    virtual ::util::String typeString() const { return "write-char"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Read : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Read()  : procedureName_("read") {}
    virtual ~Read() {}

    virtual ::util::String toString() {    return "#<closure ""read"">";}
    virtual ::util::String typeString() const { return "read"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class ReadChar : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    ReadChar()  : procedureName_("read-char") {}
    virtual ~ReadChar() {}

    virtual ::util::String toString() {    return "#<closure ""read-char"">";}
    virtual ::util::String typeString() const { return "read-char"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class PeekChar : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    PeekChar()  : procedureName_("peek-char") {}
    virtual ~PeekChar() {}

    virtual ::util::String toString() {    return "#<closure ""peek-char"">";}
    virtual ::util::String typeString() const { return "peek-char"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Write : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Write()  : procedureName_("write") {}
    virtual ~Write() {}

    virtual ::util::String toString() {    return "#<closure ""write"">";}
    virtual ::util::String typeString() const { return "write"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NewLine : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NewLine()  : procedureName_("newline") {}
    virtual ~NewLine() {}

    virtual ::util::String toString() {    return "#<closure ""newline"">";}
    virtual ::util::String typeString() const { return "newline"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Display : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Display()  : procedureName_("display") {}
    virtual ~Display() {}

    virtual ::util::String toString() {    return "#<closure ""display"">";}
    virtual ::util::String typeString() const { return "display"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Load : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Load()  : procedureName_("load") {}
    virtual ~Load() {}

    virtual ::util::String toString() {    return "#<closure ""load"">";}
    virtual ::util::String typeString() const { return "load"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NumberP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberP()  : procedureName_("number?") {}
    virtual ~NumberP() {}

    virtual ::util::String toString() {    return "#<closure ""number?"">";}
    virtual ::util::String typeString() const { return "number?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Plus : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Plus()  : procedureName_("+") {}
    virtual ~Plus() {}

    virtual ::util::String toString() {    return "#<closure ""+"">";}
    virtual ::util::String typeString() const { return "+"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Minus : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Minus()  : procedureName_("-") {}
    virtual ~Minus() {}

    virtual ::util::String toString() {    return "#<closure ""-"">";}
    virtual ::util::String typeString() const { return "-"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NumberToString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberToString()  : procedureName_("number->string") {}
    virtual ~NumberToString() {}

    virtual ::util::String toString() {    return "#<closure ""number->string"">";}
    virtual ::util::String typeString() const { return "number->string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NumberEqual : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberEqual()  : procedureName_("=") {}
    virtual ~NumberEqual() {}

    virtual ::util::String toString() {    return "#<closure ""="">";}
    virtual ::util::String typeString() const { return "="; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NumberGt : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberGt()  : procedureName_(">") {}
    virtual ~NumberGt() {}

    virtual ::util::String toString() {    return "#<closure "">"">";}
    virtual ::util::String typeString() const { return ">"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NumberLt : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberLt()  : procedureName_("<") {}
    virtual ~NumberLt() {}

    virtual ::util::String toString() {    return "#<closure ""<"">";}
    virtual ::util::String typeString() const { return "<"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NumberGe : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberGe()  : procedureName_(">=") {}
    virtual ~NumberGe() {}

    virtual ::util::String toString() {    return "#<closure "">="">";}
    virtual ::util::String typeString() const { return ">="; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NumberLe : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberLe()  : procedureName_("<=") {}
    virtual ~NumberLe() {}

    virtual ::util::String toString() {    return "#<closure ""<="">";}
    virtual ::util::String typeString() const { return "<="; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Multiply : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Multiply()  : procedureName_("*") {}
    virtual ~Multiply() {}

    virtual ::util::String toString() {    return "#<closure ""*"">";}
    virtual ::util::String typeString() const { return "*"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Divide : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Divide()  : procedureName_("/") {}
    virtual ~Divide() {}

    virtual ::util::String toString() {    return "#<closure ""/"">";}
    virtual ::util::String typeString() const { return "/"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class PairP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    PairP()  : procedureName_("pair?") {}
    virtual ~PairP() {}

    virtual ::util::String toString() {    return "#<closure ""pair?"">";}
    virtual ::util::String typeString() const { return "pair?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Cons : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Cons()  : procedureName_("cons") {}
    virtual ~Cons() {}

    virtual ::util::String toString() {    return "#<closure ""cons"">";}
    virtual ::util::String typeString() const { return "cons"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Car : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Car()  : procedureName_("car") {}
    virtual ~Car() {}

    virtual ::util::String toString() {    return "#<closure ""car"">";}
    virtual ::util::String typeString() const { return "car"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Cdr : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Cdr()  : procedureName_("cdr") {}
    virtual ~Cdr() {}

    virtual ::util::String toString() {    return "#<closure ""cdr"">";}
    virtual ::util::String typeString() const { return "cdr"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class SetCar : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SetCar()  : procedureName_("set-car!") {}
    virtual ~SetCar() {}

    virtual ::util::String toString() {    return "#<closure ""set-car!"">";}
    virtual ::util::String typeString() const { return "set-car!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class SetCdr : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SetCdr()  : procedureName_("set-cdr!") {}
    virtual ~SetCdr() {}

    virtual ::util::String toString() {    return "#<closure ""set-cdr!"">";}
    virtual ::util::String typeString() const { return "set-cdr!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringToRegexp : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringToRegexp()  : procedureName_("string->regexp") {}
    virtual ~StringToRegexp() {}

    virtual ::util::String toString() {    return "#<closure ""string->regexp"">";}
    virtual ::util::String typeString() const { return "string->regexp"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringCopy : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringCopy()  : procedureName_("string-copy") {}
    virtual ~StringCopy() {}

    virtual ::util::String toString() {    return "#<closure ""string-copy"">";}
    virtual ::util::String typeString() const { return "string-copy"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringP()  : procedureName_("string?") {}
    virtual ~StringP() {}

    virtual ::util::String toString() {    return "#<closure ""string?"">";}
    virtual ::util::String typeString() const { return "string?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MakeString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MakeString()  : procedureName_("make-string") {}
    virtual ~MakeString() {}

    virtual ::util::String toString() {    return "#<closure ""make-string"">";}
    virtual ::util::String typeString() const { return "make-string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringLength : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringLength()  : procedureName_("string-length") {}
    virtual ~StringLength() {}

    virtual ::util::String toString() {    return "#<closure ""string-length"">";}
    virtual ::util::String typeString() const { return "string-length"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringRef : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringRef()  : procedureName_("string-ref") {}
    virtual ~StringRef() {}

    virtual ::util::String toString() {    return "#<closure ""string-ref"">";}
    virtual ::util::String typeString() const { return "string-ref"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringSet : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringSet()  : procedureName_("string-set!") {}
    virtual ~StringSet() {}

    virtual ::util::String toString() {    return "#<closure ""string-set!"">";}
    virtual ::util::String typeString() const { return "string-set!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringEqualP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringEqualP()  : procedureName_("string=?") {}
    virtual ~StringEqualP() {}

    virtual ::util::String toString() {    return "#<closure ""string=?"">";}
    virtual ::util::String typeString() const { return "string=?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringToNumber : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringToNumber()  : procedureName_("string->number") {}
    virtual ~StringToNumber() {}

    virtual ::util::String toString() {    return "#<closure ""string->number"">";}
    virtual ::util::String typeString() const { return "string->number"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringSplit : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringSplit()  : procedureName_("string-split") {}
    virtual ~StringSplit() {}

    virtual ::util::String toString() {    return "#<closure ""string-split"">";}
    virtual ::util::String typeString() const { return "string-split"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StringToSymbol : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringToSymbol()  : procedureName_("string->symbol") {}
    virtual ~StringToSymbol() {}

    virtual ::util::String toString() {    return "#<closure ""string->symbol"">";}
    virtual ::util::String typeString() const { return "string->symbol"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class RegexpP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RegexpP()  : procedureName_("regexp?") {}
    virtual ~RegexpP() {}

    virtual ::util::String toString() {    return "#<closure ""regexp?"">";}
    virtual ::util::String typeString() const { return "regexp?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class RxMatchNumMatches : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchNumMatches()  : procedureName_("rxmatch-num-matches") {}
    virtual ~RxMatchNumMatches() {}

    virtual ::util::String toString() {    return "#<closure ""rxmatch-num-matches"">";}
    virtual ::util::String typeString() const { return "rxmatch-num-matches"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class RegexpToString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RegexpToString()  : procedureName_("regexp->string") {}
    virtual ~RegexpToString() {}

    virtual ::util::String toString() {    return "#<closure ""regexp->string"">";}
    virtual ::util::String typeString() const { return "regexp->string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class RxMatch : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatch()  : procedureName_("rxmatch") {}
    virtual ~RxMatch() {}

    virtual ::util::String toString() {    return "#<closure ""rxmatch"">";}
    virtual ::util::String typeString() const { return "rxmatch"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class RxMatchStart : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchStart()  : procedureName_("rxmatch-start") {}
    virtual ~RxMatchStart() {}

    virtual ::util::String toString() {    return "#<closure ""rxmatch-start"">";}
    virtual ::util::String typeString() const { return "rxmatch-start"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class RxMatchEnd : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchEnd()  : procedureName_("rxmatch-end") {}
    virtual ~RxMatchEnd() {}

    virtual ::util::String toString() {    return "#<closure ""rxmatch-end"">";}
    virtual ::util::String typeString() const { return "rxmatch-end"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class RxMatchSubString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchSubString()  : procedureName_("rxmatch-substring") {}
    virtual ~RxMatchSubString() {}

    virtual ::util::String toString() {    return "#<closure ""rxmatch-substring"">";}
    virtual ::util::String typeString() const { return "rxmatch-substring"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class RxMatchAfter : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchAfter()  : procedureName_("rxmatch-after") {}
    virtual ~RxMatchAfter() {}

    virtual ::util::String toString() {    return "#<closure ""rxmatch-after"">";}
    virtual ::util::String typeString() const { return "rxmatch-after"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class RxMatchBefore : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchBefore()  : procedureName_("rxmatch-before") {}
    virtual ~RxMatchBefore() {}

    virtual ::util::String toString() {    return "#<closure ""rxmatch-before"">";}
    virtual ::util::String typeString() const { return "rxmatch-before"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class SymbolP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SymbolP()  : procedureName_("symbol?") {}
    virtual ~SymbolP() {}

    virtual ::util::String toString() {    return "#<closure ""symbol?"">";}
    virtual ::util::String typeString() const { return "symbol?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class SymbolToString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SymbolToString()  : procedureName_("symbol->string") {}
    virtual ~SymbolToString() {}

    virtual ::util::String toString() {    return "#<closure ""symbol->string"">";}
    virtual ::util::String typeString() const { return "symbol->string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class VectorP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    VectorP()  : procedureName_("vector?") {}
    virtual ~VectorP() {}

    virtual ::util::String toString() {    return "#<closure ""vector?"">";}
    virtual ::util::String typeString() const { return "vector?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MakeVector : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MakeVector()  : procedureName_("make-vector") {}
    virtual ~MakeVector() {}

    virtual ::util::String toString() {    return "#<closure ""make-vector"">";}
    virtual ::util::String typeString() const { return "make-vector"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CreateVector : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CreateVector()  : procedureName_("vector") {}
    virtual ~CreateVector() {}

    virtual ::util::String toString() {    return "#<closure ""vector"">";}
    virtual ::util::String typeString() const { return "vector"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class VectorLength : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    VectorLength()  : procedureName_("vector-length") {}
    virtual ~VectorLength() {}

    virtual ::util::String toString() {    return "#<closure ""vector-length"">";}
    virtual ::util::String typeString() const { return "vector-length"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class VectorRef : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    VectorRef()  : procedureName_("vector-ref") {}
    virtual ~VectorRef() {}

    virtual ::util::String toString() {    return "#<closure ""vector-ref"">";}
    virtual ::util::String typeString() const { return "vector-ref"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class VectorSet : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    VectorSet()  : procedureName_("vector-set!") {}
    virtual ~VectorSet() {}

    virtual ::util::String toString() {    return "#<closure ""vector-set!"">";}
    virtual ::util::String typeString() const { return "vector-set!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class DynamicWindProc : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    DynamicWindProc()  : procedureName_("dynamic-wind") {}
    virtual ~DynamicWindProc() {}

    virtual ::util::String toString() {    return "#<closure ""dynamic-wind"">";}
    virtual ::util::String typeString() const { return "dynamic-wind"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CallWithValues : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallWithValues()  : procedureName_("call-with-values") {}
    virtual ~CallWithValues() {}

    virtual ::util::String toString() {    return "#<closure ""call-with-values"">";}
    virtual ::util::String typeString() const { return "call-with-values"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CallWithCurrentContinuation : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallWithCurrentContinuation()  : procedureName_("call-with-current-continuation") {}
    virtual ~CallWithCurrentContinuation() {}

    virtual ::util::String toString() {    return "#<closure ""call-with-current-continuation"">";}
    virtual ::util::String typeString() const { return "call-with-current-continuation"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NotSupported : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NotSupported()  : procedureName_("not-supported") {}
    virtual ~NotSupported() {}

    virtual ::util::String toString() {    return "#<closure ""not-supported"">";}
    virtual ::util::String typeString() const { return "not-supported"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class ProcedureP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    ProcedureP()  : procedureName_("procedure?") {}
    virtual ~ProcedureP() {}

    virtual ::util::String toString() {    return "#<closure ""procedure?"">";}
    virtual ::util::String typeString() const { return "procedure?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class BooleanP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    BooleanP()  : procedureName_("boolean?") {}
    virtual ~BooleanP() {}

    virtual ::util::String toString() {    return "#<closure ""boolean?"">";}
    virtual ::util::String typeString() const { return "boolean?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Exit : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Exit()  : procedureName_("exit") {}
    virtual ~Exit() {}

    virtual ::util::String toString() {    return "#<closure ""exit"">";}
    virtual ::util::String typeString() const { return "exit"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Apply : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Apply()  : procedureName_("apply") {}
    virtual ~Apply() {}

    virtual ::util::String toString() {    return "#<closure ""apply"">";}
    virtual ::util::String typeString() const { return "apply"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class Eval : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Eval()  : procedureName_("eval") {}
    virtual ~Eval() {}

    virtual ::util::String toString() {    return "#<closure ""eval"">";}
    virtual ::util::String typeString() const { return "eval"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class NullEnvironment : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NullEnvironment()  : procedureName_("null-environment") {}
    virtual ~NullEnvironment() {}

    virtual ::util::String toString() {    return "#<closure ""null-environment"">";}
    virtual ::util::String typeString() const { return "null-environment"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class SchemeReportEnvironment : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SchemeReportEnvironment()  : procedureName_("scheme-report-environment") {}
    virtual ~SchemeReportEnvironment() {}

    virtual ::util::String toString() {    return "#<closure ""scheme-report-environment"">";}
    virtual ::util::String typeString() const { return "scheme-report-environment"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class InteractionEnvironment : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    InteractionEnvironment()  : procedureName_("interaction-environment") {}
    virtual ~InteractionEnvironment() {}

    virtual ::util::String toString() {    return "#<closure ""interaction-environment"">";}
    virtual ::util::String typeString() const { return "interaction-environment"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CallPipe : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallPipe()  : procedureName_("|") {}
    virtual ~CallPipe() {}

    virtual ::util::String toString() {    return "#<closure ""|"">";}
    virtual ::util::String typeString() const { return "|"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CallProcess : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallProcess()  : procedureName_("call-process") {}
    virtual ~CallProcess() {}

    virtual ::util::String toString() {    return "#<closure ""call-process"">";}
    virtual ::util::String typeString() const { return "call-process"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class StartProcess : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StartProcess()  : procedureName_("start-process") {}
    virtual ~StartProcess() {}

    virtual ::util::String toString() {    return "#<closure ""start-process"">";}
    virtual ::util::String typeString() const { return "start-process"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class CallProcessOutString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallProcessOutString()  : procedureName_("call-process-out-string") {}
    virtual ~CallProcessOutString() {}

    virtual ::util::String toString() {    return "#<closure ""call-process-out-string"">";}
    virtual ::util::String typeString() const { return "call-process-out-string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaGuiMoveWindow : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaGuiMoveWindow()  : procedureName_("mona-gui-move-window") {}
    virtual ~MonaGuiMoveWindow() {}

    virtual ::util::String toString() {    return "#<closure ""mona-gui-move-window"">";}
    virtual ::util::String typeString() const { return "mona-gui-move-window"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaGuiGetWindowTitle : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaGuiGetWindowTitle()  : procedureName_("mona-gui-get-window-title") {}
    virtual ~MonaGuiGetWindowTitle() {}

    virtual ::util::String toString() {    return "#<closure ""mona-gui-get-window-title"">";}
    virtual ::util::String typeString() const { return "mona-gui-get-window-title"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaGuiEnumWindows : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaGuiEnumWindows()  : procedureName_("mona-gui-enum-windows") {}
    virtual ~MonaGuiEnumWindows() {}

    virtual ::util::String toString() {    return "#<closure ""mona-gui-enum-windows"">";}
    virtual ::util::String typeString() const { return "mona-gui-enum-windows"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaHalt : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaHalt()  : procedureName_("mona-halt") {}
    virtual ~MonaHalt() {}

    virtual ::util::String toString() {    return "#<closure ""mona-halt"">";}
    virtual ::util::String typeString() const { return "mona-halt"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaReboot : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaReboot()  : procedureName_("mona-reboot") {}
    virtual ~MonaReboot() {}

    virtual ::util::String toString() {    return "#<closure ""mona-reboot"">";}
    virtual ::util::String typeString() const { return "mona-reboot"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaKill : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaKill()  : procedureName_("mona-kill") {}
    virtual ~MonaKill() {}

    virtual ::util::String toString() {    return "#<closure ""mona-kill"">";}
    virtual ::util::String typeString() const { return "mona-kill"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaSleep : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaSleep()  : procedureName_("mona-sleep") {}
    virtual ~MonaSleep() {}

    virtual ::util::String toString() {    return "#<closure ""mona-sleep"">";}
    virtual ::util::String typeString() const { return "mona-sleep"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

class MonaPs : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaPs()  : procedureName_("mona-ps") {}
    virtual ~MonaPs() {}

    virtual ::util::String toString() {    return "#<closure ""mona-ps"">";}
    virtual ::util::String typeString() const { return "mona-ps"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env);
};

};
#endif
