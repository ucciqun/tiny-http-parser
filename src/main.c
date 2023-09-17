#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define QUEUE_SIZE 3

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    socklen_t addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0) {
        perror("Failed to create socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, addrlen) < 0) {
        perror("Failed to bind socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, QUEUE_SIZE) < 0) {
        perror("Failed to listen on socket");
        exit(EXIT_FAILURE);
    }

    printf("[+] Listening on port %d\n", PORT);

    if ((new_socket =
             accept(server_fd, (struct sockaddr *)&address, &addrlen)) < 0) {
        perror("Failed to accept connection");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_read;
    while ((bytes_read = recv(new_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        buffer[bytes_read] = '\0';

        // parse request and print header
        char *start_line, *header_line, *message_body;

        start_line = strtok(buffer, "\r\n");
        if (start_line == NULL) {
            printf("start_line is NULL\n");
            exit(EXIT_FAILURE);
        }
        printf("start_line: %s\n", start_line);

        header_line = strtok(NULL, "\r\n");
        while (header_line != NULL && strlen(header_line) > 0) {
            printf("Header Field: %s\n", header_line);
            header_line = strtok(NULL, "\r\n");
        }

        message_body = strtok(NULL, "\r\n\r\n");
        if (message_body != NULL)
            printf("Message Body: %s\n", message_body);
        else
            printf("Message Body: NULL\n");

        memset(buffer, 0, BUFFER_SIZE);
    }

    if (bytes_read < 0) {
        perror("Failed to receive data");
    }

    close(new_socket);
    close(server_fd);
    return 0;
}