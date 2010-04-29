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

    uint32_t id = monapi_file_open(path, true);
    assert(id);

    monapi_cmemoryinfo* buffer = alloc_buffer(MAXDATA);
    

    
    uint8_t buf[MAXDATA];
    int readSize = recv(sd, buf, MAXDATA, 0);
    do {
        memcpy(buffer->Data, buf, readSize);
        monapi_file_write(id, buffer, readSize);
        monapi_file_seek(id, readSize, SEEK_CUR);
    } while ((readSize = recv(sd, buf, MAXDATA, 0)) > 0);

    monapi_file_close(id);
    free_buffer(buffer);
}


#define PORT    80

static void outputFileContent(char *path)
{
    char buf[2048+1];
    fprintf(stderr, "path=%s\n", path);
    uint32_t id = monapi_file_open(path, false);
    fprintf(stderr, "id=%x\n", id);

    monapi_cmemoryinfo* mi = monapi_file_read(id, 2048);
// OK
    memcpy(buf, mi->Data, mi->Size);
    buf[mi->Size] = '\0';
    fprintf(stderr, "contents:%s\n", buf);
/*
    int column = 0;
    int size = mi->Size > 200 ? 200: mi->Size;
    for(int i = 0; i < size; i++)
      {
          fprintf(stderr, "%2x ", mi->Data[i]);
          column++;
          if(column > 80) {
              column = 0;
              fprintf(stderr, "\n");
          }
      }
*/
    monapi_file_close(id);
    monapi_cmemoryinfo_dispose(mi);
    monapi_cmemoryinfo_delete(mi);
}

int main(int argc, char *argv[])
{
    // temp
    if(argc == 3)
      {
          // usage: frecv.ex5 [path] deb
          outputFileContent(argv[1]);
          return 0;
      }

    
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
