#include "../lib/headers.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

int main() {
    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    char sendline[2000];
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(5000);
    inet_pton(AF_INET, LOCALHOST, &server_addr.sin_addr);

    for (int i = 0; i < 1400; i++) {
        sendto(sockfd, sendline, 2000, 0, (struct sockaddr*)&server_addr, sizeof(server_addr));
    }
}
