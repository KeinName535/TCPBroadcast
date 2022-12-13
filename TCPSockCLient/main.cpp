#include<iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include<thread>
#include<vector>
#include<fstream>
/// grundlage von bogotobogo kopiert


///@brief error function
///@param msg message to be displayed with error of type char*
void error(const char *msg)
{
    std::cerr<<msg;
    exit(0);
}


/// @brief global buffers for the input and the output
char inBuff[256];
char outBuff[256];

/// @brief function of the reading thread, it reads reccurently reads the data from the screen and either simply puts it in the ostream to be sdisplayed via cout or it dislplays it on another terminal depending on whether i am able to sort out the clusterfuck of code which is needed to add a new terminal for the thread 
/// @param socket the socket which to read from
/// @param data the file to which to append the data to be displayed via a system call that opens a gnome-terminal and executes via bash -c tails -f on the file
void inStream(int socket, std::ofstream* data){
    while(true){

        int n = read(socket, inBuff, 255);
        if (n > 0) {       
            *data<<inBuff; 
            std::cout<<"writting stuff"<<std::endl;
            std::cout<<inBuff;
            bzero(inBuff, 256);

            }
    }
}



/// @brief the main function
/// @param argc the number of arguments given
/// @param argv argv[1] is supposed to get the ip of the server to which the client needs to connect and argv[2] it's port
/// @return returns 0 if everxthing worked
int main(int argc, char *argv[])
{

    /// the file which is supposed to be read by the new terminal
    std::ofstream file;
    file.open("data.file", std::ios::app | std::ios::out |std::ios::in);
    file<<"hi";

    /// variables which need to be defined
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    /// sets the in and out buffer to all zeros
    bzero(inBuff,256);
    bzero(outBuff,256);


    if (argc < 3) {
       /// argv[0] returns the current working directory
       std::cerr<<"usage "<<argv[0]<< "hostname port\n";
       exit(0);
    }

    /// converts char[] port number to int 
    portno = atoi(argv[2]);

    /// create a socket
    /// socket(int domain, int type, int protocol) AF_INET is the address family, type is which socket we are using (in this case TCP), protocol is the ethernet protocol 0 is ip
    sockfd = socket(AF_INET, SOCK_STREAM, 0);


    if (sockfd < 0){ 
        error("ERROR opening socket");
        }
    /// looks up the which to connect to
    server = gethostbyname(argv[1]);
    if (server == NULL) {
        std::cerr<<"ERROR, no such host\n";
        exit(0);
    }
    /// zeros out the server_addr structure
    bzero((char *) &serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    /// copys the host address from the server hostent object to the address structure 
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);

    /// puts the port in the address structure in the apropriate network bit order
    serv_addr.sin_port = htons(portno);

    /// connects the socket with the server using the information of the serv_addr structure defined above 
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");}

    /// creates thread used to read  and detaches ir from the main thread
    std::thread input(inStream, sockfd, &file);
    input.detach();

    ///makes a system call which creates a new gnome-terminal which takes the file created above and outputs it's newest value using tail -f
    //system("gnome-terminal -- \"bash -c 'tail -f ./log.file'\"");

    /// main loop which reads messages and send them to the server
    bool RUNNING = true;
    while(RUNNING){
        
        std::cout<<"Please enter the message: ";
        bzero(outBuff,256);
        std::cin>>outBuff; 
        n = write(sockfd, outBuff, strlen(outBuff));
        if (n < 0){ 
            error("ERROR writing to socket");}
        
        }
    
    /// closes the socket and the fd to leave no dangling fds which could get exploited and use up memory
    close(sockfd);
    file.close();
    return 0;
}
