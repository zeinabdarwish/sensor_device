#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>  // Including the necessary library for IP and socket operations

#define SERVER_IP "127.0.0.1"  // Server address (localhost in this case)
#define SERVER_PORT 8080       // Port the server is listening on

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[256];

    // Create a socket (TCP)
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }

    // Set up the server address structure
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    // Send a request to the server to get the temperature
    send(sockfd, "GET_TEMP", 8, 0);

    // Receive the temperature data from the server
    memset(buffer, 0, 256);  // Clear the buffer before receiving
    int n = recv(sockfd, buffer, 255, 0);
    if (n < 0) {
        perror("Error reading from socket");
    }

    // Print the temperature received from the server
    printf("Temperature from server: %s\n", buffer);

    // Close the socket connection
    close(sockfd);

    return 0;
}
