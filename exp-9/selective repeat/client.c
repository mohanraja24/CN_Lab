#include <bits/stdc++.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <ctime>

#define PORT 8080
#define ll long long int

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

int main() {
    WSADATA wsaData;
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup failed." << endl;
        return 1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cout << "Socket creation error" << endl;
        WSACleanup();
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (InetPton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address/ Address not supported" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    srand(time(NULL));

    // Wait for server request to send frame count and window size
    recv(sock, buffer, 1024, 0);
    if (strcmp(buffer, "SEND_FRAME_AND_WINDOW_SIZE") == 0) {
        ll tf, N;
        cout << "Enter total number of frames: ";
        cin >> tf;
        cout << "Enter window size: ";
        cin >> N;

        string msg = to_string(tf) + " " + to_string(N);
        send(sock, msg.c_str(), msg.size(), 0);
    }

    // Receive frames and manually send acknowledgments
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(sock, buffer, 1024, 0);

        if (valread > 0) {
            cout << buffer << endl;  // Display the received frame message
            
            // Prompt user for acknowledgment
            string ack;
            cout << "Enter ACK or NACK for the above frame: ";
            cin >> ack;
            send(sock, ack.c_str(), ack.size(), 0);
            Sleep(500); // slight delay
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
