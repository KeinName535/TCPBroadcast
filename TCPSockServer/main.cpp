#include<iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include<vector>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
char* gBuff = new char[256];


std::vector<int> getReadQueue(fd_set *fdset, int sockfdC){
    bzero(gBuff, 256);

    std::vector<int> *queue = new std::vector<int>;
    select(sockfdC+1, fdset, nullptr,nullptr,nullptr);
    for (int i = 0; i < FD_SETSIZE-1; i++)
    {
        if(FD_ISSET(i, fdset)){
            int n = read(i, gBuff, 256)!=0;
            // std::cout<<"messagesize: "<<n;
            if(n!=0){
                queue->push_back(i);
                std::cout<<"connected fd ==>   "<<i<<std::endl;}
        }
    }
    return *queue;
    
}
std::vector<int> getWriteQueue(fd_set *fdset, int sockfdC){

    std::vector<int> *queue = new std::vector<int>;
    for (int i = 0; i < FD_SETSIZE-1; i++)
    {
        if(FD_ISSET(i, fdset)){        
                queue->push_back(i);
                std::cout<<"connected fd ==>   "<<i<<std::endl;
        }
    }
    return *queue;
    
}

int main(int argc, char *argv[])
{
    std::cout<<"started Server"<<std::endl;
   
     int sockfd, newsockfd, portno;
     fd_set main;
     socklen_t clilen;
     FD_ZERO(&main);
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         std::cerr<<"ERROR, no port provided"<<std::endl;
         exit(1);
     }

     // create a socket
     // socket(int domain, int type, int protocol)
     sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    std::cout<<"socket created "<<sockfd<<std::endl;

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

    FD_SET(sockfd, &main);

    bool RUNNING = true;


    // This listen() call tells the socket to listen to the incoming connections.
        // The listen() function places all incoming connection into a backlog queue
        // until accept() call accepts the connection.
        // Here, we set the maximum size for the backlog queue to 5.
    listen(sockfd, 4);

    int size = sockfd;
    while(RUNNING){
        fd_set tmp = main;

        
        
        std::vector<int> readQueue = getReadQueue(&tmp, size);
        std::vector<int> writeQueue = getWriteQueue(&main, size);

        clilen = sizeof(cli_addr);
        std::cout<<"ReadQueue: "<<readQueue.size()<<std::endl;
        std::cout<<"WriteQueue: "<<writeQueue.size()<<std::endl;


        for (int i = 0; i < readQueue.size(); i++)
        {
            
        
                if(readQueue[i]==sockfd){

                    std::cout<<readQueue[i]<<std::endl;
                    // This accept() function will write the connecting client's address info 
                    // into the the address structure and the size of that structure is clilen.
                    // The accept() returns a new socket file descriptor for the accepted connection.
                    // So, the original socket file descriptor can continue to be used 
                    // for accepting new connections while the new socker file descriptor is used for
                    // communicating with the connected client.
                    newsockfd = accept(readQueue[i], (struct sockaddr *) &cli_addr, &clilen);
                    if (newsockfd < 0){ 
                        error("ERROR on accept");}
                    std::cout<<"connection on main socket"<<std::endl;

                    listen(newsockfd, 1);
                    FD_SET(newsockfd, &main);

                    // This send() function sends the 13 bytes of the string to the new socket
                    n = read(newsockfd,gBuff,255);
                    if (n <= 0){ 
                         error("ERROR reading from socket");}

                    std::cout<<"Here is the message:"<<gBuff<<"   on:"<<newsockfd<<std::endl;

                    size = newsockfd;
                    std::cout<<"size: "<<size<<std::endl;
                }else{
                    std::cout<<"other socket ==> " <<readQueue[i]<<std::endl;
                    // n = read(queue[i],gBuff,256);
                    // if (n < 0){ 
                    //     error("ERROR reading from socket");}
                    
                    std::cout<<"message:  "<<gBuff<<"  size:" <<strlen(gBuff)<<std::endl;

                    for (int j = 0; j < writeQueue.size(); j++)
                    {
                        if(!((writeQueue[j]==sockfd)||(writeQueue[j]==readQueue[i]))){
                            std::cout<<"sending stuff"<<std::endl;
                            send(writeQueue[j], gBuff, strlen(gBuff), 0);
                        }
                        std::cout<<"in for loop"<<std::endl;
                    }
                    

                }

            
        }
        
    }
    for (int i = 0; i < FD_SETSIZE-1; i++)
        {
            close(i);
        }
     

     return 0; 
}