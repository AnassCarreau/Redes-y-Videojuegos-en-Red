#include <netdb.h>
#include <string.h>
#include <iostream> 
//Main del ejercicio
 int main(int argc, char *argv[])
       {
           //filtro y resultado
           struct addrinfo hints;
           struct addrinfo *result;

           /* Obtain address(es) matching host/port */
 
           memset(&hints, 0, sizeof(struct addrinfo));

          hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
        // hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
        // hints.ai_flags = 0;
        // hints.ai_protocol = 0;          / Any protocol /

         int  rc = getaddrinfo(argv[1], argv[2], &hints, &result);
         
           if (rc != 0) {
               fprintf(stderr, "Error: %s\n", gai_strerror(rc));
               exit(EXIT_FAILURE);
           }

           for (auto i = result; i != NULL; i = i->ai_next) {

               char host[NI_MAXHOST];
               char serv[NI_MAXSERV];

               getnameinfo(i->ai_addr,i->ai_addrlen,host,NI_MAXHOST,serv
               ,NI_MAXSERV,NI_NUMERICHOST);

               std::cout <<host <<" " << i->ai_family <<" "<<i->ai_socktype <<std::endl;

           } 

           freeaddrinfo(result);           /* No longer needed */

          return 0; 
    }