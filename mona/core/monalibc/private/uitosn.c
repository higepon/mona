#include <monalibc.h>
/*!
 * \brief unsigned int to string with max size n
 *
 * \param s          string buffer printed characters
 * \param max_width  maximum field width of number to be converted
 *                   (nearly equal string buffer size), 0< : infinity
 * \param n          number converted
 * \param real_width width of number to be converted
 * \param base       radix of number to be converted
 * \param flag       conversion using flag
 * \return size of characters printed to buffer
 */
int uitosn(char* s, int max_width, unsigned int n, int real_width, unsigned int base, char flag) {
    int j = 0;
    int i;
    size_t ch;
    char basechar = 'a';

    if((s == NULL) || (max_width == 0)) return 0;

    if(flag & P_FORMAT_CAPITAL) {
        basechar = 'A';
    }

    for(i = real_width -1; i >= 0; i--) {
        ch = n / pow(base, i);
        n %= (int)pow(base, i);

        if (ch > 9) {
            s[j] = (basechar + ch -10);
            j++;
        } else {
            s[j] = ('0' + ch);
            j++;
        }
        if((max_width - j) == 0) break;
    }

    return j;
}

