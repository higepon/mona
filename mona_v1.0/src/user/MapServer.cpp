#include <userlib.h>
#include <collection.h>
#include <Map.h>

int main() {

    Server* server = new MapServer();
    server->service();

    return 0;
}

MapServer::MapServer() {
}

MapServer::~MapServer() {
}

void MapServer::service() {

    /* regist initial service */
    map_ = new HashMap<MapInfo*>(1024);
    if (map_ == NULL) {
        printf("MapServer::unable to allocate map\n");
        return;
    }

    registerMapInfo("mona://device/fd/buffer", new MapInfo(SHARED_FDC_BUFFER, 4096, 0x20000000, 0x21000000));

    /* Message loop */
    MessageInfo info;
    MessageInfo reply;
    for (;;) {
        /* receive */
        if (!Message::receive(&info)) {

            dword result;

            switch(info.header) {

            case MSG_MAP:

                result = map(info.arg1, info.str, info.arg2);
                reply.header = MSG_RESULT_OK;
                Message::send(info.from, &reply);
                break;

            default:
                /* igonore this message */
                break;
            }
        }
    }
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
