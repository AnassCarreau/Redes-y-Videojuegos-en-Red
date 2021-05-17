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

   if (bind(sock, res->ai_addr, res->ai_addrlen) == -1)
   {
      std::cerr << "[bind]\n";
      return -1;
   }

   freeaddrinfo(res); /* No longer needed */
   char buffer[80];

   char host[NI_MAXHOST];
   char serv[NI_MAXSERV];

   struct sockaddr client;
   socklen_t clientlen = sizeof(struct sockaddr);
   bool exit = false;

   time_t time_;
   struct tm *tm_;
   while (!exit)
   {

      int bytes = recvfrom(sock, (void *)buffer, 79 * sizeof(char), 0, &client, &clientlen);

      if (bytes == -1)
      {
         return -1;
      }

      getnameinfo(&client, clientlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

      std::cout << bytes << " bytes de " << host << ":" << serv << std::endl;
     
   
      switch (buffer[0])
      {
      case 't':
         time(&time_);
         tm_ = localtime(&time_);
         bytes = strftime(buffer, 80, "%T %p", tm_);
         sendto(sock, buffer, bytes, 0, &client, clientlen);
         break;
      case 'd':
         time(&time_);
         tm_ = localtime(&time_);
         bytes = strftime(buffer, 80, "%F", tm_);
         sendto(sock, buffer, bytes, 0, &client, clientlen);
         break;
      case 'q':
         exit = true;
         std::cout << "Saliendo..." << std::endl;
         break;
      default:
         std::cout << "Comando no soportado " << buffer[0] << std::endl;
         break;
      }
   }

   close(sock);

   return 0;
}
