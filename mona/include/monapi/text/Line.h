#ifndef _MONAPI_TEXT_LINE_
#define _MONAPI_TEXT_LINE_

#include "sys/types.h"
#include "monapi/Assert.h"
#include "monapi/CString.h"

#ifdef TEXT_TEST
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#else
#include <monapi.h>
#endif

namespace MonAPI {
namespace text {

class Line
{
public:
    Line();
    virtual ~Line();

    void write(const ::MonAPI::CString& text);
    ::MonAPI::CString get();
    int getCursorPosition();
    void moveCursorLeft();
    void moveCursorRight();
    void reset();

protected:
    int position_;
    CString text_;
};

}; // namespace text
}; // namespace MonAPI

#endif // _MONAPI_TEXT_LINE_
