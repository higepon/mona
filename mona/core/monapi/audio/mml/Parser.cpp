#include <monapi/audio/mml/Parser.h>


using namespace std;
using namespace MonAPI::audio::mml;

// temporary

static int __isdigit(int c)
{
    if( c >= 0x30 && c <= 0x39 )
        return 1;
    else
        return 0;
}

static int __islower(int c)
{
    if( c >= 0x61 && c <= 0x7a )
        return 1;
    else
        return 0;
}

static int __toupper(int c)
{
    if(__islower(c))
      return c += 'A' - 'a';
    else
      return c;
}

static int __isspace(int c)
{
    if( c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v' )
        return 1;
    else
        return 0;
}

static int __atoi(const char *s){
    int result;
    int mflag;

    if(s == NULL) return 0;
    mflag = 1;
    result = 0;

    while(__isspace(*s)) s++;

    if(*s == '+'){
        mflag = 1;
        s++;
    } else if(*s == '-'){
        mflag = -1;
        s++;
    }

    while((*s >= '0') && (*s <= '9')){
        result = result*10 + (int)(*s - '0');
        s++;
    }

    return result*mflag;
}




// tempo(120) means 4=500ms 8=250ms

const uint32_t Parser::pitchTable[] =
{
/*        C     C#    D     D#    E     F     F#    G     G#    A     A#    B*/
/* 0 */   65,   69,   73,   78,   82,   87,   93,   98,  103,  110,  117,  123,
/* 1 */  131,  139,  147,  156,  165,  175,  185,  196,  208,  220,  233,  247,
/* 2 */  262,  277,  294,  311,  330,  349,  370,  392,  415,  440,  466,  494,
/* 3 */  523,  554,  587,  622,  659,  698,  740,  784,  831,  880,  932,  988,
/* 4 */ 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1975,
/* 5 */ 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951,
/* 6 */ 4186, 4435, 4698, 4978, 5274, 5588, 5920, 6272, 6644, 7040, 7459, 7902,
};

const uint8_t Parser::pitchIndex[] = {9, 11, 0, 2, 4, 5, 7};

Parser::Parser() : channel_(NULL), config_(NULL), postion_(0)
{

}

Parser::~Parser()
{
}

uint32_t Parser::calcMilliSecond(double value)
{
    return (uint32_t)((60 * 1000 / config_->tempo) * (4 / value));
}

Channels* Parser::parse(const std::string& text)
{
    channels_ = new Channels;
    channel_ = new Channel;
    config_ = new Config;
    title_ = "no title";
    text_ = text;
    postion_ = 0;
    parseInternal();
    Channels* ret = channels_;
    delete config_;
    config_ = NULL;
    channel_ = NULL;
    channels_ = NULL;
    return ret;
}

void Parser::destroyChannels(Channels* channels)
{
    for (Channels::const_iterator it = channels->begin(); it != channels->end(); ++it)
    {
        Channel* channel = *it;
        for (Channel::iterator eit = channel->begin(); eit != channel->end(); ++eit)
        {
            delete(*eit);
        }
        delete channel;
    }
    delete channels;
}

int Parser::parseInternal()
{
    for (;;)
    {
        char c = getChar();
        if (EOF == c)
        {
            channels_->push_back(channel_);
            return MONA_SUCCESS;
        }
        if (__isspace(c)) continue;

        switch(c)
        {
        case ';':
        {
            std::string title = "";
            for (;;)
            {
                char n = getChar();
                if (n == '\n')
                {
                    title_ = title;
                    break;
                }
                else
                {
                    title += n;
                }
            }
            break;
        }
        case '&':
        {
            /* donothing */
            break;
        }
        case '>':
        {
            config_->octave++;
            break;
        }
        case '<':
        {
            config_->octave--;
            break;
        }
        case 'Q':
        {
            double number;
            if (getNumber(&number))
            {
                config_->q = (uint32_t)number;
            }
            else
            {

                warn("ignored unknown Q syntax\n");
            }
            break;
        }
        case 'L':
        {
            double number;
            if (getNumber(&number))
            {
                config_->l = (uint32_t)number;
            }
            else
            {

                warn("ignored unknown L syntax\n");
            }
            break;
        }
        case 'T':
        {
            double number;
            if (getNumber(&number))
            {
                config_->tempo = (uint32_t)number;
            }
            else
            {
                warn("ignored unknown T syntax\n");
            }
            break;
        }
        case 'V':
        {
            double number;
            if (getNumber(&number))
            {
                config_->volume = (uint16_t)number;
            }
            else
            {
                warn("ignored unknown V syntax\n");
            }
            break;
        }
        case 'O':
        {
            double number;
            if (getNumber(&number))
            {
                config_->octave = (uint32_t)number;
            }
            else
            {
                warn("ignored unknown O syntax\n");
            }
            break;
        }
        case 'R':
        {
            double number;
            Element* element = new Element();
            element->rest = true;
            if (getNumber(&number))
            {
                element->ms = calcMilliSecond(number);
            }
            else
            {
                element->ms = calcMilliSecond(config_->l);
            }
            channel_->push_back(element);
            break;
        }
        case 'A'...'G':
        {
            char modifier = getChar();
            int pitch = pitchIndex[c - 'A'];
            uint32_t octave = config_->octave;
            if (modifier == '#' || modifier == '+')
            {
                pitch++;
                if (pitch == 12)
                {
                    octave++;
                    pitch = 0;
                }
            }
            else if (modifier == '-')
            {
                pitch--;
                if (pitch == -1)
                {
                    octave--;
                    pitch = 11;
                }
            }
            else
            {
                unGetChar();
            }
            double number;
            Element* element = new Element();
            element->volume = config_->volume;
            element->hz = pitchTable[pitch + config_->octave * 12];
            if (getNumber(&number)) {
                element->ms = calcMilliSecond(number);
            }
            else
            {
                element->ms = calcMilliSecond(config_->l);
            }
            channel_->push_back(element);
            break;
        }
        case '[':
        {
            string command;
            for (;;)
            {
                char nextC = getChar();
                if (nextC == EOF || nextC == ']')
                {
                    break;
                }
                else
                {
                    command += nextC;
                }
            }
            if (NULL != strstr(command.c_str(), "CHANNEL"))
            {
                if (channel_->size() != 0)
                {
                    channels_->push_back(channel_);
                    channel_ = new Channel;
                    config_->octave = Config::DEFAULT_OCTAVE;
                }
            }

            break;
        }
        default:
            printf("i[%c]", c);
            break;
        }
//        parseInternal();
    }
}

bool Parser::getNumber(double* number)
{
    char buffer[32];
    for (int i = 0; i < 32; i++)
    {
        char c = getChar();
        if (__isdigit(c))
        {
            buffer[i] = c;
        }
        else if (i != 0 && c == '.')
        {
            buffer[i] = '\0';
            *number = __atoi(buffer) / 1.5;
            return true;
        }
        else if (i == 0 && c == '.')
        {
            *number = config_->l * 1.5;
            return true;
        }
        else
        {
            unGetChar();
            if (i == 0)
            {
                return false;
            }
            else
            {
                buffer[i] = '\0';
                *number = __atoi(buffer);
                return true;
            }
        }
    }
    return false;
}

char Parser::getChar()
{
    if (text_.size() <= (size_t)postion_) return EOF;
    int c = text_[postion_];
    postion_++;
    return __toupper(c);
}

void Parser::unGetChar()
{
    postion_--;
    if (postion_ < 0) postion_ = 0;
}

void Parser::warn(const char* msg)
{
    MONAPI_WARN("[warn] %s", msg);
}
