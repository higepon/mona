#ifndef _MACRO_FILTER_
#define _MACRO_FILTER_

#include <Node.h>

namespace monash {

class MacroFilter
{
public:
    MacroFilter();
    virtual ~MacroFilter();

    int filter(Node* from, Node** to);
};

}; // namespace monash

#endif // _MACRO_FILTER_
