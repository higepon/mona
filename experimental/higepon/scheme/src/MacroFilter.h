#ifndef _MACRO_FILTER_
#define _MACRO_FILTER_

#include "Filter.h"
#include "Macro.h"
namespace monash {

    class Macro;
typedef std::map<std::string, Macro*> Macros;

class MacroFilter : public Filter
{
public:
    MacroFilter();
    virtual ~MacroFilter();

    int filter(Node* from, Node** to);
    Node* filterInternal(Node* node);
    void findDefineSyntaxes(Node* root, Nodes& defineSyntaxes);
    int storeDefineSyntaxes(Node* defineSyntax);
    int findAndStoreDefineSyntaxes(Node* root);
    Node* expandMacros(Node* root);
    Node* expandMacroIfMatch(const std::string& name, Node* node);
    Node* expand(Macro* macro, Node* matchedPattern, Node* node);
    void expandInternal(Node* from, BindMap& bindMap);

    int foreachNodes(Node* root, int (MacroFilter::*f)(Node*));
    int tryExpandMacro(Node* node);

private:
    Macros macros_;
};

}; // namespace monash

#endif // _MACRO_FILTER_
