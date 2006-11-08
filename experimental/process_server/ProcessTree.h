#ifndef _PROCESS_TREE_H_
#define _PROCESS_TREE_H_

class ProcessInfo{
public:
    ProcessInfo();
    virtual ~ProcessInfo(){};
    virtual int add();
    virtual int remove();

private:
    MonAPI::CString name, path;
    dword parent;
    HList<dword> children;
    HList<dword> receivers;


};


#endif // _PROCESS_TREE_H_
