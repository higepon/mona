#ifndef _MONAPI_OBSERVER_
#define _MONAPI_OBSERVER_

#include <sys/types.h>
#include <sys/HList.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Observable/Observer
----------------------------------------------------------------------*/
class Observable;
interface Observer {

  public:
    Observer();
    virtual ~Observer();
    virtual void update(Observable* o, void* arg) = 0;
};

class Observable {

  public:
    Observable();
    virtual ~Observable();

  public:
    virtual void addObserver(Observer* o);
    virtual void notifyObservers();
    virtual void notifyObservers(void* arg);
    virtual void deleteObservers();
    virtual void setChanged();
    virtual void clearChanged();
    virtual bool hasChanged() const;
    virtual int countObservers();

  protected:
    HList<Observer*> observers;
    bool changed;
};

}

#endif
