#ifndef _MONA_USERLIB_OBSERVER_
#define _MONA_USERLIB_OBSERVER_

#include <sys/types.h>
#include <sys/HList.h>

namespace MonAPI {

/*----------------------------------------------------------------------
    Observable/Observer
----------------------------------------------------------------------*/
class Observable;
interface Observer;
interface Observer {

  public:
    virtual void update(Observable* o, void* arg) = 0;
};

class Observable {

  public:
    Observable() : changed(false) {}
    virtual ~Observable(){}

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
