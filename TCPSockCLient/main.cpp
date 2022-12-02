#include<iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


void error(const char *msg)
{
    std::cerr<<msg;
    exit(0);
}

int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[256];

    if (argc < 3) {
       std::cerr<<"usage %s hostname port\n"<< argv[0];
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


    std::cout<<"Please enter the message: ";
    bzero(buffer,256);
    std::cin>>buffer;


    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0){ 
         error("ERROR writing to socket");}

    bzero(buffer,256);
    n = read(sockfd, buffer, 255);
    if (n < 0) {
         error("ERROR reading from socket");}


    std::cout<<buffer;
    bool RUNNING = true;
    while(RUNNING){
        char* buf = new char[256];
        std::cout<< "newmsg:";
        std::cin>>buf;
        int tmp = write(sockfd, buf, 256);
        std::cout<<buf<<" Has been written!"<<std::endl;
        
        RUNNING=false;
    }
    close(sockfd);
    return 0;
}
