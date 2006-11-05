#ifndef __ELEMENT_H__
#define __ELEMENT_H__

namespace MML {

class Element
{
public:
    Element();
    virtual ~Element();

public:
    dword hz;
    dword ms;
};

}; // namespace MML

#endif // __ELEMENT_H__
