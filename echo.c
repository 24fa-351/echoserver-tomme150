#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#define PORT 8080

void handleConnection(int socket_fd) {
    char buffer[1024];
    int bytes_read;
    while ((bytes_read = recv(socket_fd, buffer, 1024, 0)) > 0) {
        buffer[bytes_read] = '\0'; // Null-terminate the received data
        printf("Received: %s\n", buffer);
        send(socket_fd, buffer, bytes_read, 0);
    }
    closesocket(socket_fd);
}

int main(int argc, char* argv[]) {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        closesocket(socket_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(socket_fd, 10) < 0) {
        perror("Listen failed");
        closesocket(socket_fd);
        exit(EXIT_FAILURE);
    }

    printf("Echo server is listening on port %d\n", PORT);

    while (1) {
        struct sockaddr_in new_socket_address;
        socklen_t new_socket_address_len = sizeof(new_socket_address);
        int new_socket_fd = accept(socket_fd, (struct sockaddr*)&new_socket_address, &new_socket_address_len);
        if (new_socket_fd < 0) {
            perror("Accept failed");
            continue;
        }
        handleConnection(new_socket_fd);
    }

#ifdef _WIN32
    WSACleanup();
#endif

    return 0;
}