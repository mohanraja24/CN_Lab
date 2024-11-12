#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsa;
    SOCKET sockfd;
    struct sockaddr_in server_addr;
    int number;
    long result;
    int addr_len = sizeof(server_addr);

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed. Error Code: %d\n", WSAGetLastError());
        return -1;
    }

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));

    // Fill server information
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Input number from user
    printf("Enter a number to calculate factorial: ");
    scanf("%d", &number);

    // Send number to server
    sendto(sockfd, (const char *)&number, sizeof(number), 0, (const struct sockaddr *)&server_addr, addr_len);

    // Receive result from server
    recvfrom(sockfd, (char *)&result, sizeof(result), 0, (struct sockaddr *)&server_addr, &addr_len);
    printf("Factorial of %d is: %ld\n", number, result);

    closesocket(sockfd);
    WSACleanup();
    return 0;
}
