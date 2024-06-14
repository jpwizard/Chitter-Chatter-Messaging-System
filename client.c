#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_CHITTERS 3
//James Panagis
//client.c connection to a server(server.c) by ip address and port number, and allows the user to send and receive Chitters with a limit of three per session. client.c also has  basic input validation and prompts the user to confirm the sending of each Chitter.
int main(int argc, char *argv[]) {
    // checking server ip address and port number are provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <Server IP address> <Server port number>\n", argv[0]);
        exit(1);
    }

    // creating socket for the client
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // configuring the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid server IP address");
        exit(1);
    }

    // connecting server
    if (connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        exit(1);
    }

    // sending Please tell us your Chitter? to the server
    char intro_message[] = "Please tell us your Chitter?";
    if (send(client_fd, intro_message, strlen(intro_message), 0) == -1) {
        perror("Error sending intro message to server");
        exit(1);
    }

    int num_chitters_sent = 0;
    char buffer[140];
    while (1) {
        printf("Enter your Chitter (140 characters or less): ");
        fgets(buffer, sizeof(buffer), stdin);
        int bytes_sent = send(client_fd, buffer, strlen(buffer), 0);
        if (bytes_sent == -1) {
            perror("Error sending Chitter to server");
            exit(1);
        }

        // receiving Chatter message from server and printing to console
        char chatter[140];
        int bytes_received = recv(client_fd, chatter, sizeof(chatter), 0);
        if (bytes_received == -1) {
            perror("Error receiving Chatter from server");
            exit(1);
        }
        printf("Chatter: %.*s\n", bytes_received, chatter);

        // ask user if they want to send the Chitter again
        printf("Do you REALLY want to send that Chitter? (yes/no): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; 
        if (strcmp(buffer, "yes") == 0) {
            num_chitters_sent++;
            if (num_chitters_sent >= MAX_CHITTERS) {
                printf("You have reached the maximum number of Chitters for this session.\n");
                break;
            }
            continue;
        } else if (strcmp(buffer, "no") == 0) {
            continue;
        } else if (strcmp(buffer, "bye") == 0) {
            break;
        } else {
            printf("Invalid input. Please try again.\n");
            continue;
        }
    }

    // closing client socket
    close(client_fd);

    return 0;
}

