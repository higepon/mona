#include <userlib.h>
#include <collection.h>

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

MapServer::MapServer() {
}

MapServer::~MapServer() {
}

void MapServer::service() {

    map_ = new HashMap<MapInfo*>(1024);
    if (map_ == NULL) {
        printf("MapServer::unable to allocate map\n");
        return;
    }

    registerMapInfo("mona://device/fd/buffer", new MapInfo(SHARED_FDC_BUFFER, 4096, 0x20000000, 0x21000000));
}

bool MapServer::registerMapInfo(char* identifier, MapInfo* info) {

    MapInfo* prevInfo = map_->put(identifier, info);

    /* check value is overwritten */
    return prevInfo == info;
}

MapInfo* MapServer::getMapInfo(char* identifier) const {
    return map_->get(identifier);
}

bool MapServer::map(dword pid, char* identifier, dword linearAddress) {

    MapInfo* info = getMapInfo(identifier);

    /* map service not found */
    if (info == NULL) {
        return false;
    }

    /* range check */
    if (!info->inRange(linearAddress)) {
        return false;
    }

    return syscall_map(pid, info->getId(), linearAddress, info->getSize()) ;
}

int main() {

    printf("Hello World\n");


    dword pid = Message::lookup("HELLO.ELF");

    Server* server = new MapServer();
    server->service();
    ((MapServer*)(server))->map(pid, "mona://device/fd/buffer", 0x20000000);
    printf("str=%s", (char*)(0x20000000));

    for (;;);
    return 0;
}
