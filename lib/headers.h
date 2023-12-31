#include <fcntl.h>
#include <sys/socket.h>

#define MAXLINE 1024
#define LISTENQ 10
#define LOCALHOST "127.0.0.1"

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

void err_quit(char *msg, ...);
void err_sys(char *msg, ...);
void println(char *msg, ...);

int Socket(int domain, int type, int protocol);
int Bind(int fd, const struct sockaddr *addr, socklen_t len);

char *Sock_ntop(const struct sockaddr *sa, socklen_t salen);

/* Read "n" bytes from a descriptor. */
ssize_t Readn(int fd, void *ptr, size_t nbytes);

/* Write "n" bytes to a descriptor. */
void Writen(int fd, void *ptr, size_t nbytes);

/* Following shortens all the typecasts of pointer arguments: */
#define SA struct sockaddr
