#include "MacroFilter.h"

using namespace monash;
using namespace std;

MacroFilter::MacroFilter()
{
}

MacroFilter::~MacroFilter()
{
}

int MacroFilter::filter(SExp* from)
{
    findAndStoreDefineSyntaxes(from);
    SExp* root = new SExp(SExp::SEXPS);
    SExp* left = new SExp(SExp::SYMBOL);
    left->text = "dummy";
    root->sexps.push_back(left);
    root->sexps.push_back(from);
    tryExpandMacro(NULL, root);
    while (foreachSExps(root,  &MacroFilter::tryExpandMacro));
    return 0;
}

// basic & useful
int MacroFilter::foreachSExp(SExp* root, bool (SExp::*match)() const, int (MacroFilter::*func)(SExp* root, SExp* sexp))
{
    int ret = 0;
    if (root->isSExps())
    {
        // don't use iterator here, sexps.size() will be changed by func
        for (SExps::size_type i = 0; i < root->sexps.size(); i++)
        {
            SExp* sexp = root->sexps[i];
            if ((sexp->*match)())
            {
                ret += (this->*func)(root, sexp);
            }
            ret += foreachSExp(sexp, match, func);
        }
    }
    return ret;
}

int MacroFilter::foreachSymbols(SExp* root, int (MacroFilter::*f)(SExp* root, SExp* sexp))
{
    return foreachSExp(root, &SExp::isSymbol, f);
}

int MacroFilter::foreachSExps(SExp* root, int (MacroFilter::*f)(SExp*root, SExp* sexp))
{
    return foreachSExp(root, &SExp::isSExps, f);
}

int MacroFilter::expandMacro(SExp* root, SExp* sexp)
{
    string name = sexp->text;
    SExps::size_type i;
    for (i = 0; i < root->sexps.size(); ++i)
    {
        if (root->sexps[i] == sexp) break;
    }
    if (bindMap_.find(name) == bindMap_.end())
    {
        if (sexp->isMatchAllKeyword())
        {
            root->sexps.erase(root->sexps.begin() + i);
        }
        return 0;
    }

    BindObject b = bindMap_[name];

    if (sexp->isMatchAllKeyword())
    {
        if (b.sexps.size() == 0)
        {
            root->sexps[i] = b.sexp;
        }
        else
        {
            for (SExps::size_type j = 0; j < b.sexps.size(); j++)
            {
                if (j == 0)
                {
                    root->sexps[i] = b.sexps[j];
                }
                else
                {
                    root->sexps.insert(root->sexps.begin() + i + j, b.sexps[j]);
                }
            }
        }
    }
    else
    {
        root->sexps[i] = b.sexp;
    }
    return 1;
}

int MacroFilter::tryExpandMacro(SExp* dummy, SExp* root)
{
    if (!root->isSExps() || root->sexps.size() <= 0) return 0;
    SExp* left = root->sexps[0];
    if (!left->isSymbol()) return 0;

    string name = left->text;
    Macros::iterator p = macros_.find(name);
    if (p == macros_.end()) return 0;
    Macro* m = (*p).second;

    // todo we should return Macro::Pattern?
    SExp* matchedPattern = m->match(name, root);
    if (NULL == matchedPattern) return 0;

    BindMap bindMap;
    SExp::extractBindings(matchedPattern, root, bindMap);
#if 0
    static int z = 0;
        for (BindMap::const_iterator p = bindMap.begin(); p != bindMap.end(); ++p)
        {
            BindObject b = (*p).second;
            if (b.sexps.size() > 0)
            {
                for (SExps::const_iterator q = b.sexps.begin(); q != b.sexps.end(); ++q)
                {
                    printf("<<%s:%s>>\n", (*p).first.c_str(), (*q)->toString().c_str());fflush(stdout);
                }
            }
            else
            {
                printf("<<%s:%s>>\n", (*p).first.c_str(), b.sexp->toString().c_str());fflush(stdout);
            }
        }
        z++;
//        if (z == 6) exit(-1);
#endif

    SExp* expanded = m->patterns[matchedPattern]->clone();
    bindMap_ = bindMap;
    SExp* wrap =new SExp(SExp::SEXPS);

    // todo fix me! for foreachSExp you need to wrap
    wrap->sexps.push_back(expanded);

    int ret = foreachSymbols(wrap, &MacroFilter::expandMacro);
    expanded = wrap->sexps[0];
    if (ret)
    {
        root->sexps.clear();
        for (SExps::const_iterator p = expanded->sexps.begin(); p != expanded->sexps.end(); ++p)
        {
            root->sexps.push_back(*p);
        }
        root->type = expanded->type;
        root->value = expanded->value;
        root->text = expanded->text;
    }
    return ret;
}

int MacroFilter::findAndStoreDefineSyntaxes(SExp* root)
{
    SExps defineSyntaxes;
    findDefineSyntaxes(root, defineSyntaxes);

    // todo foreach?
    for (SExps::const_iterator p = defineSyntaxes.begin(); p != defineSyntaxes.end(); ++p)
    {
        int ret = storeDefineSyntaxes(*p);
        if (ret != Translator::SUCCESS) return ret;
    }
    return Translator::SUCCESS;
}

void MacroFilter::findDefineSyntaxes(SExp* root, SExps& defineSyntaxes)
{
    if (!root->isSExps() || root->sexps.size() <= 0) return;
    SExp* left = root->sexps[0];
    if (left->isSymbol() && left->text == "define-syntax")
    {
        defineSyntaxes.push_back(root->clone());

//         // ugly fix me.
//         // define-syntax is replaced to true!
        root->sexps.clear();
        root->type = SExp::NUMBER;
        root->value = 1;
        return;
    }

    for (SExps::const_iterator p = root->sexps.begin() + 1; p != root->sexps.end(); ++p)
    {
        findDefineSyntaxes(*p, defineSyntaxes);
    }
    return;
}

int MacroFilter::storeDefineSyntaxes(SExp* sexp)
{
    if (L() != 3) return Translator::SYNTAX_ERROR;
    if (!N(1)->isSymbol()) return Translator::SYNTAX_ERROR;
    if (!N(2)->isSExps() || LL(2) < 3) return Translator::SYNTAX_ERROR;
    if (!NN(2, 0)->isSymbol() || NN(2, 0)->text != "syntax-rules") return Translator::SYNTAX_ERROR;
    if (!NN(2, 1)->isSExps()) return Translator::SYNTAX_ERROR;

    // macro name
    Macro* macro = new Macro(N(1)->text);

    // store reserved words
    for (SExps::const_iterator p = NN(2, 1)->sexps.begin(); p != NN(2, 1)->sexps.end(); ++p)
    {
        SExp* n = (*p);
        if (!n->isSymbol()) return Translator::SYNTAX_ERROR;
        macro->reservedWords.push_back(n->text);
    }
    // store pattern / definition
    for (SExps::size_type i = 2; i < LL(2); ++i)
    {
        SExp* n = NN(2, i);
        if (!n->isSExps() || n->sexps.size() != 2) return Translator::SYNTAX_ERROR;
        renameMatchAllKeywords(n->sexps[0]);
        renameMatchAllKeywords(n->sexps[1]);
        macro->addPattern(n->sexps[0], n->sexps[1]);
    }
    macros_[macro->name] = macro;
    return Translator::SUCCESS;
}

int MacroFilter::renameMatchAllKeywords(SExp* sexp)
{
    index_ = 0;
    return foreachSymbols(sexp, &MacroFilter::renameMatchAllKeyword);
}

// ... => ...n
int MacroFilter::renameMatchAllKeyword(SExp* dummy, SExp* root)
{
    if (root->text == "...")
    {
        char buf[16];
        sprintf(buf, "%s%d", "...", index_);
        root->text = buf;
        index_++;
        return 1;
    }
    return 0;
}
