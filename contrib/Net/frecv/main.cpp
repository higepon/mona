#include <monapi.h>
#include <map>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <monapi.h>
#include <stdio.h>
#include <assert.h>

using namespace MonAPI;

#define MAXDATA 1024

static monapi_cmemoryinfo* alloc_buffer(int size)
{
    monapi_cmemoryinfo* buffer = new monapi_cmemoryinfo();
    monapi_cmemoryinfo_create(buffer, size, 0);
    return buffer;
}

static void free_buffer(monapi_cmemoryinfo* buffer)
{
    monapi_cmemoryinfo_dispose(buffer);
    monapi_cmemoryinfo_delete(buffer);
}



void copyToPath(int sd, const char *name)
{
    char path[5+8+1+3+1]; // "/MEM/[name]"

    assert(strlen(name) <= 8+1+3);
    sprintf(path, "/MEM/%s", name);
    fprintf(stderr, "path: %s\n", path);

    intptr_t id = monapi_file_open(path, true);
    assert(id > 0);

    monapi_cmemoryinfo* buffer = alloc_buffer(MAXDATA);

    uint8_t buf[MAXDATA];
    int readSize = recv(sd, buf, MAXDATA, 0);
    do {
        memcpy(buffer->Data, buf, readSize);
        monapi_file_write(id, buffer, readSize);
    } while ((readSize = recv(sd, buf, MAXDATA, 0)) > 0);

    monapi_file_close(id);
    free_buffer(buffer);
}


#define PORT    80

int main(int argc, char *argv[])
{
    if(argc != 2)
      {
          fprintf(stderr, "usage: frecv [filename]\n");
          return -1;
      }

    if(strlen(argv[1]) > 8+1+3)
      {
          fprintf(stderr, "too long filename. filename must be shorter than 8.3. %s\n", argv[1]);
          return -1;
      }

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    fprintf(stderr, "sock=%x\n", sock);

    struct sockaddr_in addr;
    memset(&addr, sizeof(addr), 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT);

    int ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
    fprintf(stderr, "bind: ret=%x\n", ret);
    ret = listen(sock, 5);
    fprintf(stderr, "listen: ret=%x\n", ret);

    fd_set rfds;

    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);

    // struct timeval tv;
    // tv.tv_sec = 10000; // should be longer?

    int retval = select(sock + 1, &rfds, NULL, NULL,  NULL /* &tv */);
    if(retval == 0) {
        fprintf(stderr, "timeout!\n");
        return -1;
    }
    fprintf(stderr, "select return!\n");



    struct sockaddr_in waddr;
    socklen_t writer_len;

    int s = accept(sock, (struct sockaddr*)&waddr, &writer_len);

    copyToPath(s, argv[1]);
    closesocket(s);
    closesocket(sock);

    return 0;
}
