#include <locale.h>

static struct lconv l;

// for cairo
struct lconv* localeconv() {
  l.decimal_point = ".";
  return &l;
}
