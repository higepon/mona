// Common/StringToInt.h

#ifndef __COMMON_STRINGTOINT_H
#define __COMMON_STRINGTOINT_H

#include <string.h>
#include "Types.h"

UInt64 ConvertStringToUINT64(const char *s, const char **end);
UInt64 ConvertStringToUINT64(const wchar_t *s, const wchar_t **end);

Int64 ConvertStringToINT64(const char *s, const char **end);

#endif


