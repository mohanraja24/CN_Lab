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

void transmission(SOCKET client_socket, ll tf, ll N) {
    ll tt = 0;
    ll i = 1;
    char buffer[1024];

    while (i <= tf) {
        int z = 0;

        // Send frames in the current window
        for (int k = i; k < i + N && k <= tf; k++) {
            snprintf(buffer, sizeof(buffer), "Sending Frame %lld", k);
            send(client_socket, buffer, strlen(buffer), 0);
            printf("%s\n", buffer);
            tt++;
            Sleep(500); // 500 ms delay
        }

        // Check acknowledgments for frames in the window
        for (int k = i; k < i + N && k <= tf; k++) {
            memset(buffer, 0, sizeof(buffer));

            fd_set readfds;
            struct timeval tv;
            FD_ZERO(&readfds);
            FD_SET(client_socket, &readfds);
            tv.tv_sec = 8;
            tv.tv_usec = 0;

            int activity = select(0, &readfds, NULL, NULL, &tv);

            if (activity > 0) {
                int valread = recv(client_socket, buffer, sizeof(buffer), 0);
                if (valread > 0 && strcmp(buffer, "ACK") == 0) {
                    printf("Acknowledgment received for Frame %lld\n", k);
                    z++;
                } else {
                    printf("Timeout or NACK! Frame Number %lld Not Received\n", k);
                    printf("Retransmitting Window...\n");
                    break;
                }
            } else {
                printf("Timeout! Frame Number %lld Not Received\n", k);
                printf("Retransmitting Window...\n");
                break;
            }
        }

        i = i + z;
        printf("\n");
    }
    printf("Total frames sent and resent: %lld\n", tt);
}

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    WSAStartup(MAKEWORD(2, 2), &wsa);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);

    printf("Waiting for client connection...\n");
    client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    printf("Client connected.\n");

    ll tf, N;
    char buffer[1024];

    printf("Requesting frame count and window size from client...\n");
    send(client_socket, "SEND_FRAME_AND_WINDOW_SIZE", strlen("SEND_FRAME_AND_WINDOW_SIZE"), 0);
    recv(client_socket, buffer, sizeof(buffer), 0);
    sscanf(buffer, "%lld %lld", &tf, &N);
    printf("Total Frames: %lld, Window Size: %lld\n", tf, N);

    transmission(client_socket, tf, N);

    closesocket(client_socket);
    closesocket(server_fd);
    WSACleanup();

    return 0;
}

