#ifndef _MONA_MAP_
#define _MONA_MAP_

#include <types.h>
#include <Server.h>
#include <Map.h>

/*----------------------------------------------------------------------
    MapInfo
----------------------------------------------------------------------*/
class MapInfo {

  public:
    MapInfo(dword id, dword size, dword start, dword end) : id_(id), size_(size), start_(start), end_(end) {
    }

    ~MapInfo() {
    }

  public:
    inline dword getId() const {
        return id_;
    }

    inline dword getSize() const {
        return size_;
    }

    inline bool inRange(dword address) const {
        return (address >= start_) && (address <= end_);
    }

  protected:
    dword id_;
    dword size_;
    dword start_;
    dword end_;
    int refCount_;
};

/*----------------------------------------------------------------------
    Map Interface
----------------------------------------------------------------------*/
template <class T> class Map {
public:
    virtual ~Map() {}

public:
    virtual bool containsKey(char* key) = 0;
    virtual T get(char* key)            = 0;
    virtual T put(char* key, T value)   = 0;
    virtual T remove(char* key)         = 0;
    virtual int size()          const = 0;
};

/*----------------------------------------------------------------------
    MapServer
----------------------------------------------------------------------*/
class MapServer : public Server {

  public:
    MapServer();
    virtual ~MapServer();

  public:
    void service();
    bool map(dword pid, char* identifier, dword linearAddress);

  protected:
    bool registerMapInfo(char* identifier, MapInfo* info);
    MapInfo* getMapInfo(char* identifier) const;

  protected:
    Map<MapInfo*>* map_;
};

#endif
