#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <time.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define ll long long int

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    srand((unsigned int)time(NULL));

    recv(sock, buffer, sizeof(buffer), 0);
    if (strcmp(buffer, "SEND_FRAME_AND_WINDOW_SIZE") == 0) {
        ll tf, N;
        printf("Enter total number of frames: ");
        scanf("%lld", &tf);
        printf("Enter window size: ");
        scanf("%lld", &N);

        snprintf(buffer, sizeof(buffer), "%lld %lld", tf, N);
        send(sock, buffer, strlen(buffer), 0);
    }

    while (1) {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(sock, buffer, sizeof(buffer), 0);

        if (valread > 0) {
            printf("%s\n", buffer);
            
            char ack[10];
            printf("Enter ACK or NACK for the above frame: ");
            scanf("%s", ack);
            send(sock, ack, strlen(ack), 0);
            Sleep(500);
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}

