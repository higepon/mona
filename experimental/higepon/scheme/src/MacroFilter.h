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

    int filter(Node* from);
    void findDefineSyntaxes(Node* root, Nodes& defineSyntaxes);
    int storeDefineSyntaxes(Node* defineSyntax);
    int findAndStoreDefineSyntaxes(Node* root);
    int tryExpandMacro(Node* dummy, Node* node);
    int expandMacro(Node* root, Node* node);
    int foreachNode(Node* root, bool (Node::*match)() const, int (MacroFilter::*func)(Node* root, Node* node));
    int foreachNodes(Node* root, int (MacroFilter::*f)(Node*, Node*));
    int foreachSymbols(Node* root, int (MacroFilter::*f)(Node*, Node*));

protected:
    int renameMatchAllKeywords(Node* node);
    int renameMatchAllKeyword(Node* dummy, Node* root);

    Macros macros_;
    BindMap bindMap_;
    int index_;
};

}; // namespace monash

#endif // _MACRO_FILTER_
