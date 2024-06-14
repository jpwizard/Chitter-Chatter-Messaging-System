#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_PENDING 5
#define MAX_CHITTERS 3
//James Panagis
// server.c creates a server that listens for sent connections from clients (client.c)  and responds to there  message. It also keeps track of the number of chitters sent by each client and limits the number of chitters to a maximum of three per-session.
void record_chitter(char *chitter);

int main(int argc, char *argv[]) {

void record_chitter(char *chitter) {
    FILE *fp = fopen("chitters.txt", "a");
    if (fp == NULL) {
        perror("Error opening file");
        return;
    }

    time_t current_time;
    char* c_time_string;
    current_time = time(NULL);
    c_time_string = ctime(&current_time);
    c_time_string[strlen(c_time_string) - 1] = '\0';

    fprintf(fp, "%s: %s", c_time_string, chitter);
    fclose(fp);
}
    


	// checking server port number is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <Server port number>\n", argv[0]);
        exit(1);
    }

    // creating socket for the server
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Error creating socket");
        exit(1);
    }

    // configuring the server address
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(atoi(argv[1]));

    // binding socket to the server address
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        exit(1);
    }

    // listening for incoming connections
    if (listen(server_fd, MAX_PENDING) == -1) {
        perror("Error listening for incoming connections");
        exit(1);
    }

    while (1) {
        printf("Waiting for incoming connection...\n");

        // accepts sent connection
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("Error accepting incoming connection");
            continue;
        }

        printf("Incoming connection accepted\n");

        // sending greeting message to client
        char greeting_message[] = "Welcome to Chitter Server\n";
        if (send(client_fd, greeting_message, strlen(greeting_message), 0) == -1) {
            perror("Error sending greeting message to client");
            close(client_fd);
            continue;
        }

        int consecutive_chitters = 0;
        // reading Chitter messages from client and sending Chatter messages
        while (1) {
            // receive Chitter message from client
            char chitter[140];
            int bytes_received = recv(client_fd, chitter, sizeof(chitter), 0);
            if (bytes_received == -1) {
                perror("Error receiving Chitter from client");
                close(client_fd);
                break;
            }

            chitter[bytes_received] = '\0';

            // check for session termination
            if (strcasecmp(chitter, "bye\n") == 0) {
                printf("Client has terminated the session\n");
                close(client_fd);
                break;
            }

            // record Chitter in a text file
            record_chitter(chitter);

            // sending Chatter message to client
            char chatter[140];
            if (consecutive_chitters < MAX_CHITTERS) {
                sprintf(chatter, "Chatter: %s", chitter);
                consecutive_chitters++;
            } else {
                sprintf(chatter, "Chatter: You have reached the maximum number of consecutive Chitters\n");
                consecutive_chitters = 0;
            }

            if (send(client_fd, chatter, strlen(chatter), 0) == -1) {
                perror("Error sending Chatter to client");
                close(client_fd);
                break;
            }
        }
    }

    // close server socket
    close(server_fd);

    return 0;
}

