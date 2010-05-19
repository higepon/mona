#pragma once

#include <sys/BinaryTree.h>
#include <sys/HList.h>

// ugh!
#ifndef _MONAPI_STRING_
extern "C" void* memcpy(void* s1, void* s2, size_t size);
extern "C" size_t strlen(const char*);
#endif


namespace SymbolDictionary {

class SymbolEntry {
public:
    SymbolEntry(char* fname, int fnlen, char *funcName, int funcLen, uint32_t address)
    {
        FileName = allocCString(fname, fnlen);
        FunctionName = allocCString(funcName, funcLen);
        Address = address;
    }
    SymbolEntry(const SymbolEntry& ent)
    {
        Address = ent.Address;
        FileName = allocCString(ent.FileName, strlen(ent.FileName));
        FunctionName = allocCString(ent.FunctionName, strlen(ent.FunctionName));
    }
    bool operator ! () const { return (FileName == NULL) || (FunctionName == NULL); }
    ~SymbolEntry()
    {
        if(FileName)
            delete FileName;
        if(FunctionName)
            delete FunctionName;
    }
    char* FileName;
    char* FunctionName;
    uint32_t Address;

private:
    char* allocCString(char* str, int strlen)
    {
        char* ret = new char[strlen+1];
        if(!ret)
            return NULL;
        memcpy(ret, str, strlen);
        ret[strlen] = '\0';
        return ret;
    }
    
};

template <class T> class SymbolsDeserializer
{
public:
    T& builder_;
    SymbolsDeserializer(T& builder) : builder_(builder) {}

    bool deserialize(uint8_t* data, int size)
    {
        int pos = 0;
        while(pos < size)
        {
            uint32_t* ptr32 = (uint32_t*)&data[pos];
            uint32_t address = *ptr32;
            pos+=4;

            uint16_t* ptr16 = (uint16_t*)&data[pos];
            int fnlen = (int)*ptr16;
            pos+=2;
            char* fname = (char*)&data[pos];
            pos+=fnlen;

            ptr16 = (uint16_t*)&data[pos];
            int funcLen = (int)*ptr16;
            pos+=2;
            char* funcName = (char*)&data[pos];
            pos+=funcLen;

            if(!builder_.add(fname, fnlen, funcName, funcLen, address))
                return false;
        }
        return true;
    }

};

class SymbolDictionary
{
public:
    ~SymbolDictionary()
    {
        for(int i = 0; i < list_.size(); i++)
        {
            SymbolEntry * ent = list_[i];
            delete ent;
        }
    }
    SymbolEntry* lookup(uint32_t address) const {
        return tree_.get_lower_nearest(address);
    }
    bool add(char* fname, int fnlen, char*funcName, int funcLen, uint32_t address)
    {
        SymbolEntry* ent = new SymbolEntry(fname, fnlen, funcName, funcLen, address);
        if(!ent)
            return false;

        list_.add(ent);
        tree_.add(address, ent);
        return true;
    }
    bool deserialize(uint8_t* data, int size)
    {
        SymbolsDeserializer<SymbolDictionary> des(*this);
        return des.deserialize(data, size);
    }

    BinaryTree<SymbolEntry*> tree_;
    HList<SymbolEntry*> list_;
};

class SymbolDictionaryMap
{
public:
    ~SymbolDictionaryMap()
    {
        for(int i = 0; i < list_.size(); i++)
        {
            SymbolDictionary * ent = list_[i];
            delete ent;
        }
    }
    void add(uint32_t pid, SymbolDictionary* dict)
    {
        list_.add(dict);
        tree_.add(pid, dict);
    }
    SymbolDictionary* get(uint32_t pid)
    {
        return tree_.get(pid);
    }
    void remove(uint32_t pid)
    {
        if(tree_.contains(pid))
        {
            SymbolDictionary* ent = tree_.get(pid);
            list_.remove(ent);
            tree_.remove(pid);
            delete ent;
        }
    }
    BinaryTree<SymbolDictionary*> tree_;
    HList<SymbolDictionary*> list_;
};


}

