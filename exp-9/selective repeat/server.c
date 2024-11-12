#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define ll long long int

void sendFrame(int client_socket, ll frameNumber) {
    char msg[1024];
    sprintf(msg, "Sending Frame %lld", frameNumber);
    send(client_socket, msg, strlen(msg), 0);
    printf("%s\n", msg);
}

void selectiveRepeatTransmission(int client_socket, ll tf, ll N) {
    ll tt = 0;
    int ackMap[tf+1];  // Track acknowledgments for each frame
    memset(ackMap, 0, sizeof(ackMap));  // Initialize all to 0 (not acknowledged)
    ll i = 1;

    while (i <= tf) {
        for (ll k = i; k < i + N && k <= tf; ++k) {
            if (!ackMap[k]) {  // Send only if not acknowledged
                sendFrame(client_socket, k);
                tt++;
                Sleep(500); // slight delay between frames (in milliseconds)
            }
        }

        // Checking acknowledgments for frames in the window
        for (ll k = i; k < i + N && k <= tf; ++k) {
            if (ackMap[k]) continue;  // Skip already acknowledged frames

            char buffer[1024] = {0};
            fd_set readfds;
            struct timeval tv;
            FD_ZERO(&readfds);
            FD_SET(client_socket, &readfds);
            tv.tv_sec = 8;  // Timeout for selective repeat ARQ
            tv.tv_usec = 0;

            int activity = select(0, &readfds, NULL, NULL, &tv);

            if (activity > 0) {
                int valread = recv(client_socket, buffer, 1024, 0);
                if (valread > 0 && strcmp(buffer, "ACK") == 0) {
                    printf("Acknowledgment received for Frame %lld\n", k);
                    ackMap[k] = 1;
                } else {
                    printf("NACK received or no acknowledgment for Frame %lld\n", k);
                }
            } else {
                printf("Timeout! No acknowledgment for Frame %lld\n", k);
            }
        }

        // Move window to the next unacknowledged frame
        while (i <= tf && ackMap[i]) {
            i++;
        }

        printf("\n");
    }

    printf("Total frames sent and resent: %lld\n", tt);
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return -1;
    }

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Socket creation failed");
        return -1;
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (const char *)&opt, sizeof(opt)) < 0) {
        perror("Setsockopt failed");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Listen for client connections
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        return -1;
    }

    printf("Waiting for client connection...\n");
    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
        perror("Accept failed");
        return -1;
    }

    printf("Client connected.\n");

    // Start transmission using selective repeat
    ll tf, N;
    char buffer[1024] = {0};

    printf("Requesting frame count and window size from client...\n");
    send(client_socket, "SEND_FRAME_AND_WINDOW_SIZE", 26, 0);
    recv(client_socket, buffer, 1024, 0);
    sscanf(buffer, "%lld %lld", &tf, &N);
    printf("Total Frames: %lld, Window Size: %lld\n", tf, N);

    // Start selective repeat transmission
    selectiveRepeatTransmission(client_socket, tf, N);

    // Clean up and close sockets
    closesocket(client_socket);
    closesocket(server_fd);
    WSACleanup();

    return 0;
}

