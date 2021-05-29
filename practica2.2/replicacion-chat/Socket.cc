#include <string.h>

#include "Serializable.h"
#include "Socket.h"

Socket::Socket(const char *address, const char *port) : sd(-1)
{
    //Construir un socket de tipo AF_INET y SOCK_DGRAM usando getaddrinfo.

    struct addrinfo hints;
    struct addrinfo *res;

    /* Obtain address(es) matching host/port */

    memset((void *)&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;      // Allow IPv4
    hints.ai_socktype = SOCK_DGRAM; // Datagram socket
    hints.ai_protocol = 0;          // protocol

    int rc = getaddrinfo(address, port, &hints, &res);

    if (rc != 0)
    {
        std::cerr << "Error: " << gai_strerror(rc) << "\n";
    }

    sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    if (sd == -1)
    {
        std::cerr << "[socket]\n";
    }
    //Con el resultado inicializar los miembros sd, sa y sa_len de la clase
    sa = *res->ai_addr;
    sa_len = res->ai_addrlen;
    freeaddrinfo(res); /* No longer needed */
}

int Socket::recv(Serializable &obj, Socket *&sock)
{
    struct sockaddr sa;
    socklen_t sa_len = sizeof(struct sockaddr);

    char buffer[MAX_MESSAGE_SIZE];

    ssize_t bytes = recvfrom(sd, buffer, MAX_MESSAGE_SIZE, 0, &sa, &sa_len);

    if (bytes <= 0)
    {
        return -1;
    }

    if (sock != 0)
    {
        sock = new Socket(&sa, sa_len);
    }

    obj.from_bin(buffer);

    return 0;
}

int Socket::send(Serializable &obj, const Socket &sock)
{
    //Serializar el objeto
    obj.to_bin();
    //Enviar el objeto binario a sock usando el socket sd
    char buffer[MAX_MESSAGE_SIZE];
    ssize_t bytes = sendto(sock.sd, obj.data(), obj.size(), 0, &sock.sa, sock.sa_len);

    if (bytes <= 0)
    {
        return -1;
    }
    return 0;
}

bool operator==(const Socket &s1, const Socket &s2)
{
    //Comparar los campos sin_family, sin_addr.s_addr y sin_port
    //de la estructura sockaddr_in de los Sockets s1 y s2
    if (((struct sockaddr_in *)(&s1.sa))->sin_family != ((struct sockaddr_in *)(&s2.sa))->sin_family)
        return false;
    if (((struct sockaddr_in *)(&s1.sa))->sin_addr.s_addr != ((struct sockaddr_in *)(&s2.sa))->sin_addr.s_addr)
        return false;
    if (((struct sockaddr_in *)(&s1.sa))->sin_port != ((struct sockaddr_in *)(&s2.sa))->sin_port)
        return false;

    //Retornar false si alguno difiere
    return true;
};

std::ostream &operator<<(std::ostream &os, const Socket &s)
{
    char host[NI_MAXHOST];
    char serv[NI_MAXSERV];

    getnameinfo((struct sockaddr *)&(s.sa), s.sa_len, host, NI_MAXHOST, serv,
                NI_MAXSERV, NI_NUMERICHOST);

    os << host << ":" << serv;

    return os;
};
