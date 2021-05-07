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

    if (bind(sock, res->ai_addr, res->ai_addrlen) == -1)
    {
        std::cerr << "[bind]\n";
        return -1;
    }

    listen(sock, 5);

    freeaddrinfo(res); /* No longer needed */
    char buffer[80];

    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    struct sockaddr client;
    socklen_t clientlen = sizeof(struct sockaddr);

    while (true)
    {

        int cliente_sd = accept(sock, (struct sockaddr *)&client, &clientlen);

        if (clientlen == -1)
        {
            return -1;
        }

        getnameinfo(&client, clientlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Conexion desde " << host << " " << serv << std::endl;

        int c, i;
        do
        {
            c = recv(cliente_sd, &(buffer[i]), 1, 0);
            send(cliente_sd, buffer, i, 0);
        } while (c >= 0 && i < 79 && buffer[i++] != '\n');

        std::cout << "Conexion terminada " << std::endl;
    }

    close(sock);

    return 0;
}
