#include <monalibc.h>

/**
 * \brief int to string
 *
 * \param s     string buffer printed characters
 * \param n     number converted
 * \param width minimum field width of number to be converted
 * \param base  radix of number to be converted
 * \param flag  conversion using flag
 * \return size of characters printed to buffer
 *
 */
int itos(char *s, int n, int width, unsigned int base, char flag) {
    return itosn(s, -1, n, width, base, flag);
}

/*!
 * \brief unsigned int to string
 *
 * \param s         string buffer printed characters
 * \param max_width maximum field width of number to be converted
 *                  (nearly equal string buffer size), 0< : infinity
 * \param n         number converted
 * \param width     minimum field width of number to be converted
 * \param base      radix of number to be converted
 * \param flag      conversion using flag
 * \return size of characters printed to buffer
 *
 */
int itosn(char *s, int max_width, int n, int width, unsigned int base, char flag) {
    int num;
    int real_width;
    int i;
    int j = 0;
    char charP = '+';

    if((s == NULL) || (max_width == 0) || (base == 0) || (base > 36)) {
        return 0;
    }

    if((max_width > 0) && (max_width < width)) width = max_width;

    if(!(flag & P_FORMAT_UNSIGNED) && (n < 0)) {/* negative number */
        flag |= P_FORMAT_PLUS;
        charP = '-';
        n = n * -1;
    }
    num = n;

    if((flag & P_FORMAT_SPACE) && !(flag & P_FORMAT_PLUS)) {
        flag |= P_FORMAT_PLUS;
        charP = ' ';
    }

    if(num == 0) {
        real_width = 1;
    } else {
        for(real_width = 0; num != 0; real_width++) {
            num /= base;
        }
    }

    if((flag & P_FORMAT_ZERO) && (real_width >= width)) {
        flag &= ~P_FORMAT_ZERO;
    }

    if(flag & P_FORMAT_PLUS) {
        if(flag & P_FORMAT_MINUS) {
            s[j] = charP;
            j++;
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
            for(i = 0; j < width; i++) {
                s[j] = ' ';
                j++;
            }
        } else if(flag & P_FORMAT_ZERO) {
            s[j] = charP;
            j++;
            for(i = 0; j < width - real_width; i++) {
                s[j] = '0';
                j++;
            }
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
        } else {
            for(i = 0; j < width - real_width - 1; i++) {
                s[j] = ' ';
                j++;
            }
            s[j] = charP;
            j++;
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
        }
    } else {
        if(flag & P_FORMAT_MINUS) {
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
            for(i = 0; j < width; i++) {
                s[j] = ' ';
                j++;
            }
        } else if(flag & P_FORMAT_ZERO) {
            for(i = 0; j < width - real_width; i++) {
                s[j] = '0';
                j++;
            }
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
        } else {
            for(i = 0; j < width - real_width; i++) {
                s[j] = ' ';
                j++;
            }
            j += uitosn(&s[j], max_width - j, n, real_width, base, flag);
        }
    }
    if(flag & P_FORMAT_TERMINATE) {
        s[j] = 0;
        j++;
    }
    return j;
}


