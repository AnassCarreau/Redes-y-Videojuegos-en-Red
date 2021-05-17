#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <time.h>

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *res;

    /* Obtain address(es) matching host/port */

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;      /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (rc != 0)
    {
        std::cerr << "Error: " << gai_strerror(rc) << "\n";
        return -1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sock == -1)
    {
        std::cerr << "[socket]\n";
        return -1;
    }

    freeaddrinfo(res); /* No longer needed */
    char buffer[80];

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr server;
    socklen_t serverlen = sizeof(struct sockaddr);
    bool exit = false;

    sendto(sock, argv[3], strlen(argv[3]) + 1, 0, res->ai_addr, res->ai_addrlen);

    int bytes = recvfrom(sock, (void *)buffer, 79 * sizeof(char), 0,res->ai_addr,&res->ai_addrlen);

    if (bytes == -1)
    {
        return -1;
    }

    std::cout << buffer << std::endl;

    //}

       close(sock);


    return 0;
}
