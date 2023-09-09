#include <sys/socket.h>

#define MAXLINE 1024
#define LISTENQ 1024

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
