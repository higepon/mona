#include <string>

class URL
{
public:
    URL(std::string url) : url_(url), parsed_(false)
    {
        parse();
    }
    virtual ~URL() {}

    bool isValid() const { return parsed_;}
    std::string toString() const { return url_;}

public:
    unsigned char getA() const { return a_;}
    unsigned char getB() const { return b_;}
    unsigned char getC() const { return c_;}
    unsigned char getD() const { return d_;}
    std::string getPath() const { return path_;}

private:
    void parse()
    {
        int result = sscanf(url_.c_str(), "http://%d.%d.%d.%d%s", &a_, &b_, &c_, &d_, path_);
        if (result == 4)
        {
            path_[0] = '\0';
        }
        parsed_ = result >= 4;
    }

protected:
    unsigned char a_, b_, c_, d_;
    char path_[256];
    std::string url_;
    bool parsed_;
};
