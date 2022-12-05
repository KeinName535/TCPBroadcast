#include<iostream>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include<vector>
/// grundlage von bogotobogo kopiert

///@brief error function
///@param msg message to be displayed with error of type char*
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

/// @brief global Buffer
char* gBuff = new char[256];

/// @brief abstracts and filters the select function which doesn't return an array but an fd_set which needs some special handling to get ot in the form of a nice list, it also directly reads from the socket and puts the value in the global Buffer, this is needed because select also says an fd is ready to be read if there is less than 1 byte to be read
/// @param fdset the fdset which is supposed to be filtered
/// @param sockfdC  the fd for the socket from which the function is supposed to read
/// @return it resturns a vector<int> which includes the fds (file descriptor) which are suppose dto be read
std::vector<int> getReadQueue(fd_set *fdset, int sockfdC){
    bzero(gBuff, 256);

    std::vector<int> *queue = new std::vector<int>;
    select(sockfdC+1, fdset, nullptr,nullptr,nullptr);
    for (int i = 0; i < FD_SETSIZE-1; i++)
    {
        if(FD_ISSET(i, fdset)){
            int n = read(i, gBuff, 256)!=0;
            if(n!=0){
                queue->push_back(i);
                std::cout<<"connected fd ==>   "<<i<<std::endl;}
        }
    }
    return *queue;
    
}

/// @brief it is needed because we want to send the broadcasted to every connected socket, it only abstracts the annoying process to get a vector of the fds
/// @param fdset the fdset which includes every connected socket fd
/// @return it returns the fds which can receive messages in a vector<int>
std::vector<int> getWriteQueue(fd_set *fdset){

    std::vector<int> *queue = new std::vector<int>;
    for (int i = 0; i < FD_SETSIZE-1; i++)
    {
        if(FD_ISSET(i, fdset)){        
                queue->push_back(i);
        }
    }
    return *queue;
    
}


/// @brief the main function
/// @param argc the number of arguments given
/// @param argv the index[1] is supposed to get the port on which the server listens
/// @return returns 0 if everxthing worked
int main(int argc, char *argv[])
{
     std::cout<<"started Server"<<std::endl;
    
     /// some variables which are needed
     int sockfd, newsockfd, portno, n;
     fd_set main;
     socklen_t clilen;

     /// FD_ZERO sets the fds in main to zero
     FD_ZERO(&main);

     /// those structs will be filled with communication informations like the port and the ip of the server or a client connecting
     struct sockaddr_in serv_addr, cli_addr;
    
     if (argc < 2) {
         std::cerr<<"ERROR, no port provided"<<std::endl;
         exit(1);
     }

     /// create a socket
     /// socket(int domain, int type, int protocol) AF_INET is the address family, type is which socket we are using (in this case TCP), protocol is the ethernet protocol 0 is ip
    sockfd =  socket(AF_INET, SOCK_STREAM, 0);
    std::cout<<"socket created "<<sockfd<<std::endl;

     if (sockfd < 0) {
        error("ERROR opening socket");}

    

     /// clear address structure
     bzero((char *) &serv_addr, sizeof(serv_addr));

     /// defines port 
     portno = atoi(argv[1]);

     /// setup the host_addr structure for use in bind call 
     serv_addr.sin_family = AF_INET;  

     /// automatically be filled with current host's IP address
     serv_addr.sin_addr.s_addr = INADDR_ANY;  

     /// convert short integer value for port must be converted into network byte order
     serv_addr.sin_port = htons(portno);

     /// bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
     /// bind() passes file descriptor, the address structure, 
     /// and the length of the address structure
     /// This bind() call will bind  the socket to the current IP address on port, portno
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
    ///adds the fd of the new created socket to the main fd_set  
    FD_SET(sockfd, &main);

    bool RUNNING = true;


    /// This listen() call tells the socket to listen to the incoming connections.
    /// The listen() function places all incoming connection into a backlog queue
    /// until accept() call accepts the connection.
    /// Here, we set the maximum size for the backlog queue to 5.
    listen(sockfd, 4);

    /// needed for some function calls 
    int size = sockfd;

    /// the main loop 
    while(RUNNING){

        /// defines a temporary fd set to mess with (to filter via select) 
        fd_set tmp = main;

        
        /// read Queue saves fds ready to read
        std::vector<int> readQueue = getReadQueue(&tmp, size);

        /// write Queue saves fds ready to write
        std::vector<int> writeQueue = getWriteQueue(&main);

        clilen = sizeof(cli_addr);
        std::cout<<"ReadQueue: "<<readQueue.size()<<std::endl;
        std::cout<<"WriteQueue: "<<writeQueue.size()<<std::endl;


        for (int i = 0; i < readQueue.size(); i++)
        {
            
        
                if(readQueue[i]==sockfd){

                    std::cout<<readQueue[i]<<std::endl;
                    
                    /// This accept() function will write the connecting client's address info 
                    /// into the the address structure and the size of that structure is clilen.
                    /// The accept() returns a new socket file descriptor for the accepted connection.
                    /// So, the original socket file descriptor can continue to be used 
                    /// for accepting new connections while the new socker file descriptor is used for
                    /// communicating with the connected client.
                    newsockfd = accept(readQueue[i], (struct sockaddr *) &cli_addr, &clilen);
                    if (newsockfd < 0){ 
                        error("ERROR on accept");}
                    std::cout<<"connection on main socket"<<std::endl;
                    
                    /// sets the new socket to listening mode and adds it to the main fd_set 
                    listen(newsockfd, 1);
                    FD_SET(newsockfd, &main);

                    /// sets new size 
                    size = newsockfd;
                    std::cout<<"size: "<<size<<std::endl;
                }else{
                    std::cout<<"connection on socket " <<readQueue[i]<<std::endl;
                    
                    
                    std::cout<<"message:  "<<gBuff<<"  size:" <<strlen(gBuff)<<std::endl;


                    /// iterates through every writable socket and sends the received message if it isn't the socket which has just been read from 
                    ///or the main socket 
                    for (int j = 0; j < writeQueue.size(); j++)
                    {
                        if(!((writeQueue[j]==sockfd))){
                        
                            send(writeQueue[j], gBuff, strlen(gBuff), 0);
                        }
                    }
                    // ||(writeQueue[j]==readQueue[i])

                }

            
        }
        
    }
    for (int i = 0; i < FD_SETSIZE-1; i++)
        {
            close(i);
        }
     

     return 0; 
}