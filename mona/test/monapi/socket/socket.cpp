#include <monapi.h>
#include <map>
#define MUNIT_GLOBAL_VALUE_DEFINED
#include <monapi/MUnit.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <monapi.h>
#include <stdio.h>
#include <limits.h>
#include <string>
#include <time.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

using namespace MonAPI;

static void testSendInvalidFd()
{
    errno = 0;
    EXPECT_EQ(-1, send(-1, NULL, 0, 0));
    EXPECT_EQ(EBADF, errno);
}

static void testGetAddrInfo()
{
    struct addrinfo hints;
    struct addrinfo* res;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("www.monaos.org", "80", &hints, &res));
    EXPECT_EQ(SOCK_STREAM, res->ai_socktype);
    EXPECT_EQ(AF_INET, res->ai_family);
    freeaddrinfo(res);
}

static bool endsWith(const std::string& s, const std::string& t)
{
    size_t i = s.rfind(t);
    return (i != std::string::npos) && (i == (s.length() - t.length()));
}

static void testSocket()
{
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("api.twitter.com", "80", &hints, &res));

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        EXPECT_TRUE(sock != -1);
    }
    freeaddrinfo(res);
}

static void testConnect()
{
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("api.twitter.com", "80", &hints, &res));

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        EXPECT_TRUE(sock != -1);

        int ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        EXPECT_EQ(0, ret);
    }
    freeaddrinfo(res);
}

static void testReceive()
{
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("api.twitter.com", "80", &hints, &res));

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        EXPECT_TRUE(sock != -1);

        int ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        EXPECT_EQ(0, ret);

        const char* message = "status=Yay! Hello, World!. Tweet from Mona OS 0.4.0";
        const char* base64 = "";
        char reqbuf[1024];
        sprintf(reqbuf, "POST /1/statuses/update.xml HTTP/1.1\r\nHost: api.twitter.com\r\nAuthorization: Basic %s\r\nContent-Length: %d\r\n\r\n%s\r\n",
                base64, strlen(message), message);

        EXPECT_TRUE(send(sock, reqbuf, strlen(reqbuf), 0) > 0);
        char buf[127];
        int readSize = recv(sock, buf, 127, 0);
        do {
//             for (int i = 0; i < readSize; i++) {
//                 printf("%c", buf[i]);
//             }
        } while ((readSize = recv(sock, buf, 127, 0)) > 0);

        EXPECT_EQ(0, closesocket(sock));
    }
    freeaddrinfo(res);
}

static void testSocketOption()
{
    struct addrinfo hints;
    struct addrinfo* res;
    struct addrinfo* rp;

    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    EXPECT_EQ(0, getaddrinfo("api.twitter.com", "80", &hints, &res));

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        EXPECT_TRUE(sock != -1);

        socklen_t n = 1;
        EXPECT_EQ(-1, setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &n, sizeof(n)));

        uint8_t buf;
        EXPECT_EQ(-1, getsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &buf, &n));

        int ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
        EXPECT_EQ(0, ret);

        int val = 1;
        EXPECT_EQ(0, ioctlsocket(sock, FIONBIO, &val));
        EXPECT_EQ(0, shutdown(sock, SHUT_RD));
    }
    freeaddrinfo(res);
}

#define PORT    8823
#define MAXDATA 1024
static void testEcho()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    ASSERT_TRUE(sock != -1);

    struct sockaddr_in addr;
    memset(&addr, sizeof(addr), 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(80);

    ASSERT_EQ(0, bind(sock, (struct sockaddr *)&addr, sizeof(addr)));
    ASSERT_EQ(0, listen(sock, 5));

    fd_set rfds;
    struct timeval tv;

    FD_ZERO(&rfds);
    FD_SET(sock, &rfds);

    tv.tv_sec = 0;
    tv.tv_usec = 100;

    int retval = select(sock + 1, &rfds, NULL, NULL, &tv);

    // timeout
    ASSERT_TRUE(retval == 0);

    closesocket(sock);

// needs access from clinet
#if 0
    struct sockaddr_in waddr;
    socklen_t writer_len;

    int s = accept(sock, (struct sockaddr*)&waddr, &writer_len);

    ASSERT_TRUE(s != -1);

    uint8_t buf[127];
    int readSize = recv(s, buf, 127, 0);
    do {
        for (int i = 0; i < readSize; i++) {
            printf("%c", buf[i]);
        }
    } while ((readSize = recv(sock, buf, 127, 0)) > 0);
    const char* res = "HTTP/1.1 200\r\n\r\n\r\nHello";
    send(s, (void*)res, strlen(res), 0);
#endif
}

static void testOpenManyTimes()
{
    for(int i = 0; i < 100; i++)
    {
        struct addrinfo hints;
        struct addrinfo* res;
        struct addrinfo* rp;

        memset(&hints, 0, sizeof(hints));
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_family = AF_INET;

        EXPECT_EQ(0, getaddrinfo("yahoo.co.jp", "80", &hints, &res));

        for (rp = res; rp != NULL; rp = rp->ai_next) {
            int sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            EXPECT_TRUE(sock != -1);

            int ret = connect(sock, rp->ai_addr, rp->ai_addrlen);
            EXPECT_EQ(0, ret);

            EXPECT_EQ(0, closesocket(sock));
        }
        freeaddrinfo(res);
    }
}

#define BUF_LEN 256
static void testSSLHttpGet()
{
    const char *host = "www.hatena.ne.jp";
    const char *path = "/";

    ASSERT(sizeof(unsigned long long) == 8);

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_INET;

    struct addrinfo* res;
    ASSERT_EQ(0, getaddrinfo(host, "443", &hints, &res));

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    ASSERT_TRUE(sock != -1);

    int ret = connect(sock, res->ai_addr, res->ai_addrlen);
    ASSERT_EQ(0, ret);
    SSL_load_error_strings();
    SSL_library_init();
    SSL_CTX *ctx = SSL_CTX_new(SSLv23_client_method());
    ASSERT_TRUE(ctx != NULL);
    SSL *ssl = SSL_new(ctx);
    ASSERT_TRUE(ssl != NULL);
    ret = SSL_set_fd(ssl, sock);
    ASSERT_TRUE(ret != 0);

    RAND_poll();
    while (RAND_status() == 0) {
        unsigned short rand_ret = rand() % 65536;
        RAND_seed(&rand_ret, sizeof(rand_ret));
    }
    ret = SSL_connect(ssl);
    ASSERT_TRUE(ret != -1);
    char request[BUF_LEN];
    sprintf(request,
            "GET %s HTTP/1.0\r\n"
            "Host: %s\r\n\r\n",
            path, host);

    ret = SSL_write(ssl, request, strlen(request));
    ASSERT_TRUE(ret > 0);
    std::string content;
    while (1){
        char buf[BUF_LEN];
        int read_size;
        read_size = SSL_read(ssl, buf, sizeof(buf)-1);

        if ( read_size > 0 ){
            buf[read_size] = '\0';
            content += buf;
        } else if ( read_size == 0 ){
            break;
        } else {
            ASSERT_TRUE(false);
        }
    }
    ASSERT_TRUE(endsWith(content, "</html>\n"));
    ret = SSL_shutdown(ssl);
    ASSERT_EQ(1, ret);
    closesocket(sock);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    ERR_free_strings();
}

int main(int argc, char *argv[])
{
    testSendInvalidFd();
    testGetAddrInfo();
    testSocket();
    testConnect();
    testReceive();
    testSocketOption();
    testEcho();
    testOpenManyTimes();
    testSSLHttpGet();

    // todo close

    TEST_RESULTS(monapi_socket);
    return 0;
}
