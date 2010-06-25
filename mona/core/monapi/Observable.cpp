/*----------------------------------------------------------------------
    Observer/Observable
----------------------------------------------------------------------*/

#include <monapi.h>

namespace MonAPI {

Observable::Observable() : changed(false) {}

Observable::~Observable() {}

void Observable::addObserver(Observer* o) {
    this->observers.add(o);
}

void Observable::deleteObservers() {

    Observer* toDelete;
    for (int i = this->observers.size() - 1; i >=0; i--) {
        this->observers.removeAt(i, &toDelete);
    }
}

void Observable::setChanged() {
    this->changed = true;
}

void Observable::clearChanged() {
    this->changed = false;
}

bool Observable::hasChanged() const {
    return this->changed;
}

int Observable::countObservers() {
    return observers.size();
}

void Observable::notifyObservers() {

    this->notifyObservers(NULL);
}

void Observable::notifyObservers(void* arg) {

    for (int i = 0; i < this->observers.size(); i++) {
        this->observers.get(i)->update(this, arg);
    }
    clearChanged();
}

}
