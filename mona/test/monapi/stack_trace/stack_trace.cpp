#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>

#include <stdio.h>
#include <limits.h>
#include <string>
#include <vector>
#include <map>
#include <assert.h>

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
    SymbolInfoEntry(const std::string& funcName,
                    const std::string& address) : FunctionName(funcName), Address(address){}
    std::string FunctionName;
    std::string Address;
};

typedef std::vector<SymbolInfoEntry> SymbolInfoEntries;
typedef std::map<std::string, SymbolInfoEntries> FileEntriesMap;



class SymbolInfoCollection
{
public:
    SymbolInfoCollection() : current_(NULL) {}

    void addFileEntry(const std::string& fileName){
        SymbolInfoEntries ent;
        Files[fileName] = ent;
        current_ = &Files[fileName];
    }
    void addFunctionEntry(const std::string& address, const std::string& functionName) {
        if(current_ == NULL) {
            printf("function entry before file entry. unknown map file format\n");
            return;
        }

        SymbolInfoEntry ent(functionName, address);
        current_->push_back(ent);
    }
    FileEntriesMap Files;


    void DebugDump()
    {
        for(FileEntriesMap::iterator it = Files.begin(); it != Files.end(); it++)
        {
            _printf("file=%s\n", it->first.c_str());
            SymbolInfoEntries ents = it->second;
            for(SymbolInfoEntries::iterator entIt = ents.begin(); entIt != ents.end(); entIt++)
            {
                _printf("  func=%s, address=%s\n", entIt->FunctionName.c_str(), entIt->Address.c_str());
            }
        }
    }
private:
    SymbolInfoEntries* current_;
};

void testSymbolInfoCollection() {
    SymbolInfoCollection col;
    col.addFileEntry("test.c");
    col.addFunctionEntry("0xaaaaa", "myFunc");
    col.addFunctionEntry("0xbbbbb", "myFunc2");

    col.addFileEntry("test2.c");
    col.addFunctionEntry("0xccccc", "hogeFunc");

    EXPECT_EQ(2, col.Files.size());

    SymbolInfoEntries ents = col.Files["test.c"];
    EXPECT_EQ(2, ents.size());
    EXPECT_STR_EQ("0xaaaaa", ents[0].Address.c_str());
    EXPECT_STR_EQ("myFunc", ents[0].FunctionName.c_str());
    EXPECT_STR_EQ("0xbbbbb", ents[1].Address.c_str());
    EXPECT_STR_EQ("myFunc2", ents[1].FunctionName.c_str());

    ents = col.Files["test2.c"];
    EXPECT_EQ(1, ents.size());
    EXPECT_STR_EQ("0xccccc", ents[0].Address.c_str());
    EXPECT_STR_EQ("hogeFunc", ents[0].FunctionName.c_str());
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

static void expectEntryLine(const char* address, const char *funcName, SymbolInfoEntry& ent, int line)
{
    EXPECT_STR_EQ_LINE(address, ent.Address.c_str(), line);
    EXPECT_STR_EQ_LINE(address, ent.Address.c_str(), line);
}

#define EXPECT_ENTRY(a, f, e) expectEntryLine((a), (f), (e), __LINE__)

static void EXPECT_REAL_CONTENT_COL(SymbolInfoCollection& col)
{
    EXPECT_EQ(3, col.Files.size());

    SymbolInfoEntries ents = col.Files["c:/Users/mumurik/program/monaos/mona/mona/lib/monapi_crt.o"];
    EXPECT_EQ(2, ents.size());
    EXPECT_ENTRY("0xa0001009", "__fu0__monapi_memory_initialized", ents[0] );
    EXPECT_ENTRY("0xa0001000", "_user_start", ents[1] );

    ents = col.Files["c:/Users/mumurik/program/monaos/mona/mona/lib\\libmonapi-imp.a(d000430.o)"];
    EXPECT_EQ(0, ents.size());

    ents = col.Files["c:/Users/mumurik/program/monaos/mona/mona/lib\\libmonapi-imp.a(d000555.o)"];
    EXPECT_EQ(1, ents.size());
    EXPECT_ENTRY("0xa0001220", "_user_start_c_impl", ents[0] );
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

    parser.symbolInfos_.DebugDump();
}


int main(int argc, char *argv[])
{
    testFileReader_Open();
    testFileReader_getLine_once();
    testFileReader_getLine_twice();

    testStringReaderForTest();

    testSymbolInfoCollection();

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
    
    TEST_RESULTS(stack_trace);
    return 0;
}
