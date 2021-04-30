#include <netdb.h>
#include <string.h>
#include <iostream> 
#include <unistd.h>

 int main(int argc, char *argv[])
       {
           struct addrinfo hints;
           struct addrinfo *result;

           /* Obtain address(es) matching host/port */
 
           memset(&hints, 0, sizeof(struct addrinfo));

          hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
          hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
        // hints.ai_flags = 0;
        // hints.ai_protocol = 0;          / Any protocol /

         int  rc = getaddrinfo(argv[1], argv[2], &hints, &result);
         
           if (rc != 0) {
               fprintf(stderr, "Error: %s\n", gai_strerror(rc));
              return -1;
           }

            int sock=socket(result->ai_family,result->ai_socktype,result->ai_protocol);

        if (sock == -1) 
           {
               std::cerr << "[socket]:creacion socket\n";
              return -1;
           }
            bind(sock,result->ai_addr,result->ai_addrlen);

           freeaddrinfo(result);           /* No longer needed */
        while (true)
        {
            char buffer[80];
            struct sockaddr  client;
            socklen_t        clientlen =sizeof(struct sockaddr);
               char host[NI_MAXHOST];
               char serv[NI_MAXSERV];

           int bytes = recvfrom(sock,(void *)buffer,79,0,&client,&clientlen);

           buffer[bytes]='\0';
        
           if (bytes == -1) 
           {
              return -1;
           }  

               getnameinfo(&client,clientlen,host,NI_MAXHOST,serv
               ,NI_MAXSERV,NI_NUMERICHOST);

            std::cout <<"Host :" << host <<"Port: "<<serv <<std::endl;
            std::cout <<"\tData :" <<buffer <<std::endl;
            int send =sendto(sock,(void *)buffer,bytes,0,&client,clientlen);
        if ( send == -1) 
           {
              return -1;
           }  

        }
        
          close(sock);

          return 0; 
    }