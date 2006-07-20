#include <monalibc.h>

/**
  \brief double to string

  \param s          string buffer printed characters
  \param n          number converted
  \param real_width width of number to be converted before decimal-point
  \param precision  width of number to be converted after decimal-point
  \param flag       conversion using flag
  \return size of characters printed to buffer
 */
int ftos(char *s, double n, int width, int precision, char flag) {
    int num, fraction;
    int j = 0;
    char tmpflag = 0;

    if(s == NULL) {
        return 0;
    }

    num = (int)n;
    fraction = (int)((n - (double)num)*pow(10, precision));
    if(precision > 0) {
        if(flag & P_FORMAT_TERMINATE) {
            flag &= ~P_FORMAT_TERMINATE;
            tmpflag = P_FORMAT_TERMINATE;
        }
        if(num != 0) {
            width -= (precision + 1);
            j += itos(s, num, width, 10, flag);
            s[j] = '.';
            j++;
            if(fraction < 0) fraction *= -1;
            j += itos(&s[j], fraction, precision, 10, tmpflag);
        } else { /* num == 0 */
            if(fraction >= 0) {
                s[j++] = '0';
                s[j++] = '.';
                j += itos(&s[j], fraction, precision, 10, tmpflag);
            } else { /* num == 0 && fraction < 0 */
                s[j++] = '-';
                s[j++] = '0';
                s[j++] = '.';
                fraction *= -1;
                j += itos(&s[j], fraction, precision, 10, tmpflag);
            }
        }
    } else { /* precision <= 0 */
        j += itos(s, num, width, 10, flag);
    }

    return j;
}

