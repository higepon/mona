//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include "L4Base.h"
#include "Nic.h"

namespace mones{

class RouteMan
{
    //Not Impremented Yet.
};

class Dispatch :public RouteMan
{
private:
    Nic* nic;    
    HList<L4Base*> cinfolist;
    void ReplyUnReach(Ether*);
    word packetid;
public:      
    int serialno;
    word getPacketID();
    void Dispose(int n){ nic->Delete(n); } 
    Ether* GetFrame(int n){ return nic->RecvFrm(n); }
    void DoDispatch();
    Dispatch();
    virtual ~Dispatch();
    bool initialize();
    void interrupt();
    int  Send(byte* ,int, L4Base* );
    void readStatus(NetStatus*);
    void PeriodicUpdate();
    L4Base* GetInfo(int n){ return cinfolist.get(n); }
    void AddInfo(L4Base* c){cinfolist.add(c); }
    void RemoveInfo(L4Base*,dword);
    int InfoNum(){ return cinfolist.size();}
private:

};

};

