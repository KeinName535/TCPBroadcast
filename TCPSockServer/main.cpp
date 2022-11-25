#include<iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    std::cout<<"started Server"<<std::endl;
   
     int sockfd, newsockfd, portno;
     fd_set main;
     socklen_t clilen;
     FD_ZERO(&main);
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         std::cerr<<"ERROR, no port provided"<<std::endl;
         exit(1);
     }

     // create a socket
     // socket(int domain, int type, int protocol)
     sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    std::cout<<"socket created"<<sockfd<<std::endl;

     if (sockfd < 0) {
        error("ERROR opening socket");}

    

     // clear address structure
     bzero((char *) &serv_addr, sizeof(serv_addr));

     portno = atoi(argv[1]);

     /* setup the host_addr structure for use in bind call */
     // server byte order
     serv_addr.sin_family = AF_INET;  

     // automatically be filled with current host's IP address
     serv_addr.sin_addr.s_addr = INADDR_ANY;  

     // convert short integer value for port must be converted into network byte order
     serv_addr.sin_port = htons(portno);

     // bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
     // bind() passes file descriptor, the address structure, 
     // and the length of the address structure
     // This bind() call will bind  the socket to the current IP address on port, portno
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
    std::cout<<"61"<<std::endl;

    FD_SET(sockfd, &main);

    bool RUNNING = true;


    // This listen() call tells the socket to listen to the incoming connections.
        // The listen() function places all incoming connection into a backlog queue
        // until accept() call accepts the connection.
        // Here, we set the maximum size for the backlog queue to 5.
    listen(sockfd, 4);
    std::cout<<"listening"<<std::endl;

    while(RUNNING){
        fd_set tmp = main;
        std::cout<<"tmp_set"<<std::endl;
        
        int count = select(1, &tmp, nullptr,nullptr,nullptr);
        std::cout<<"be for "<<std::endl;

        clilen = sizeof(cli_addr);

        for (int i = 0; i < FD_SETSIZE-1; i++)
        {
            std::cout<<"iterating through fds"<<i<<std::endl;

            if(FD_ISSET(i, &tmp)){
                if(i==0){
                    // This accept() function will write the connecting client's address info 
                    // into the the address structure and the size of that structure is clilen.
                    // The accept() returns a new socket file descriptor for the accepted connection.
                    // So, the original socket file descriptor can continue to be used 
                    // for accepting new connections while the new socker file descriptor is used for
                    // communicating with the connected client.
                    newsockfd = accept(i, (struct sockaddr *) &cli_addr, &clilen);
                    if (newsockfd < 0){ 
                        error("ERROR on accept");}
                        printf("server: got connection from %s port %d\n");
                    FD_SET(newsockfd, &main);

                    listen(newsockfd, 1);
                    // This send() function sends the 13 bytes of the string to the new socket
                    send(newsockfd, "Hello, world!\n", 13, 0);
                }else{
                    bzero(buffer,256);

                    n = read(i,buffer,255);
                    if (n < 0){ 
                        error("ERROR reading from socket");}

                    std::cout<<"Here is the message: %s\n"<<buffer;

                }

            }
        }
        
        // inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port)
        for (int i = 0; i < FD_SETSIZE-1; i++)
        {
            close(i);
        }
        
        
        RUNNING=false;
    }
     close(newsockfd);
     close(sockfd);
     return 0; 
}