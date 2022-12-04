#include<iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<thread>
#include<vector>

void error(const char *msg)
{
    std::cerr<<msg;
    exit(0);
}

char inBuff[256];
char outBuff[256];

std::vector<std::string> *inQueue = new std::vector<std::string>;

void inStream(int socket){
    while(true){
        int n = 0;
        n = read(socket, inBuff, 255);
        if (n > 0) {
        
            inQueue->push_back(inBuff);
            std::cout<<inBuff;
            }
    }
}




int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    
    bzero(inBuff,256);
    bzero(outBuff,256);


    if (argc < 3) {
       std::cerr<<"usage "<<argv[0]<< "hostname port\n";
       exit(0);
    }


    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);


    if (sockfd < 0){ 
        error("ERROR opening socket");
        }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        std::cerr<<"ERROR, no such host\n";
        exit(0);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);


    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");}


    bool RUNNING = true;
    while(RUNNING){
        std::thread input(inStream, sockfd);
        input.detach();
        std::cout<<"Please enter the message: ";
        bzero(outBuff,256);
        std::cin>>outBuff; 
        n = write(sockfd, outBuff, strlen(outBuff));
        if (n < 0){ 
            error("ERROR writing to socket");}
        
        }
    close(sockfd);
    return 0;
}
