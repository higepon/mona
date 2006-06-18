//$Id$
#pragma once
#include <monapi.h>
#include <sys/types.h>
#include <Connection.h>
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
    HList<ConnectionInfo*> cinfolist;
    void read_bottom_half(int,ConnectionInfo*); 
    void write_bottom_half(int,ConnectionInfo*);
    void Dispose(int n){ nic->Delete(n); } 
public:    
    void DoDispatch();
    Dispatch();
    virtual ~Dispatch();
    bool initialize();
    void  interrupt();
    int  Send(byte* ,int, ConnectionInfo* );
    void readStatus(NetStatus* stat){ nic->getStatus(stat); }
    void PeriodicUpdate();
    ConnectionInfo* GetConnection(int n){ return cinfolist.get(n); }
    void AddConnection(ConnectionInfo* c){cinfolist.add(c); }
    void RemoveConnection(ConnectionInfo*,dword);
    int ConnectionNum(){ return cinfolist.size();}
private:

};

};

