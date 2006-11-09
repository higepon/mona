#include "Parser.h"


using namespace std;
using namespace MML;

// tempo(120) means 4=500ms 8=250ms

const dword Parser::pitchTable[] =
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

const byte Parser::pitchIndex[] = {9, 11, 0, 2, 4, 5, 7};

Parser::Parser() : elements_(NULL), config_(NULL), postion_(0)
{

}

Parser::~Parser()
{
}

dword Parser::calcMilliSecond(double value)
{
    return (dword)((60 * 1000 / config_->tempo) * (4 / value));
}

Elements* Parser::parse(const std::string& text)
{
    elements_ = new Elements;
    config_ = new Config;
    text_ = text;
    postion_ = 0;
    parseInternal();
    Elements* ret = elements_;
    delete config_;
    config_ = NULL;
    elements_ = NULL;
    return ret;
}

int Parser::parseInternal()
{
    for (;;)
    {
        char c = getChar();
        if (EOF == c) return MONA_SUCCESS;
        if (isspace(c)) continue;

        c = toupper(c);
        switch(c)
        {
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
                config_->q = (dword)number;
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
                config_->l = (dword)number;
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
                config_->tempo = (dword)number;
            }
            else
            {
                warn("ignored unknown T syntax\n");
            }
            break;
        }
        case 'O':
        {
            double number;
            if (getNumber(&number))
            {
                config_->octave = (dword)number;
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
            elements_->push_back(element);
            break;
        }
        case 'A'...'G':
            {
                char modifier = getChar();
                int pitch = pitchIndex[c - 'A'];
                dword octave = config_->octave;
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
                element->hz = pitchTable[pitch + config_->octave * 12];
                if (getNumber(&number)) {
                    element->ms = calcMilliSecond(number);
                }
                else
                {
                    element->ms = calcMilliSecond(config_->l);
                }
                elements_->push_back(element);
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
        if (isdigit(c))
        {
            buffer[i] = c;
        }
        else if (i != 0 && c == '.')
        {
            buffer[i] = '\0';
            *number = atoi(buffer) / 1.5;
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
                *number = atoi(buffer);
                return true;
            }
        }
    }
}

char Parser::getChar()
{
    if (text_.size() <= postion_) return EOF;
    int c = text_[postion_];
    postion_++;
    return c;

}

void Parser::unGetChar()
{
    postion_--;
    if (postion_ < 0) postion_ = 0;
}

void Parser::warn(const char* msg)
{
    printf("[warn] %s", msg);
}
