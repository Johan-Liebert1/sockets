#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define read_end 0
#define write_end 1

void client(int *client_pipe, int *server_pipe) {
    // close read end of client pipe as we are only going to send stuff on this
    // pipe
    close(client_pipe[write_end]);

    char file_name[256];

    if (fgets(file_name, 256, stdin) == NULL) {
        printf("fgets error\n");
        exit(1);
    }

    write(server_pipe[write_end], file_name, strlen(file_name));

    int n = read(client_pipe[read_end], file_name, 256);
    file_name[n] = '\0';
    printf("Client: Read '%d' bytes and '%s' from pipe\n", n, file_name);
}

void server(int *client_pipe, int *server_pipe) {
    // close read end of client pipe as we are only going to send stuff on this
    // pipe
    close(server_pipe[write_end]);

    char file_name[256];
    int n = read(server_pipe[read_end], file_name, 256);

    // get rid of the new line
    file_name[n - 1] = '\0';

    printf("Server: Read '%d' bytes and '%s' from pipe\n", n, file_name);

    int fd = open(file_name, O_RDONLY);

    if (fd < 0) {
        char *msg = "Failed to open file";
        write(client_pipe[write_end], msg, strlen(msg));
        exit(1);
    }

    char file_contents[256];
    n = read(fd, file_contents, 256);

    if (n < 0) {
        char *msg = "Failed to read from file";
        write(client_pipe[write_end], msg, strlen(msg));
        exit(1);
    }

    write(client_pipe[write_end], file_contents, strlen(file_contents));
}

int main() {
    int server_pipe[2], client_pipe[2];

    if (pipe(server_pipe) < 0) {
        perror("server_pipe");
    }

    if (pipe(client_pipe) < 0) {
        perror("client_pipe");
    }

    pid_t child_pid = fork();

    if (child_pid == 0) {
        client(client_pipe, server_pipe);
    } else {
        server(client_pipe, server_pipe);
    }

    return 0;
}
