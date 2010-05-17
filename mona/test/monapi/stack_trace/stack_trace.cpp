#include <monapi.h>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <stdio.h>
#include <limits.h>
#include <string>

using namespace MonAPI;

#define TEST_MAP "/APPS/TSTACK.APP/TEST.MAP"

class FileReader
{
  public:
    FileReader() : fp_(NULL), pos_(0){}
    bool open(const std::string& path)
    {
        fp_ = fopen(path.c_str(), "r");
        return fp_ != NULL;
    }
    void close()
    {
        close_if_necessary();
    }
    void close_if_necessary()
    {
        if(fp_ != NULL)
        {
            fclose(fp_);
            fp_ = NULL;
        }
    }
    std::string getLine()
    {
        std::string str;
        int ch;
        while((ch = fgetc(fp_)) != EOF)
        {
            if(ch == '\n')
              return str;
            if(ch != '\r') // just ignore \r
              str += (char)ch;
        }
        return str;
    }
    
    ~FileReader()
    {
        close_if_necessary();
    }
  private:
    FILE *fp_;
    int pos_;
};

void testOpen()
{
    FileReader reader;
    EXPECT_TRUE(reader.open(TEST_MAP));
}

void testReadFirstLine()
{
    FileReader reader;
    EXPECT_TRUE(reader.open(TEST_MAP));
    EXPECT_TRUE(0 == strcmp("Archive member included because of file (symbol)", reader.getLine().c_str()));
}


int main(int argc, char *argv[])
{
    testOpen();
    testReadFirstLine();
    
    TEST_RESULTS(stack_trace);
    return 0;
}
