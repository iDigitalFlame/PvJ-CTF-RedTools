#define HOST "0.0.0.0\0"
#define PORT "443\0"
#define TOKEN "<token>>\0"
#define _WIN32_WINNT 0x0501

#include <ws2tcpip.h>

int msg(char *host, char *port, char *data) {
    SOCKET s;
    struct addrinfo i, *a, *c;
    memset(&i, 0, sizeof(i));
    i.ai_family = PF_INET;
    i.ai_socktype = SOCK_STREAM;
    if (getaddrinfo(host, port, &i, &a) != 0) {
        return 0;
    }
    for (c = a; c != NULL; c = c->ai_next) {
        s = socket(c->ai_family, c->ai_socktype, c->ai_protocol);
        if (s == INVALID_SOCKET) {
            continue;
        }
        if (connect(s, c->ai_addr, (int)c->ai_addrlen) != SOCKET_ERROR) {
            break;
        }
        closesocket(s);
    }
    freeaddrinfo(a);
    send(s, data, strlen(data), 0);
    closesocket(s);
    return 1;
}
int main() {
    WSADATA w;
    if (WSAStartup(MAKEWORD(2, 2), &w) != 0) {
        return 1;
    }
    msg(HOST, PORT, TOKEN);
    WSACleanup();
    return 0;
}