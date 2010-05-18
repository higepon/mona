#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include <stdlib.h> // algorithm require rand()
#include <algorithm>

using namespace MonAPI;

#define TEST_MAP "/APPS/TSTACK.APP/TEST.MAP"

class FileReader
{
  public:
    FileReader() : fp_(NULL), eof_(false){}
    inline bool end() { return eof_; }
    bool open(const std::string& path)
    {
        fp_ = fopen(path.c_str(), "r");
        return fp_ != NULL;
    }
    void close()
    {
        close_if_necessary();
    }
    std::string getLine()
    {
        std::string str("");
        int ch;
        while((ch = fgetc(fp_)) != EOF)
        {
            if(ch == '\n') 
                return str;
            if(ch != '\r') // just ignore \r
                str += (char)ch;
        }
        eof_ = true;
        return str;
    }
    
    ~FileReader()
    {
        close_if_necessary();
    }
  private:
    void close_if_necessary()
    {
        if(fp_ != NULL)
        {
            fclose(fp_);
            fp_ = NULL;
        }
    }
    FILE *fp_;
    bool eof_;
};

enum MapFileLineType
{
    MAP_FILE_LINE_TYPE_BEGIN_TEXT_SECTION,
    MAP_FILE_LINE_TYPE_FILE,
    MAP_FILE_LINE_TYPE_FUNCTION,
    MAP_FILE_LINE_TYPE_BEGIN_OTHER_SECTION,
    MAP_FILE_LINE_TYPE_OTHER
};

template <class T> class MapFileScanner
{
  public:
    MapFileScanner(T& reader) :  end_(false), reader_(reader) {}

    bool end() { return end_; }
    bool nextLine() {
        end_ = reader_.end();
        if(!reader_.end())
            currentLine_ = reader_.getLine();
        return !end_;
    }

    bool beginWith(const char* pat) {
        return 0 == currentLine_.find(pat);
    }

    bool notSpaceAt(unsigned int pos) {
        if(currentLine_.size() <= pos)
          return false;
        return currentLine_[pos] != ' ';
    }

    std::string fileName() {
        assert(lineType() == MAP_FILE_LINE_TYPE_FILE);
        return currentLine_.substr(38);
    }

    std::string address() {
        assert(lineType() == MAP_FILE_LINE_TYPE_FILE || lineType() == MAP_FILE_LINE_TYPE_FUNCTION);
        return currentLine_.substr(16, 10);
    }

    std::string functionName() {
        assert(lineType() == MAP_FILE_LINE_TYPE_FUNCTION);
        return currentLine_.substr(42);
    }

    MapFileLineType lineType() {
        if(beginWith(".text"))
          return MAP_FILE_LINE_TYPE_BEGIN_TEXT_SECTION;
        if(beginWith("."))
          return MAP_FILE_LINE_TYPE_BEGIN_OTHER_SECTION;
        if(beginWith(" .text "))
          return MAP_FILE_LINE_TYPE_FILE;
        if(notSpaceAt(38) && beginWith("                0x")) // tenuki-
          return MAP_FILE_LINE_TYPE_FILE;
        if(beginWith("                0x")) // tenuki-
          return MAP_FILE_LINE_TYPE_FUNCTION;
        return MAP_FILE_LINE_TYPE_OTHER;
    }



    // make public for test.
    std::string currentLine_;


  private:
    bool end_;
    T& reader_;
};

class StringReaderForTest
{
  public:
    inline bool end() { return lines_.size() == 0; }
    void close(){}
    std::string getLine() {
        std::string str = lines_.front();
        lines_.erase(lines_.begin());
        return str;
    }
    void putLine(const std::string& str) {
        lines_.push_back(str);
    }
    std::vector<std::string> lines_;
};

void testFileReader_Open()
{
    FileReader reader;
    EXPECT_TRUE(reader.open(TEST_MAP));
}

void testFileReader_getLine_once()
{
    FileReader reader;
    EXPECT_TRUE(reader.open(TEST_MAP));
    EXPECT_STR_EQ("Archive member included because of file (symbol)", reader.getLine().c_str());
}

void testFileReader_getLine_twice()
{
    FileReader reader;
    reader.open(TEST_MAP);
    reader.getLine();
    std::string actual = reader.getLine();
    EXPECT_STR_EQ("", actual.c_str());
}

// tenuki-
void testStringReaderForTest()
{
    StringReaderForTest reader;

    reader.putLine("hoge");
    reader.putLine("ika");

    std::string actual = reader.getLine();
    EXPECT_STR_EQ("hoge", actual.c_str());
    EXPECT_TRUE(!reader.end());

    actual = reader.getLine();
    EXPECT_STR_EQ("ika", actual.c_str());
    EXPECT_TRUE(reader.end());
}

static void expectLineTypeWithInputWithLine(const char* input, MapFileLineType lType, int linenum)
{
    StringReaderForTest reader;
    MapFileScanner<StringReaderForTest> scanner(reader);

    char buf[10];
    sprintf(buf, "line: %d", linenum);
    scanner.currentLine_ = input;
    EXPECT_EQ_MSG(lType, scanner.lineType(), buf);
}

#define EXPECT_LINETYPE(i, l) expectLineTypeWithInputWithLine((i), (l), __LINE__)

void testMapFileScanner_lineType()
{
    EXPECT_LINETYPE(" .text          0xa005be20      0x190 .\\libindep.a(obj_map.o)", MAP_FILE_LINE_TYPE_FILE);
    EXPECT_LINETYPE(" .text          0xa003e700        0x0 libwc/ces.o", MAP_FILE_LINE_TYPE_FILE);
    EXPECT_LINETYPE(" *(.init)", MAP_FILE_LINE_TYPE_OTHER);
    EXPECT_LINETYPE(" *(.text)", MAP_FILE_LINE_TYPE_OTHER);
    EXPECT_LINETYPE(" *(.glue_7t)", MAP_FILE_LINE_TYPE_OTHER);
    EXPECT_LINETYPE(" *(.ctors)", MAP_FILE_LINE_TYPE_OTHER);
    EXPECT_LINETYPE(".text           0xa0001000    0x5d210", MAP_FILE_LINE_TYPE_BEGIN_TEXT_SECTION);
    EXPECT_LINETYPE("                0xa0001009                __fu0__monapi_memory_initialized", MAP_FILE_LINE_TYPE_FUNCTION);
    EXPECT_LINETYPE(" .text$_ZN7W3MPane5paintEPN6baygui8GraphicsE", MAP_FILE_LINE_TYPE_OTHER); // tenuki-
    EXPECT_LINETYPE("                0xa005db60      0x550 main_mona.o", MAP_FILE_LINE_TYPE_FILE);
    EXPECT_LINETYPE("                0xa005db60      0x550 a", MAP_FILE_LINE_TYPE_FILE);
    EXPECT_LINETYPE("", MAP_FILE_LINE_TYPE_OTHER);
    EXPECT_LINETYPE(".data           0xa005f000    0xa7000", MAP_FILE_LINE_TYPE_BEGIN_OTHER_SECTION);


}

void testMapFileScanner_next()
{
    const char* expect = "dummy";

    StringReaderForTest reader;
    MapFileScanner<StringReaderForTest> scanner(reader);
    reader.putLine(expect);

    scanner.nextLine();

    EXPECT_STR_EQ(expect, scanner.currentLine_.c_str()); 
}


void testMapFileScanner_end_EmptyReader()
{
    StringReaderForTest reader;
    MapFileScanner<StringReaderForTest> scanner(reader);

    EXPECT_TRUE(!scanner.end());

    scanner.nextLine();
    EXPECT_TRUE(scanner.end());
}

void testMapFileScanner_end_OneLineReader()
{
    StringReaderForTest reader;
    MapFileScanner<StringReaderForTest> scanner(reader);

    reader.putLine("dummy");
    EXPECT_TRUE(!scanner.end());

    scanner.nextLine();
    EXPECT_TRUE(!scanner.end());

    scanner.nextLine();
    EXPECT_TRUE(scanner.end());
}

static void EXPECT_STR_EQ_LINE(const char *expect, const char *actual, int linenum)
{
    char buf[10];
    sprintf(buf, "line: %d", linenum);
    EXPECT_STR_EQ_MSG(expect, actual, buf);
}

static void expectFileNameWithLine(const char *input, const char *expect, int linenum)
{
    StringReaderForTest reader;
    MapFileScanner<StringReaderForTest> scanner(reader);
    scanner.currentLine_ = input;

    std::string str = scanner.fileName();
    const char *actual = str.c_str();

    EXPECT_STR_EQ_LINE(expect, actual, linenum);
}

#define EXPECT_FILENAME(input, expect) expectFileNameWithLine((input), (expect), __LINE__)

void testMapFileScanner_fileName()
{
    EXPECT_FILENAME(" .text          0xa005be20      0x190 .\\libindep.a(obj_map.o)", ".\\libindep.a(obj_map.o)");
    EXPECT_FILENAME(" .text          0xa003e700        0x0 libwc/ces.o", "libwc/ces.o");
    EXPECT_FILENAME("                0xa005db60      0x550 main_mona.o", "main_mona.o");
}

static void expectAddressWithLine(const char *input, const char *expect, int linenum)
{
    StringReaderForTest reader;
    MapFileScanner<StringReaderForTest> scanner(reader);
    scanner.currentLine_ = input;

    std::string str = scanner.address();
    const char *actual = str.c_str();

    EXPECT_STR_EQ_LINE(expect, actual, linenum);
}

#define EXPECT_ADDRESS(input, expect) expectAddressWithLine((input), (expect), __LINE__)


void testMapFileScanner_address()
{
    EXPECT_ADDRESS(" .text          0xa005be20      0x190 .\\libindep.a(obj_map.o)", "0xa005be20");
    EXPECT_ADDRESS(" .text          0xa003e700        0x0 libwc/ces.o", "0xa003e700");
    EXPECT_ADDRESS("                0xa005db60      0x550 main_mona.o", "0xa005db60");
}


static void expectFunctionNameWithLine(const char *input, const char *expect, int linenum)
{
    StringReaderForTest reader;
    MapFileScanner<StringReaderForTest> scanner(reader);
    scanner.currentLine_ = input;

    std::string str = scanner.functionName();
    const char *actual = str.c_str();

    EXPECT_STR_EQ_LINE(expect, actual, linenum);
}

#define EXPECT_FUNCTION_NAME(input, expect) expectFunctionNameWithLine((input), (expect), __LINE__)


void testMapFileScanner_functionName()
{
    EXPECT_FUNCTION_NAME("                0xa0001009                __fu0__monapi_memory_initialized", "__fu0__monapi_memory_initialized");
    EXPECT_FUNCTION_NAME("                0xa005dab0                baygui::Frame::getTitlebarClicked(int, int)", "baygui::Frame::getTitlebarClicked(int, int)");
}



struct SymbolInfoEntry
{
    SymbolInfoEntry(const std::string& fileName,
                    const std::string& funcName,
                    const std::string& address) : FileName(fileName), FunctionName(funcName), Address(address){}
    std::string FileName;
    std::string FunctionName;
    std::string Address;
};

typedef std::vector<SymbolInfoEntry> SymbolInfoEntries;

class SymbolInfoEntrySerializer
{
public:
    int writeString(uint8_t* data, const std::string& str)
    {
        uint16_t size = (uint16_t)str.size();

        uint16_t *ptr16 = (uint16_t*)data;
        *ptr16 = size;
        data+= 2;
        for(unsigned int i = 0; i < str.size(); i++)
        {
            data[i] = str[i];
        }
        return 2+str.size();
    }

    uint32_t addressToInt(const std::string& address)
    {
        uint32_t res = 0;

        assert(address.size() == 10);

        // begin with 0x
        for(int i = 2; i < 10; i++)
        {
            if('a' <= address[i] && 'f' >= address[i])
            {
                res |= (address[i] - 'a' + 10) << (9 - i)*4;
            }
            else
            {
                assert('0' <= address[i] && '9' >= address[i]);
                res |= (address[i] - '0') << (9 - i)*4;
            }
        }
        return res;
    }

    void serialize(const SymbolInfoEntry &ent, uint8_t* data, int size)
    {
        assert(calcSize(ent) <= size);

        uint32_t address = addressToInt(ent.Address);
        uint32_t* ptr32 = (uint32_t*)data;
        *ptr32 = address;

        data += 4;

        data += writeString(data, ent.FileName);
        data += writeString(data, ent.FunctionName);
    }

    /*
      caracter is ascii.
       10byte: address
       2byte: length
       length byte: funcName
       2byte: length
       length byte: fileName
      */
    int calcSize(const SymbolInfoEntry &ent)
    {
        assert(ent.Address.size() == 10);
        return 4+2+ent.FunctionName.size()+2+ent.FileName.size();
    }
};


class SymbolInfoCollection
{
public:
    void addFileEntry(const std::string& fileName){
        currentFileName_ = fileName;
    }
    void addFunctionEntry(const std::string& address, const std::string& functionName) {
        if(currentFileName_.size() == 0) {
            printf("function entry before file entry. unknown map file format\n");
            return;
        }

        SymbolInfoEntry ent(currentFileName_, functionName, address);
        Symbols.push_back(ent);
    }
    SymbolInfoEntries Symbols;

    class SymbolSorterByAddress {
    public:
        SymbolSorterByAddress() {}
        //operator  < 
        bool operator ()(const SymbolInfoEntry *left, const SymbolInfoEntry *right )
        {	return operator()(*left, *right);	}
        bool operator ()(const SymbolInfoEntry & left, const SymbolInfoEntry & right )
        {
            return left.Address < right.Address;
        }
    };


    // by address
    void sort()
    {
        std::sort(Symbols.begin(), Symbols.end(), SymbolSorterByAddress());
    }

    class SizeCounter
    {
    public:
        SizeCounter(SymbolInfoEntrySerializer &s, int& sum) : Sum(sum), serializer_(s) {}

        void operator()( const SymbolInfoEntry *ent) {
            return operator()(*ent);
        }
        void operator()( const SymbolInfoEntry &ent) {
            Sum += serializer_.calcSize(ent);
        }
        int& Sum;
        SymbolInfoEntrySerializer& serializer_;
    };

    class Serializer
    {
    public:
        Serializer(SymbolInfoEntrySerializer& s, uint8_t *buf, int size) : serializer_(s), size_(size), pos_(0), buf_(buf) {}

        void operator()( const SymbolInfoEntry *ent) {
            return operator()(*ent);
        }
        void operator()( const SymbolInfoEntry &ent) {
            serializer_.serialize(ent, buf_+pos_, size_-pos_);
            pos_ += serializer_.calcSize(ent);
        }
        SymbolInfoEntrySerializer& serializer_;
        int size_;
        int pos_;
        uint8_t *buf_;
    };

    monapi_cmemoryinfo* serialize()
    {
        SymbolInfoEntrySerializer serializer;
        int size = 0;
        SizeCounter counter(serializer, size);

        std::for_each(Symbols.begin(), Symbols.end(), counter);

        monapi_cmemoryinfo *buf = new monapi_cmemoryinfo();
        if(!buf)
            return NULL;
        if(M_OK != monapi_cmemoryinfo_create(buf, size, 0))
        {
            monapi_cmemoryinfo_delete(buf);
            return NULL;
        }

        Serializer s(serializer, buf->Data, size);
        std::for_each(Symbols.begin(), Symbols.end(), s);
        return buf;
    }

    void DebugDump()
    {
        for(SymbolInfoEntries::iterator entIt = Symbols.begin(); entIt != Symbols.end(); entIt++)
        {
            _printf("file=%s,  func=%s, address=%s\n", entIt->FileName.c_str(), entIt->FunctionName.c_str(), entIt->Address.c_str());
        }
    }
private:
    std::string currentFileName_;
};

static void expectEntryLine(const char* fileName, const char* address, const char *funcName, SymbolInfoEntry& ent, int line)
{
    EXPECT_STR_EQ_LINE(fileName, ent.FileName.c_str(), line);
    EXPECT_STR_EQ_LINE(funcName, ent.FunctionName.c_str(), line);
    EXPECT_STR_EQ_LINE(address, ent.Address.c_str(), line);
}

#define EXPECT_ENTRY(f, a, fn, e) expectEntryLine((f), (a), (fn), (e), __LINE__)


void testSymbolInfoCollection() {
    SymbolInfoCollection col;
    col.addFileEntry("test.c");
    col.addFunctionEntry("0xaaaaa", "myFunc");
    col.addFunctionEntry("0xbbbbb", "myFunc2");

    col.addFileEntry("test2.c");
    col.addFunctionEntry("0xccccc", "hogeFunc");

    SymbolInfoEntries& ents = col.Symbols;
    EXPECT_EQ(3, ents.size());

    EXPECT_ENTRY("test.c", "0xaaaaa", "myFunc", ents[0]);
    EXPECT_ENTRY("test.c", "0xbbbbb", "myFunc2", ents[1]);
    EXPECT_ENTRY("test2.c", "0xccccc", "hogeFunc", ents[2]);
}

void testSymbolInfoCollection_sort() {
    SymbolInfoCollection col;
    col.addFileEntry("test.c");
    col.addFunctionEntry("0xa0001", "lastFunc");
    col.addFunctionEntry("0x10001", "secondFunc");

    col.addFileEntry("test2.c");
    col.addFunctionEntry("0x12345", "thirdFunc");
    col.addFunctionEntry("0x10000", "firstFunc");

    col.sort();

    SymbolInfoEntries& ents = col.Symbols;
    EXPECT_EQ(4, ents.size());

    EXPECT_STR_EQ("firstFunc", ents[0].FunctionName.c_str());
    EXPECT_STR_EQ("secondFunc", ents[1].FunctionName.c_str());
    EXPECT_STR_EQ("thirdFunc", ents[2].FunctionName.c_str());
    EXPECT_STR_EQ("lastFunc", ents[3].FunctionName.c_str());
}

void testSymbolInfoCollection_serialize() {
    SymbolInfoCollection col;
    col.addFileEntry("test.c");
    col.addFunctionEntry("0xa0001000", "lastFunc");
    col.addFunctionEntry("0x10001000", "secondFunc");

    col.addFileEntry("test2.c");
    col.addFunctionEntry("0x12345000", "thirdFunc");
    col.addFunctionEntry("0x10000000", "firstFunc");

    col.sort();
    monapi_cmemoryinfo* cm =  col.serialize();

    EXPECT_TRUE(NULL !=cm);

    monapi_cmemoryinfo_dispose(cm);
    monapi_cmemoryinfo_delete(cm);
}


void EXPECT_CALCSIZE(const char* msg, int expect, const char* fileName, const char* funcName, const char* address)
{
    SymbolInfoEntrySerializer serializer;
    SymbolInfoEntry ent(fileName, funcName, address);

    int actual = serializer.calcSize(ent);
    EXPECT_EQ_MSG(expect, actual, msg); 
}

void testSymbolInfoEntrySerializer_calcSize() {
    EXPECT_CALCSIZE("empty case", 2+2+4, "", "",  "0xa0001000");
    EXPECT_CALCSIZE("fname=aa, func=bbb", 2+2+4+2+3, "aa", "bbb",  "0xa0001000");
}

void testSymbolInfoEntrySerializer_serialize_empty() {
    uint8_t buf[9];
    buf[8] = 0xff;

    SymbolInfoEntrySerializer serializer;
    SymbolInfoEntry ent("", "", "0xa1234567");

    serializer.serialize(ent, buf, 8);

    EXPECT_EQ(0x67, buf[0]);
    EXPECT_EQ(0x45, buf[1]);
    EXPECT_EQ(0x23, buf[2]);
    EXPECT_EQ(0xa1, buf[3]);
    EXPECT_EQ(0, buf[4]);
    EXPECT_EQ(0, buf[5]);
    EXPECT_EQ(0, buf[6]);
    EXPECT_EQ(0, buf[7]);
    EXPECT_EQ(0xff, buf[8]);
}

void testSymbolInfoEntrySerializer_serialize_ab() {
    uint8_t buf[11];
    buf[10] = 0xff;

    SymbolInfoEntrySerializer serializer;
    SymbolInfoEntry ent("a", "b", "0xa1234567");

    serializer.serialize(ent, buf, 10);

    EXPECT_EQ(0x67, buf[0]);
    EXPECT_EQ(0x45, buf[1]);
    EXPECT_EQ(0x23, buf[2]);
    EXPECT_EQ(0xa1, buf[3]);
    EXPECT_EQ(1, buf[4]);
    EXPECT_EQ(0, buf[5]);
    EXPECT_EQ('a', buf[6]);
    EXPECT_EQ(1, buf[7]);
    EXPECT_EQ(0, buf[8]);
    EXPECT_EQ('b', buf[9]);
    EXPECT_EQ(0xff, buf[10]);
}

void testSymbolInfoEntrySerializer_addressToInt() {
    SymbolInfoEntrySerializer serializer;
    uint32_t actual = serializer.addressToInt("0xabcd1234");
    EXPECT_EQ(((signed)0xabcd1234), actual);
}

enum MapFileParseState
{
    MAP_FILE_STATE_INIT,
    MAP_FILE_STATE_TEXT_SECTION,
    MAP_FILE_STATE_FILE_SECTION,
    MAP_FILE_STATE_AFTER_TEXT_SECTION
};

template <class T> class MapFileParser
{
  public:
    SymbolInfoCollection symbolInfos_;

    MapFileParser(T &scanner) : scanner_(scanner), state_(MAP_FILE_STATE_INIT) {} 

    inline MapFileParseState state() { return state_; }

    void skipUntilBeginTextSection() {
        assert(state_ == MAP_FILE_STATE_INIT);
        scanner_.nextLine();
        while(!scanner_.end() && scanner_.lineType() != MAP_FILE_LINE_TYPE_BEGIN_TEXT_SECTION)
            scanner_.nextLine();

        if(scanner_.end())
            state_ = MAP_FILE_STATE_AFTER_TEXT_SECTION;
    }

    void skipUnknownSection() {
        while(!scanner_.end() && scanner_.lineType() == MAP_FILE_LINE_TYPE_OTHER)
            scanner_.nextLine();
    }

    // public for test purpose.
    inline void gotoState(MapFileParseState next) { state_ = next; }

    void evalCurrentLine() {
        switch(state_) {
        case MAP_FILE_STATE_INIT:
            if(scanner_.lineType() == MAP_FILE_LINE_TYPE_BEGIN_TEXT_SECTION)
                gotoState(MAP_FILE_STATE_TEXT_SECTION);
            break;
        case MAP_FILE_STATE_AFTER_TEXT_SECTION:
            break;
        case MAP_FILE_STATE_TEXT_SECTION:
            if(scanner_.lineType() == MAP_FILE_LINE_TYPE_FILE)
            {
                symbolInfos_.addFileEntry(scanner_.fileName());
                gotoState(MAP_FILE_STATE_FILE_SECTION);
            }
            else if(scanner_.lineType() == MAP_FILE_LINE_TYPE_BEGIN_OTHER_SECTION)
            {
                gotoState(MAP_FILE_STATE_AFTER_TEXT_SECTION);
            }
            break;
        case MAP_FILE_STATE_FILE_SECTION:
            if(scanner_.lineType() == MAP_FILE_LINE_TYPE_FILE)
            {
                symbolInfos_.addFileEntry(scanner_.fileName());
            }
            else if(scanner_.lineType() == MAP_FILE_LINE_TYPE_FUNCTION)
            {
                symbolInfos_.addFunctionEntry(scanner_.address(), scanner_.functionName());
            }
            else if(scanner_.lineType() == MAP_FILE_LINE_TYPE_BEGIN_OTHER_SECTION)
            {
                gotoState(MAP_FILE_STATE_AFTER_TEXT_SECTION);
            }
            else
            {
                assert(false);
            }
            break;
        }
    }

    void parseAll() {
        while(next())
            evalCurrentLine();
    }

    bool next() {
        switch(state_) {
        case MAP_FILE_STATE_INIT: 
            skipUntilBeginTextSection();
            break;
        case MAP_FILE_STATE_TEXT_SECTION:
            scanner_.nextLine();
            skipUnknownSection();
            break;
        case MAP_FILE_STATE_FILE_SECTION:
            scanner_.nextLine();
            skipUnknownSection();
            break;
        case MAP_FILE_STATE_AFTER_TEXT_SECTION:
            return false;
        default:
            assert(false);
            return false;
        }
        if(scanner_.end()) {
            gotoState(MAP_FILE_STATE_AFTER_TEXT_SECTION);
            return false;
        }
        return true;
    }

    inline bool end() { return MAP_FILE_STATE_AFTER_TEXT_SECTION == state(); }

  private:
    T& scanner_;
    MapFileParseState state_;
};

class ScannerForTest
{
  public:
    ScannerForTest() : end_(false) {}

    bool end_;
    bool end() { return end_; }

    MapFileLineType lineType_;
    MapFileLineType lineType() { return lineType_; }

    std::string fileName_;
    std::string fileName() { return fileName_; }

    std::string address_;
    std::string address() { return address_; }

    std::string functionName_;
    std::string functionName() { return functionName_; }


    bool nextCalled_;
    bool nextLine() { nextCalled_ = true; return true; }
};

static void testMapFileParser_init()
{
    ScannerForTest scanner;
    MapFileParser<ScannerForTest> parser(scanner);
    EXPECT_EQ(MAP_FILE_STATE_INIT, parser.state());
}

static void testMapFileParser_next_emptyFile_initToEnd()
{
    ScannerForTest scanner;
    MapFileParser<ScannerForTest> parser(scanner);

    scanner.end_ = true;
    EXPECT_TRUE(scanner.end());
    EXPECT_TRUE(scanner.end());
    parser.gotoState(MAP_FILE_STATE_INIT);

    bool actual = parser.next();

    EXPECT_EQ(false, actual);
    EXPECT_EQ(MAP_FILE_STATE_AFTER_TEXT_SECTION, parser.state());
}

static void testMapFileParser_next_initToTextSection()
{
    ScannerForTest scanner;
    MapFileParser<ScannerForTest> parser(scanner);

    scanner.end_ = false;
    scanner.nextCalled_ = false;
    scanner.lineType_ = MAP_FILE_LINE_TYPE_BEGIN_TEXT_SECTION;
    parser.gotoState(MAP_FILE_STATE_INIT);
    bool actual = parser.next();

    EXPECT_TRUE(actual);
    EXPECT_TRUE(scanner.nextCalled_);
    EXPECT_EQ(MAP_FILE_STATE_INIT, parser.state());

    parser.evalCurrentLine();
    EXPECT_EQ(MAP_FILE_STATE_TEXT_SECTION, parser.state());
}

static void setupRealContents(StringReaderForTest &reader)
{
    reader.putLine("Archive member included because of file (symbol)");
    reader.putLine("");
    reader.putLine(".\\libindep.a(myctype.o)       main_mona.o (_MYCTYPE_MAP)");
    reader.putLine("c:/Users/mumurik/program/monaos/mona/mona/lib\\libmonalibc-imp.a(d000274.o)");
    reader.putLine("                              main_mona.o (_time)");
    reader.putLine(".text           0xa0001000    0x5d210");
    reader.putLine(" *(.init)");
    reader.putLine(" *(.text)");
    reader.putLine(" .text          0xa0001000       0x90 c:/Users/mumurik/program/monaos/mona/mona/lib/monapi_crt.o");
    reader.putLine("                0xa0001009                __fu0__monapi_memory_initialized");
    reader.putLine("                0xa0001000                _user_start");
    reader.putLine(" .text          0xa0001220        0x0 c:/Users/mumurik/program/monaos/mona/mona/lib\\libmonapi-imp.a(d000430.o)");
    reader.putLine(" .text          0xa0001220        0x8 c:/Users/mumurik/program/monaos/mona/mona/lib\\libmonapi-imp.a(d000555.o)");
    reader.putLine("                0xa0001220                _user_start_c_impl");

}

static void EXPECT_REAL_CONTENT_COL(SymbolInfoCollection& col)
{
    SymbolInfoEntries ents = col.Symbols;
    EXPECT_EQ(3, ents.size());


    char *fname = "c:/Users/mumurik/program/monaos/mona/mona/lib/monapi_crt.o";
    EXPECT_ENTRY(fname, "0xa0001009", "__fu0__monapi_memory_initialized", ents[0] );
    EXPECT_ENTRY(fname, "0xa0001000", "_user_start", ents[1] );

    fname = "c:/Users/mumurik/program/monaos/mona/mona/lib\\libmonapi-imp.a(d000555.o)";
    EXPECT_ENTRY(fname, "0xa0001220", "_user_start_c_impl", ents[2] );
}

static void testMapFileParser_parse_real()
{
    StringReaderForTest reader;
    MapFileScanner<StringReaderForTest> scanner(reader);
    MapFileParser<MapFileScanner<StringReaderForTest> > parser(scanner);

    setupRealContents(reader);

    bool res = parser.next();
    EXPECT_TRUE(res);
    EXPECT_STR_EQ(".text           0xa0001000    0x5d210", scanner.currentLine_.c_str());

    parser.evalCurrentLine();
    EXPECT_EQ(MAP_FILE_STATE_TEXT_SECTION, parser.state());

    res = parser.next();
    EXPECT_TRUE(res);
    EXPECT_STR_EQ(" .text          0xa0001000       0x90 c:/Users/mumurik/program/monaos/mona/mona/lib/monapi_crt.o", scanner.currentLine_.c_str());

    parser.evalCurrentLine();
    EXPECT_EQ(MAP_FILE_STATE_FILE_SECTION, parser.state());

    res = parser.next();
    EXPECT_TRUE(res);
    EXPECT_STR_EQ("                0xa0001009                __fu0__monapi_memory_initialized", scanner.currentLine_.c_str());

    parser.evalCurrentLine();
    EXPECT_EQ(MAP_FILE_STATE_FILE_SECTION, parser.state());

    res = parser.next();
    EXPECT_TRUE(res);
    EXPECT_STR_EQ("                0xa0001000                _user_start", scanner.currentLine_.c_str());

    parser.evalCurrentLine();
    EXPECT_EQ(MAP_FILE_STATE_FILE_SECTION, parser.state());

    res = parser.next();
    EXPECT_TRUE(res);
    EXPECT_STR_EQ(" .text          0xa0001220        0x0 c:/Users/mumurik/program/monaos/mona/mona/lib\\libmonapi-imp.a(d000430.o)", scanner.currentLine_.c_str());

    parser.evalCurrentLine();

    res = parser.next();
    EXPECT_TRUE(res);
    EXPECT_STR_EQ(" .text          0xa0001220        0x8 c:/Users/mumurik/program/monaos/mona/mona/lib\\libmonapi-imp.a(d000555.o)", scanner.currentLine_.c_str());

    parser.evalCurrentLine();

    res = parser.next();
    EXPECT_TRUE(res);
    EXPECT_STR_EQ("                0xa0001220                _user_start_c_impl", scanner.currentLine_.c_str());

    parser.evalCurrentLine();

    res = parser.next();
    EXPECT_TRUE(!res);
    EXPECT_TRUE(parser.end());

    EXPECT_REAL_CONTENT_COL(parser.symbolInfos_);

}

static void testMapFileParser_parseAll()
{
    StringReaderForTest reader;
    MapFileScanner<StringReaderForTest> scanner(reader);
    MapFileParser<MapFileScanner<StringReaderForTest> > parser(scanner);

    setupRealContents(reader);
    parser.parseAll();

    EXPECT_REAL_CONTENT_COL(parser.symbolInfos_);

    // parser.symbolInfos_.DebugDump();
}

static void testMapFileParser_parse_testMap()
{
    FileReader reader;
    reader.open(TEST_MAP);
    MapFileScanner<FileReader> scanner(reader);
    MapFileParser<MapFileScanner<FileReader> > parser(scanner);

    parser.parseAll();
    EXPECT_EQ(19, parser.symbolInfos_.Symbols.size());

    // parser.symbolInfos_.DebugDump();
}

// kernel side test.
#include <sys/BinaryTree.h>

void testBinaryTree_get_lower_nearest()
{
    BinaryTree<int> tree;
    tree.add(3, 3);
    tree.add(5, 5);
    tree.add(1, 1);
    tree.add(9, 9);
    tree.add(7, 7);

    EXPECT_EQ(0, tree.get_lower_nearest(-1));
    EXPECT_EQ(1, tree.get_lower_nearest(1));
    EXPECT_EQ(1, tree.get_lower_nearest(2));
    EXPECT_EQ(3, tree.get_lower_nearest(3));
    EXPECT_EQ(3, tree.get_lower_nearest(4));
    EXPECT_EQ(5, tree.get_lower_nearest(5));
    EXPECT_EQ(5, tree.get_lower_nearest(6));
    EXPECT_EQ(7, tree.get_lower_nearest(7));
    EXPECT_EQ(7, tree.get_lower_nearest(8));
    EXPECT_EQ(9, tree.get_lower_nearest(9));
    EXPECT_EQ(9, tree.get_lower_nearest(10));
}

class SymbolEntry {
public:
    SymbolEntry(char* fname, int fnlen, char *funcName, int funcLen, uint32_t address)
    {
        FileName = allocCString(fname, fnlen);
        FunctionName = allocCString(funcName, funcLen);
        Address = address;
    }
    SymbolEntry(const SymbolEntry& ent)
    {
        Address = ent.Address;
        FileName = allocCString(ent.FileName, strlen(ent.FileName));
        FunctionName = allocCString(ent.FunctionName, strlen(ent.FunctionName));
    }
    ~SymbolEntry()
    {
        delete FileName;
        delete FunctionName;
    }
    char* FileName;
    char* FunctionName;
    uint32_t Address;

private:
    char* allocCString(char* str, int strlen)
    {
        char* ret = new char[strlen+1];
        memcpy(ret, str, strlen);
        ret[strlen] = '\0';
        return ret;
    }
    
};

template <class T> class SymbolsDeserializer
{
public:
    T& builder_;
    SymbolsDeserializer(T& builder) : builder_(builder) {}

    void deserialize(uint8_t* data, int size)
    {
        int pos = 0;
        while(pos < size)
        {
            uint32_t* ptr32 = (uint32_t*)&data[pos];
            uint32_t address = *ptr32;
            pos+=4;

            uint16_t* ptr16 = (uint16_t*)&data[pos];
            int fnlen = (int)*ptr16;
            pos+=2;
            char* fname = (char*)&data[pos];
            pos+=fnlen;

            ptr16 = (uint16_t*)&data[pos];
            int funcLen = (int)*ptr16;
            pos+=2;
            char* funcName = (char*)&data[pos];
            pos+=funcLen;

            builder_.add(fname, fnlen, funcName, funcLen, address);
        }
    }

};

class SymbolDictionary
{
public:
    ~SymbolDictionary()
    {
        for(int i = 0; i < list_.size(); i++)
        {
            SymbolEntry * ent = list_[i];
            delete ent;
        }
    }
    SymbolEntry* lookup(uint32_t address) const {
        return tree_.get_lower_nearest(address);
    }
    void add(char* fname, int fnlen, char*funcName, int funcLen, uint32_t address)
    {
        SymbolEntry* ent = new SymbolEntry(fname, fnlen, funcName, funcLen, address);
        list_.add(ent);
        tree_.add(address, ent);
    }
    void deserialize(uint8_t* data, int size)
    {
        SymbolsDeserializer<SymbolDictionary> des(*this);
        des.deserialize(data, size);
    }

    BinaryTree<SymbolEntry*> tree_;
    HList<SymbolEntry*> list_;
};

void testSymbolDictionary_Integrate()
{
    FileReader reader;
    reader.open(TEST_MAP);
    MapFileScanner<FileReader> scanner(reader);
    MapFileParser<MapFileScanner<FileReader> > parser(scanner);

    parser.parseAll();
    monapi_cmemoryinfo* cm =  parser.symbolInfos_.serialize();
    EXPECT_TRUE(NULL !=cm);

    SymbolDictionary dict;
    dict.deserialize(cm->Data, cm->Size);

    SymbolEntry* ent = dict.lookup(0xa00010a0);
    EXPECT_STR_EQ("main.o", ent->FileName);
    EXPECT_STR_EQ("third()", ent->FunctionName);

    monapi_cmemoryinfo_dispose(cm);
    monapi_cmemoryinfo_delete(cm);
}



int main(int argc, char *argv[])
{
    testFileReader_Open();
    testFileReader_getLine_once();
    testFileReader_getLine_twice();

    testStringReaderForTest();
    testSymbolInfoCollection();
    testSymbolInfoCollection_sort();
    testSymbolInfoCollection_serialize();

    testSymbolInfoEntrySerializer_calcSize();
    testSymbolInfoEntrySerializer_serialize_empty();
    testSymbolInfoEntrySerializer_serialize_ab();
    testSymbolInfoEntrySerializer_addressToInt();

    testMapFileScanner_lineType();
    testMapFileScanner_next();
    testMapFileScanner_end_EmptyReader();
    testMapFileScanner_end_OneLineReader();
    testMapFileScanner_fileName();
    testMapFileScanner_address();
    testMapFileScanner_functionName();

    testMapFileParser_init();
    testMapFileParser_next_emptyFile_initToEnd();
    testMapFileParser_next_initToTextSection();
    testMapFileParser_parse_real();
    testMapFileParser_parseAll();

    testMapFileParser_parse_testMap();

    // kernel side test.
    testBinaryTree_get_lower_nearest();
    testSymbolDictionary_Integrate();
    
    TEST_RESULTS(stack_trace);
    return 0;
}
