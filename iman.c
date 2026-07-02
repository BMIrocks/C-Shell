#include "main.h"
#include "iman.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 4096
#define HOST "man.he.net"
#define PORT 80

void iman_command(char **args, int arg_count) {
    if (arg_count == 0 || args[0] == NULL) {
        printf("iMan: No command name provided\n");
        return;
    }
    
    char *command_name = args[0];
    
    // Create socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        return;
    }
    
    // Get host information
    struct hostent *server = gethostbyname(HOST);
    if (server == NULL) {
        printf("Error: Could not resolve hostname %s\n", HOST);
        close(sockfd);
        return;
    }
    
    // Set up server address structure
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    
    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting to server");
        close(sockfd);
        return;
    }
    
    // Prepare HTTP GET request
    char request[BUFFER_SIZE];
    snprintf(request, sizeof(request),
        "GET /?topic=%s&section=all HTTP/1.1\r\n"
        "Host: %s\r\n"
        "User-Agent: iMan/1.0\r\n"
        "Connection: close\r\n"
        "\r\n",
        command_name, HOST);
    
    // Send HTTP request
    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("Error sending request");
        close(sockfd);
        return;
    }
    
    // Collect entire response first
    char *full_response = malloc(1);
    full_response[0] = '\0';
    size_t total_length = 0;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    
    while ((bytes_received = recv(sockfd, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        
        full_response = realloc(full_response, total_length + bytes_received + 1);
        if (total_length == 0) {
            strcpy(full_response, buffer);
        } 
        else {
            strcat(full_response, buffer);
        }
        total_length += bytes_received;
    }
    
    if (bytes_received < 0) {
        perror("Error receiving data");
        free(full_response);
        close(sockfd);
        return;
    }
    
    // Find and skip HTTP headers
    char *body_start = strstr(full_response, "\r\n\r\n");
    if (body_start) {
        body_start += 4;
    } 
    else {
        // Try alternative separator
        body_start = strstr(full_response, "\n\n");
        if (body_start) {
            body_start += 2;
        } 
        else {
            body_start = full_response;
        }
    }
    
    // Extract content between <PRE> tags (where the actual man page is)
    char *pre_start = strstr(body_start, "<PRE>");
    char *pre_end = strstr(body_start, "</PRE>");
    
    if (pre_start && pre_end && pre_end > pre_start) {
        pre_start += 5; 
        
        *pre_end = '\0';
        
        char *current = pre_start;
        while (*current) {
            if (*current == '<') {
                // Skip HTML tag
                while (*current && *current != '>') {
                    current++;
                }
                if (*current == '>') current++;
            } 
            else if (strncmp(current, "&lt;", 4) == 0) {
                printf("<");
                current += 4;
            } 
            else if (strncmp(current, "&gt;", 4) == 0) {
                printf(">");
                current += 4;
            } 
            else if (strncmp(current, "&amp;", 5) == 0) {
                printf("&");
                current += 5;
            } 
            else {
                printf("%c", *current);
                current++;
            }
        }
        printf("\n"); 
    } 
    else {
        char *current = body_start;
        while (*current) {
            if (*current == '<') {
                // Skip HTML tag
                while (*current && *current != '>') {
                    current++;
                }
                if (*current == '>') current++;
            } 
            else if (strncmp(current, "&lt;", 4) == 0) {
                printf("<");
                current += 4;
            } 
            else if (strncmp(current, "&gt;", 4) == 0) {
                printf(">");
                current += 4;
            } 
            else if (strncmp(current, "&amp;", 5) == 0) {
                printf("&");
                current += 5;
            } 
            else {
                printf("%c", *current);
                current++;
            }
        }
        printf("\n");
    }
    
    free(full_response);
    close(sockfd);
}
