#include <monalibc/math.h>
#include <monalibc/ctype.h>

// http://www.hcn.zaq.ne.jp/no-ji/lib/ColorSpace.c
void HSBtoRGB(double hue, double sat, double bri, double* r, double* g, double* b)
{
        double phase, ofs, d1, d2, d3;

        if(sat==0.0) {
                *r = *g = *b = bri;
                return;
        }
        phase = ( hue - floor(hue) ) * 6.0;
        ofs = phase - floor(phase);
        d1 = bri * ( 1.0 - sat );
        d2 = bri * ( 1.0 - sat * ofs );
        d3 = bri * ( 1.0 - sat * (1.0 - ofs) );

        switch((int)phase) {
        case 0:
                *r = bri;
                *g = d3;
                *b = d1;
                break;
        case 1:
                *r = d2;
                *g = bri;
                *b = d1;
                break;
        case 2:
                *r = d1;
                *g = bri;
                *b = d3;
                break;
        case 3:
                *r = d1;
                *g = d2;
                *b = bri;
                break;
        case 4:
                *r = d3;
                *g = d1;
                *b = bri;
                break;
        case 5:
                *r = bri;
                *g = d1;
                *b = d2;
                break;
        }
}

double atof(const char *s)
{
    double a = 0.0;
    int e = 0;
    int c;
    int minus;

    c = *s;
    if (c == '-')
    {
        s++;
        minus = -1;
    }
    else
    {
        minus = 1;
    }

    while ((c = *s++) != '\0' && isdigit(c)) {
        a = a*10.0 + (c - '0');
    }
    if (c == '.') {
        while ((c = *s++) != '\0' && isdigit(c)) {
            a = a*10.0 + (c - '0');
            e = e-1;
        }
    }
    if (c == 'e' || c == 'E') {
        int sign = 1;
        int i = 0;
        c = *s++;
        if (c == '+')
            c = *s++;
        else if (c == '-') {
            c = *s++;
            sign = -1;
        }
        while (isdigit(c)) {
            i = i*10 + (c - '0');
            c = *s++;
        }
        e += i*sign;
    }
    while (e > 0) {
        a *= 10.0;
        e--;
    }
    while (e < 0) {
        a *= 0.1;
        e++;
    }
    return a * minus;
}
