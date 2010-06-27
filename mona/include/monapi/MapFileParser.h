/*----------------------------------------------------------------------
    MapFileParser
----------------------------------------------------------------------*/

#pragma once

#include <monapi/cmemoryinfo.h>
// #include <monalibc/stdio.h>
#include <monalibc/limits.h>
#include <monalibc/stdlib.h> // algorithm require rand()
#include <sys/types.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// this file is  also included in standard libc related exe for test.
#ifdef assert
#define MAP_FILE_ASSERT(cond) assert(cond)
#else
#define MAP_FILE_ASSERT(cond) {}
#endif


class FileReader
{
  public:
    static const int EOF = -1;
    FileReader() : pos_(0), cm_(NULL), eof_(false){}
    inline bool end() { return eof_; }
    bool open(const std::string& path)
    {
        cm_ = monapi_file_read_all(path.c_str());
        return cm_ != NULL;
    }
    void close()
    {
        close_if_necessary();
    }
    int getc()
    {
        if(cm_->Size <= pos_)
            return EOF;
        return cm_->Data[pos_++];
    }
    std::string getLine()
    {
        std::string str("");
        int ch;
        while((ch = getc()) != EOF)
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
        if(cm_ != 0)
        {
            monapi_cmemoryinfo_dispose(cm_);
            monapi_cmemoryinfo_delete(cm_);
            cm_ = NULL;
        }
    }
    uint32_t pos_;
    monapi_cmemoryinfo *cm_;
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
        MAP_FILE_ASSERT(lineType() == MAP_FILE_LINE_TYPE_FILE);
        return currentLine_.substr(38);
    }

    std::string address() {
        MAP_FILE_ASSERT(lineType() == MAP_FILE_LINE_TYPE_FILE || lineType() == MAP_FILE_LINE_TYPE_FUNCTION);
        if (notSpaceAt(26)) {
            // 64bit Linux: 0xffffffffa0004300
            return std::string("0x") + currentLine_.substr(26, 8);
        } else {
            return currentLine_.substr(16, 10);
        }
    }

    std::string functionName() {
        MAP_FILE_ASSERT(lineType() == MAP_FILE_LINE_TYPE_FUNCTION);
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

        MAP_FILE_ASSERT(address.size() == 10);

        // begin with 0x
        for(int i = 2; i < 10; i++)
        {
            if('a' <= address[i] && 'f' >= address[i])
            {
                res |= (address[i] - 'a' + 10) << (9 - i)*4;
            }
            else
            {
                MAP_FILE_ASSERT('0' <= address[i] && '9' >= address[i]);
                res |= (address[i] - '0') << (9 - i)*4;
            }
        }
        return res;
    }

    void serialize(const SymbolInfoEntry &ent, uint8_t* data, int size)
    {
        MAP_FILE_ASSERT(calcSize(ent) <= size);

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
        MAP_FILE_ASSERT(ent.Address.size() == 10);
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
        MAP_FILE_ASSERT(state_ == MAP_FILE_STATE_INIT);
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
                MAP_FILE_ASSERT(false);
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
            MAP_FILE_ASSERT(false);
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


