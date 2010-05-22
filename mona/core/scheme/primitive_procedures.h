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

    virtual ::util::String toString() {    return "#<subr ""char?"">";}
    virtual ::util::String typeString() const { return "char?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CharcterEqualP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterEqualP()  : procedureName_("char=?") {}
    virtual ~CharcterEqualP() {}

    virtual ::util::String toString() {    return "#<subr ""char=?"">";}
    virtual ::util::String typeString() const { return "char=?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CharcterGtP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterGtP()  : procedureName_("char>?") {}
    virtual ~CharcterGtP() {}

    virtual ::util::String toString() {    return "#<subr ""char>?"">";}
    virtual ::util::String typeString() const { return "char>?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CharcterLtP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterLtP()  : procedureName_("char<?") {}
    virtual ~CharcterLtP() {}

    virtual ::util::String toString() {    return "#<subr ""char<?"">";}
    virtual ::util::String typeString() const { return "char<?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CharcterGeP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterGeP()  : procedureName_("char>=?") {}
    virtual ~CharcterGeP() {}

    virtual ::util::String toString() {    return "#<subr ""char>=?"">";}
    virtual ::util::String typeString() const { return "char>=?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CharcterLeP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterLeP()  : procedureName_("char<=?") {}
    virtual ~CharcterLeP() {}

    virtual ::util::String toString() {    return "#<subr ""char<=?"">";}
    virtual ::util::String typeString() const { return "char<=?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CharcterToInteger : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharcterToInteger()  : procedureName_("char->integer") {}
    virtual ~CharcterToInteger() {}

    virtual ::util::String toString() {    return "#<subr ""char->integer"">";}
    virtual ::util::String typeString() const { return "char->integer"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class IntegerToCharcter : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    IntegerToCharcter()  : procedureName_("integer->char") {}
    virtual ~IntegerToCharcter() {}

    virtual ::util::String toString() {    return "#<subr ""integer->char"">";}
    virtual ::util::String typeString() const { return "integer->char"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class EqvP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    EqvP()  : procedureName_("eqv?") {}
    virtual ~EqvP() {}

    virtual ::util::String toString() {    return "#<subr ""eqv?"">";}
    virtual ::util::String typeString() const { return "eqv?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class EqP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    EqP()  : procedureName_("eq?") {}
    virtual ~EqP() {}

    virtual ::util::String toString() {    return "#<subr ""eq?"">";}
    virtual ::util::String typeString() const { return "eq?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaDirOpen : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaDirOpen()  : procedureName_("mona-dir-open") {}
    virtual ~MonaDirOpen() {}

    virtual ::util::String toString() {    return "#<subr ""mona-dir-open"">";}
    virtual ::util::String typeString() const { return "mona-dir-open"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaDirRead : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaDirRead()  : procedureName_("mona-dir-read") {}
    virtual ~MonaDirRead() {}

    virtual ::util::String toString() {    return "#<subr ""mona-dir-read"">";}
    virtual ::util::String typeString() const { return "mona-dir-read"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaDirClose : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaDirClose()  : procedureName_("mona-dir-close") {}
    virtual ~MonaDirClose() {}

    virtual ::util::String toString() {    return "#<subr ""mona-dir-close"">";}
    virtual ::util::String typeString() const { return "mona-dir-close"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class TranscriptOn : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    TranscriptOn()  : procedureName_("transcript-on") {}
    virtual ~TranscriptOn() {}

    virtual ::util::String toString() {    return "#<subr ""transcript-on"">";}
    virtual ::util::String typeString() const { return "transcript-on"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class TranscriptOff : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    TranscriptOff()  : procedureName_("transcript-off") {}
    virtual ~TranscriptOff() {}

    virtual ::util::String toString() {    return "#<subr ""transcript-off"">";}
    virtual ::util::String typeString() const { return "transcript-off"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class FileExistsP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    FileExistsP()  : procedureName_("file-exists?") {}
    virtual ~FileExistsP() {}

    virtual ::util::String toString() {    return "#<subr ""file-exists?"">";}
    virtual ::util::String typeString() const { return "file-exists?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CharReadyP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CharReadyP()  : procedureName_("char-ready?") {}
    virtual ~CharReadyP() {}

    virtual ::util::String toString() {    return "#<subr ""char-ready?"">";}
    virtual ::util::String typeString() const { return "char-ready?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class SetCurrentOutputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SetCurrentOutputPort()  : procedureName_("set-current-output-port!") {}
    virtual ~SetCurrentOutputPort() {}

    virtual ::util::String toString() {    return "#<subr ""set-current-output-port!"">";}
    virtual ::util::String typeString() const { return "set-current-output-port!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class SetCurrentInputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SetCurrentInputPort()  : procedureName_("set-current-input-port!") {}
    virtual ~SetCurrentInputPort() {}

    virtual ::util::String toString() {    return "#<subr ""set-current-input-port!"">";}
    virtual ::util::String typeString() const { return "set-current-input-port!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class InputPortP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    InputPortP()  : procedureName_("input-port?") {}
    virtual ~InputPortP() {}

    virtual ::util::String toString() {    return "#<subr ""input-port?"">";}
    virtual ::util::String typeString() const { return "input-port?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class OutputPortP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    OutputPortP()  : procedureName_("output-port?") {}
    virtual ~OutputPortP() {}

    virtual ::util::String toString() {    return "#<subr ""output-port?"">";}
    virtual ::util::String typeString() const { return "output-port?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class EOFObjectP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    EOFObjectP()  : procedureName_("eof-object?") {}
    virtual ~EOFObjectP() {}

    virtual ::util::String toString() {    return "#<subr ""eof-object?"">";}
    virtual ::util::String typeString() const { return "eof-object?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CloseInputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CloseInputPort()  : procedureName_("close-input-port") {}
    virtual ~CloseInputPort() {}

    virtual ::util::String toString() {    return "#<subr ""close-input-port"">";}
    virtual ::util::String typeString() const { return "close-input-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CloseOutputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CloseOutputPort()  : procedureName_("close-output-port") {}
    virtual ~CloseOutputPort() {}

    virtual ::util::String toString() {    return "#<subr ""close-output-port"">";}
    virtual ::util::String typeString() const { return "close-output-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class OpenOutputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    OpenOutputPort()  : procedureName_("open-output-port") {}
    virtual ~OpenOutputPort() {}

    virtual ::util::String toString() {    return "#<subr ""open-output-port"">";}
    virtual ::util::String typeString() const { return "open-output-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class OpenInputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    OpenInputPort()  : procedureName_("open-input-port") {}
    virtual ~OpenInputPort() {}

    virtual ::util::String toString() {    return "#<subr ""open-input-port"">";}
    virtual ::util::String typeString() const { return "open-input-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CurrentOutputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CurrentOutputPort()  : procedureName_("current-output-port") {}
    virtual ~CurrentOutputPort() {}

    virtual ::util::String toString() {    return "#<subr ""current-output-port"">";}
    virtual ::util::String typeString() const { return "current-output-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CurrentInputPort : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CurrentInputPort()  : procedureName_("current-input-port") {}
    virtual ~CurrentInputPort() {}

    virtual ::util::String toString() {    return "#<subr ""current-input-port"">";}
    virtual ::util::String typeString() const { return "current-input-port"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class WriteChar : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    WriteChar()  : procedureName_("write-char") {}
    virtual ~WriteChar() {}

    virtual ::util::String toString() {    return "#<subr ""write-char"">";}
    virtual ::util::String typeString() const { return "write-char"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Read : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Read()  : procedureName_("read") {}
    virtual ~Read() {}

    virtual ::util::String toString() {    return "#<subr ""read"">";}
    virtual ::util::String typeString() const { return "read"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class ReadChar : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    ReadChar()  : procedureName_("read-char") {}
    virtual ~ReadChar() {}

    virtual ::util::String toString() {    return "#<subr ""read-char"">";}
    virtual ::util::String typeString() const { return "read-char"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class PeekChar : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    PeekChar()  : procedureName_("peek-char") {}
    virtual ~PeekChar() {}

    virtual ::util::String toString() {    return "#<subr ""peek-char"">";}
    virtual ::util::String typeString() const { return "peek-char"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Write : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Write()  : procedureName_("write") {}
    virtual ~Write() {}

    virtual ::util::String toString() {    return "#<subr ""write"">";}
    virtual ::util::String typeString() const { return "write"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NewLine : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NewLine()  : procedureName_("newline") {}
    virtual ~NewLine() {}

    virtual ::util::String toString() {    return "#<subr ""newline"">";}
    virtual ::util::String typeString() const { return "newline"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Display : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Display()  : procedureName_("display") {}
    virtual ~Display() {}

    virtual ::util::String toString() {    return "#<subr ""display"">";}
    virtual ::util::String typeString() const { return "display"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Load : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Load()  : procedureName_("load") {}
    virtual ~Load() {}

    virtual ::util::String toString() {    return "#<subr ""load"">";}
    virtual ::util::String typeString() const { return "load"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NumberP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberP()  : procedureName_("number?") {}
    virtual ~NumberP() {}

    virtual ::util::String toString() {    return "#<subr ""number?"">";}
    virtual ::util::String typeString() const { return "number?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Plus : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Plus()  : procedureName_("+") {}
    virtual ~Plus() {}

    virtual ::util::String toString() {    return "#<subr ""+"">";}
    virtual ::util::String typeString() const { return "+"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Minus : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Minus()  : procedureName_("-") {}
    virtual ~Minus() {}

    virtual ::util::String toString() {    return "#<subr ""-"">";}
    virtual ::util::String typeString() const { return "-"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NumberToString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberToString()  : procedureName_("number->string") {}
    virtual ~NumberToString() {}

    virtual ::util::String toString() {    return "#<subr ""number->string"">";}
    virtual ::util::String typeString() const { return "number->string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NumberEqual : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberEqual()  : procedureName_("=") {}
    virtual ~NumberEqual() {}

    virtual ::util::String toString() {    return "#<subr ""="">";}
    virtual ::util::String typeString() const { return "="; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NumberGt : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberGt()  : procedureName_(">") {}
    virtual ~NumberGt() {}

    virtual ::util::String toString() {    return "#<subr "">"">";}
    virtual ::util::String typeString() const { return ">"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NumberLt : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberLt()  : procedureName_("<") {}
    virtual ~NumberLt() {}

    virtual ::util::String toString() {    return "#<subr ""<"">";}
    virtual ::util::String typeString() const { return "<"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NumberGe : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberGe()  : procedureName_(">=") {}
    virtual ~NumberGe() {}

    virtual ::util::String toString() {    return "#<subr "">="">";}
    virtual ::util::String typeString() const { return ">="; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NumberLe : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NumberLe()  : procedureName_("<=") {}
    virtual ~NumberLe() {}

    virtual ::util::String toString() {    return "#<subr ""<="">";}
    virtual ::util::String typeString() const { return "<="; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Multiply : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Multiply()  : procedureName_("*") {}
    virtual ~Multiply() {}

    virtual ::util::String toString() {    return "#<subr ""*"">";}
    virtual ::util::String typeString() const { return "*"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Divide : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Divide()  : procedureName_("/") {}
    virtual ~Divide() {}

    virtual ::util::String toString() {    return "#<subr ""/"">";}
    virtual ::util::String typeString() const { return "/"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class PairP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    PairP()  : procedureName_("pair?") {}
    virtual ~PairP() {}

    virtual ::util::String toString() {    return "#<subr ""pair?"">";}
    virtual ::util::String typeString() const { return "pair?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class SCons : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SCons()  : procedureName_("cons") {}
    virtual ~SCons() {}

    virtual ::util::String toString() {    return "#<subr ""cons"">";}
    virtual ::util::String typeString() const { return "cons"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Car : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Car()  : procedureName_("car") {}
    virtual ~Car() {}

    virtual ::util::String toString() {    return "#<subr ""car"">";}
    virtual ::util::String typeString() const { return "car"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Cdr : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Cdr()  : procedureName_("cdr") {}
    virtual ~Cdr() {}

    virtual ::util::String toString() {    return "#<subr ""cdr"">";}
    virtual ::util::String typeString() const { return "cdr"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class SetCar : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SetCar()  : procedureName_("set-car!") {}
    virtual ~SetCar() {}

    virtual ::util::String toString() {    return "#<subr ""set-car!"">";}
    virtual ::util::String typeString() const { return "set-car!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class SetCdr : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SetCdr()  : procedureName_("set-cdr!") {}
    virtual ~SetCdr() {}

    virtual ::util::String toString() {    return "#<subr ""set-cdr!"">";}
    virtual ::util::String typeString() const { return "set-cdr!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringToRegexp : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringToRegexp()  : procedureName_("string->regexp") {}
    virtual ~StringToRegexp() {}

    virtual ::util::String toString() {    return "#<subr ""string->regexp"">";}
    virtual ::util::String typeString() const { return "string->regexp"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringCopy : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringCopy()  : procedureName_("string-copy") {}
    virtual ~StringCopy() {}

    virtual ::util::String toString() {    return "#<subr ""string-copy"">";}
    virtual ::util::String typeString() const { return "string-copy"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringP()  : procedureName_("string?") {}
    virtual ~StringP() {}

    virtual ::util::String toString() {    return "#<subr ""string?"">";}
    virtual ::util::String typeString() const { return "string?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MakeString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MakeString()  : procedureName_("make-string") {}
    virtual ~MakeString() {}

    virtual ::util::String toString() {    return "#<subr ""make-string"">";}
    virtual ::util::String typeString() const { return "make-string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringLength : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringLength()  : procedureName_("string-length") {}
    virtual ~StringLength() {}

    virtual ::util::String toString() {    return "#<subr ""string-length"">";}
    virtual ::util::String typeString() const { return "string-length"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringRef : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringRef()  : procedureName_("string-ref") {}
    virtual ~StringRef() {}

    virtual ::util::String toString() {    return "#<subr ""string-ref"">";}
    virtual ::util::String typeString() const { return "string-ref"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringSet : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringSet()  : procedureName_("string-set!") {}
    virtual ~StringSet() {}

    virtual ::util::String toString() {    return "#<subr ""string-set!"">";}
    virtual ::util::String typeString() const { return "string-set!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringEqualP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringEqualP()  : procedureName_("string=?") {}
    virtual ~StringEqualP() {}

    virtual ::util::String toString() {    return "#<subr ""string=?"">";}
    virtual ::util::String typeString() const { return "string=?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringToNumber : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringToNumber()  : procedureName_("string->number") {}
    virtual ~StringToNumber() {}

    virtual ::util::String toString() {    return "#<subr ""string->number"">";}
    virtual ::util::String typeString() const { return "string->number"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringSplit : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringSplit()  : procedureName_("string-split") {}
    virtual ~StringSplit() {}

    virtual ::util::String toString() {    return "#<subr ""string-split"">";}
    virtual ::util::String typeString() const { return "string-split"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StringToSymbol : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StringToSymbol()  : procedureName_("string->symbol") {}
    virtual ~StringToSymbol() {}

    virtual ::util::String toString() {    return "#<subr ""string->symbol"">";}
    virtual ::util::String typeString() const { return "string->symbol"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class RegexpP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RegexpP()  : procedureName_("regexp?") {}
    virtual ~RegexpP() {}

    virtual ::util::String toString() {    return "#<subr ""regexp?"">";}
    virtual ::util::String typeString() const { return "regexp?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class RxMatchNumMatches : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchNumMatches()  : procedureName_("rxmatch-num-matches") {}
    virtual ~RxMatchNumMatches() {}

    virtual ::util::String toString() {    return "#<subr ""rxmatch-num-matches"">";}
    virtual ::util::String typeString() const { return "rxmatch-num-matches"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class RegexpToString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RegexpToString()  : procedureName_("regexp->string") {}
    virtual ~RegexpToString() {}

    virtual ::util::String toString() {    return "#<subr ""regexp->string"">";}
    virtual ::util::String typeString() const { return "regexp->string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class RxMatch : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatch()  : procedureName_("rxmatch") {}
    virtual ~RxMatch() {}

    virtual ::util::String toString() {    return "#<subr ""rxmatch"">";}
    virtual ::util::String typeString() const { return "rxmatch"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class RxMatchStart : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchStart()  : procedureName_("rxmatch-start") {}
    virtual ~RxMatchStart() {}

    virtual ::util::String toString() {    return "#<subr ""rxmatch-start"">";}
    virtual ::util::String typeString() const { return "rxmatch-start"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class RxMatchEnd : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchEnd()  : procedureName_("rxmatch-end") {}
    virtual ~RxMatchEnd() {}

    virtual ::util::String toString() {    return "#<subr ""rxmatch-end"">";}
    virtual ::util::String typeString() const { return "rxmatch-end"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class RxMatchSubString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchSubString()  : procedureName_("rxmatch-substring") {}
    virtual ~RxMatchSubString() {}

    virtual ::util::String toString() {    return "#<subr ""rxmatch-substring"">";}
    virtual ::util::String typeString() const { return "rxmatch-substring"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class RxMatchAfter : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchAfter()  : procedureName_("rxmatch-after") {}
    virtual ~RxMatchAfter() {}

    virtual ::util::String toString() {    return "#<subr ""rxmatch-after"">";}
    virtual ::util::String typeString() const { return "rxmatch-after"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class RxMatchBefore : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    RxMatchBefore()  : procedureName_("rxmatch-before") {}
    virtual ~RxMatchBefore() {}

    virtual ::util::String toString() {    return "#<subr ""rxmatch-before"">";}
    virtual ::util::String typeString() const { return "rxmatch-before"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class SymbolP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SymbolP()  : procedureName_("symbol?") {}
    virtual ~SymbolP() {}

    virtual ::util::String toString() {    return "#<subr ""symbol?"">";}
    virtual ::util::String typeString() const { return "symbol?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class SymbolToString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SymbolToString()  : procedureName_("symbol->string") {}
    virtual ~SymbolToString() {}

    virtual ::util::String toString() {    return "#<subr ""symbol->string"">";}
    virtual ::util::String typeString() const { return "symbol->string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class VectorP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    VectorP()  : procedureName_("vector?") {}
    virtual ~VectorP() {}

    virtual ::util::String toString() {    return "#<subr ""vector?"">";}
    virtual ::util::String typeString() const { return "vector?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MakeVector : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MakeVector()  : procedureName_("make-vector") {}
    virtual ~MakeVector() {}

    virtual ::util::String toString() {    return "#<subr ""make-vector"">";}
    virtual ::util::String typeString() const { return "make-vector"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CreateVector : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CreateVector()  : procedureName_("vector") {}
    virtual ~CreateVector() {}

    virtual ::util::String toString() {    return "#<subr ""vector"">";}
    virtual ::util::String typeString() const { return "vector"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class VectorLength : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    VectorLength()  : procedureName_("vector-length") {}
    virtual ~VectorLength() {}

    virtual ::util::String toString() {    return "#<subr ""vector-length"">";}
    virtual ::util::String typeString() const { return "vector-length"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class VectorRef : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    VectorRef()  : procedureName_("vector-ref") {}
    virtual ~VectorRef() {}

    virtual ::util::String toString() {    return "#<subr ""vector-ref"">";}
    virtual ::util::String typeString() const { return "vector-ref"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class VectorSet : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    VectorSet()  : procedureName_("vector-set!") {}
    virtual ~VectorSet() {}

    virtual ::util::String toString() {    return "#<subr ""vector-set!"">";}
    virtual ::util::String typeString() const { return "vector-set!"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class DefineMacro : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    DefineMacro()  : procedureName_("define-macro") {}
    virtual ~DefineMacro() {}

    virtual ::util::String toString() {    return "#<subr ""define-macro"">";}
    virtual ::util::String typeString() const { return "define-macro"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class DynamicWindProc : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    DynamicWindProc()  : procedureName_("dynamic-wind") {}
    virtual ~DynamicWindProc() {}

    virtual ::util::String toString() {    return "#<subr ""dynamic-wind"">";}
    virtual ::util::String typeString() const { return "dynamic-wind"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CallWithValues : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallWithValues()  : procedureName_("call-with-values") {}
    virtual ~CallWithValues() {}

    virtual ::util::String toString() {    return "#<subr ""call-with-values"">";}
    virtual ::util::String typeString() const { return "call-with-values"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CallWithCurrentContinuation : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallWithCurrentContinuation()  : procedureName_("call-with-current-continuation") {}
    virtual ~CallWithCurrentContinuation() {}

    virtual ::util::String toString() {    return "#<subr ""call-with-current-continuation"">";}
    virtual ::util::String typeString() const { return "call-with-current-continuation"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NotSupported : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NotSupported()  : procedureName_("not-supported") {}
    virtual ~NotSupported() {}

    virtual ::util::String toString() {    return "#<subr ""not-supported"">";}
    virtual ::util::String typeString() const { return "not-supported"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class ProcedureP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    ProcedureP()  : procedureName_("procedure?") {}
    virtual ~ProcedureP() {}

    virtual ::util::String toString() {    return "#<subr ""procedure?"">";}
    virtual ::util::String typeString() const { return "procedure?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class BooleanP : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    BooleanP()  : procedureName_("boolean?") {}
    virtual ~BooleanP() {}

    virtual ::util::String toString() {    return "#<subr ""boolean?"">";}
    virtual ::util::String typeString() const { return "boolean?"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Exit : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Exit()  : procedureName_("exit") {}
    virtual ~Exit() {}

    virtual ::util::String toString() {    return "#<subr ""exit"">";}
    virtual ::util::String typeString() const { return "exit"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Apply : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Apply()  : procedureName_("apply") {}
    virtual ~Apply() {}

    virtual ::util::String toString() {    return "#<subr ""apply"">";}
    virtual ::util::String typeString() const { return "apply"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class Eval : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    Eval()  : procedureName_("eval") {}
    virtual ~Eval() {}

    virtual ::util::String toString() {    return "#<subr ""eval"">";}
    virtual ::util::String typeString() const { return "eval"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class NullEnvironment : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    NullEnvironment()  : procedureName_("null-environment") {}
    virtual ~NullEnvironment() {}

    virtual ::util::String toString() {    return "#<subr ""null-environment"">";}
    virtual ::util::String typeString() const { return "null-environment"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class SchemeReportEnvironment : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    SchemeReportEnvironment()  : procedureName_("scheme-report-environment") {}
    virtual ~SchemeReportEnvironment() {}

    virtual ::util::String toString() {    return "#<subr ""scheme-report-environment"">";}
    virtual ::util::String typeString() const { return "scheme-report-environment"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class InteractionEnvironment : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    InteractionEnvironment()  : procedureName_("interaction-environment") {}
    virtual ~InteractionEnvironment() {}

    virtual ::util::String toString() {    return "#<subr ""interaction-environment"">";}
    virtual ::util::String typeString() const { return "interaction-environment"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CallPipe : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallPipe()  : procedureName_("|") {}
    virtual ~CallPipe() {}

    virtual ::util::String toString() {    return "#<subr ""|"">";}
    virtual ::util::String typeString() const { return "|"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CallProcess : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallProcess()  : procedureName_("call-process") {}
    virtual ~CallProcess() {}

    virtual ::util::String toString() {    return "#<subr ""call-process"">";}
    virtual ::util::String typeString() const { return "call-process"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class StartProcess : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    StartProcess()  : procedureName_("start-process") {}
    virtual ~StartProcess() {}

    virtual ::util::String toString() {    return "#<subr ""start-process"">";}
    virtual ::util::String typeString() const { return "start-process"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class CallProcessOutString : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    CallProcessOutString()  : procedureName_("call-process-out-string") {}
    virtual ~CallProcessOutString() {}

    virtual ::util::String toString() {    return "#<subr ""call-process-out-string"">";}
    virtual ::util::String typeString() const { return "call-process-out-string"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaGuiMoveWindow : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaGuiMoveWindow()  : procedureName_("mona-gui-move-window") {}
    virtual ~MonaGuiMoveWindow() {}

    virtual ::util::String toString() {    return "#<subr ""mona-gui-move-window"">";}
    virtual ::util::String typeString() const { return "mona-gui-move-window"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaGuiGetWindowTitle : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaGuiGetWindowTitle()  : procedureName_("mona-gui-get-window-title") {}
    virtual ~MonaGuiGetWindowTitle() {}

    virtual ::util::String toString() {    return "#<subr ""mona-gui-get-window-title"">";}
    virtual ::util::String typeString() const { return "mona-gui-get-window-title"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaGuiEnumWindows : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaGuiEnumWindows()  : procedureName_("mona-gui-enum-windows") {}
    virtual ~MonaGuiEnumWindows() {}

    virtual ::util::String toString() {    return "#<subr ""mona-gui-enum-windows"">";}
    virtual ::util::String typeString() const { return "mona-gui-enum-windows"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaHalt : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaHalt()  : procedureName_("mona-halt") {}
    virtual ~MonaHalt() {}

    virtual ::util::String toString() {    return "#<subr ""mona-halt"">";}
    virtual ::util::String typeString() const { return "mona-halt"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaReboot : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaReboot()  : procedureName_("mona-reboot") {}
    virtual ~MonaReboot() {}

    virtual ::util::String toString() {    return "#<subr ""mona-reboot"">";}
    virtual ::util::String typeString() const { return "mona-reboot"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaKill : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaKill()  : procedureName_("mona-kill") {}
    virtual ~MonaKill() {}

    virtual ::util::String toString() {    return "#<subr ""mona-kill"">";}
    virtual ::util::String typeString() const { return "mona-kill"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaSleep : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaSleep()  : procedureName_("mona-sleep") {}
    virtual ~MonaSleep() {}

    virtual ::util::String toString() {    return "#<subr ""mona-sleep"">";}
    virtual ::util::String typeString() const { return "mona-sleep"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaPs : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaPs()  : procedureName_("mona-ps") {}
    virtual ~MonaPs() {}

    virtual ::util::String toString() {    return "#<subr ""mona-ps"">";}
    virtual ::util::String typeString() const { return "mona-ps"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

class MonaStackDump : public PrimitiveProcedure
{
private:
    ::util::String procedureName_;
public:
    MonaStackDump()  : procedureName_("mona-stack-dump") {}
    virtual ~MonaStackDump() {}

    virtual ::util::String toString() {    return "#<subr ""mona-stack-dump"">";}
    virtual ::util::String typeString() const { return "mona-stack-dump"; }
    virtual Object* eval(Environment* env)
    {
        return this;
    }
    virtual Object* apply(Objects* arguments, Environment* env, bool evalArguments = true);
};

};
#endif
