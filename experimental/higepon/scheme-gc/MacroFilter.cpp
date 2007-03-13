#include "MacroFilter.h"

using namespace util;
using namespace monash;

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
    root->sexps.add(left);
    root->sexps.add(from);
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
        for (int i = 0; i < root->sexps.size(); i++)
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
    String name = sexp->text;
    int i;
    for (i = 0; i < root->sexps.size(); i++)
    {
        if (root->sexps[i] == sexp) break;
    }
    if (bindMap_.get(name.data()) == NULL)
    {
        if (sexp->isMatchAllKeyword())
        {
            root->sexps.removeAt(i);
        }
        return 0;
    }

    BindObject* b = bindMap_.get(name.data());

    if (sexp->isMatchAllKeyword())
    {
        if (b->sexps.size() == 0)
        {
            root->sexps.set(i, b->sexp);
        }
        else
        {
            for (int j = 0; j < b->sexps.size(); j++)
            {
                if (j == 0)
                {
                    root->sexps.set(i, b->sexps[j]);
                }
                else
                {
                    root->sexps.insert(i + j, b->sexps[j]);
                }
            }
        }
    }
    else
    {
        root->sexps.set(i, b->sexp);
    }
    return 1;
}

int MacroFilter::tryExpandMacro(SExp* dummy, SExp* root)
{
    if (!root->isSExps() || root->sexps.size() <= 0) return 0;
    SExp* left = root->sexps[0];
    if (!left->isSymbol()) return 0;

    String name = left->text;
    Macro* m = macros_.get(name.data());
    if (NULL == m) return 0;

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
                    printf("<<%s:%s>>\n", (*p).first.data(), (*q)->toString().data());fflush(stdout);
                }
            }
            else
            {
                printf("<<%s:%s>>\n", (*p).first.data(), b.sexp->toString().data());fflush(stdout);
            }
        }
        z++;
//        if (z == 6) exit(-1);
#endif

    SExp* expanded = m->findPattern(matchedPattern)->clone();
    bindMap_ = bindMap;
    SExp* wrap =new SExp(SExp::SEXPS);

    // todo fix me! for foreachSExp you need to wrap
    wrap->sexps.add(expanded);

    int ret = foreachSymbols(wrap, &MacroFilter::expandMacro);
    expanded = wrap->sexps[0];
    if (ret)
    {
        root->sexps.clear();
        for (int i = 0; i < expanded->sexps.size(); i++)
        {
            root->sexps.add(expanded->sexps.get(i));
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
    for (int i = 0; i < defineSyntaxes.size(); i++)
    {
        int ret = storeDefineSyntaxes(defineSyntaxes[i]);
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
        defineSyntaxes.add(root->clone());

//         // ugly fix me.
//         // define-syntax is replaced to true!
        root->sexps.clear();
        root->type = SExp::NUMBER;
        root->value = 1;
        return;
    }

    for (int i = 1; i < root->sexps.size(); i++)
    {
        findDefineSyntaxes(root->sexps[i], defineSyntaxes);
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
    for (int i = 0; i < NN(2, 1)->sexps.size(); i++)
    {
        SExp* n = NN(2, 1)->sexps[i];
        if (!n->isSymbol()) return Translator::SYNTAX_ERROR;
        macro->reservedWords.add(n->text);
    }
    // store pattern / definition
    for (int i = 2; i < LL(2); ++i)
    {
        SExp* n = NN(2, i);
        if (!n->isSExps() || n->sexps.size() != 2) return Translator::SYNTAX_ERROR;
        renameMatchAllKeywords(n->sexps[0]);
        renameMatchAllKeywords(n->sexps[1]);
        macro->addPattern(n->sexps[0], n->sexps[1]);
    }
    macros_.put(macro->name.data(), macro);
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
