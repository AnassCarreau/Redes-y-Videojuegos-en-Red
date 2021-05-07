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
    hints.ai_flags = AI_PASSIVE;     //Devolver 0.0.0.0
    hints.ai_family = AF_INET;       /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */

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
                       // char buffer[80];

    struct sockaddr server;
    socklen_t serverlen = sizeof(struct sockaddr);

    while (true)
    {

        int server_sd = connect(sock, (struct sockaddr *)res->ai_addr, res->ai_addrlen);

        if (server_sd == -1)
        {
            return -1;
        }

        char buffer[80];
        do
        {
            /* code */

            std::cin >> buffer;
          if(!strcmp(buffer,"Q"))continue;
            send(server_sd, buffer, 1, 0);

            recv(server_sd, (void *)buffer, 1, 0);

            std::cout << buffer<<  std::endl;

        } while (strcmp(buffer,"Q"));
    }

    close(sock);

    return 0;
}
