/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <regex.h>
#include "packet.h"
#include <stdbool.h>
#include <time.h>

void convertStringToPacket(char* buf, Packet *pack);

double uniform_rand(){
    return (double)rand() / (double)RAND_MAX;
}


int main(int argc, char** argv){
    
    
    
    //printf("Hello world from server\n");
    srand(time(NULL));
    
    // create a socket
    int server_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    
    
    // bind to specific add/port number- https://man7.org/linux/man-pages/man2/bind.2.html
    struct sockaddr_in sockAdd;
    sockAdd.sin_family = AF_INET;
    sockAdd.sin_addr.s_addr = htonl(INADDR_ANY);
    //inet_pton(AF_INET, INADDR_ANY, &sockAdd.sin_addr);
    int portNumber = atoi(argv[1]);
    sockAdd.sin_port = htons(portNumber);
    memset(sockAdd.sin_zero, 0, sizeof(sockAdd.sin_zero) * sizeof(char));
    
    
    // bind the socket to an address
    int bind_success = bind(server_fd, (struct sockaddr*)&sockAdd, sizeof(sockAdd));
    while(bind_success == -1){
        // upon error prompt user to enter another port #
        int newPort;
        printf("ERROR: PORT NUMBER IN USE. PLEASE ENTER NEW PORT NUMBER: ");
        scanf("%d", &newPort);
        sockAdd.sin_port = htons(newPort);
        bind_success = bind(server_fd, (struct sockaddr*)&sockAdd, sizeof(sockAdd));
        portNumber = newPort;
    }
    
    printf("Server receiving on port %d\n", portNumber);
    
    
    char recieveBuf[100] = {0};
    struct sockaddr_storage emptyAdd;
    socklen_t len = sizeof(emptyAdd);
    ssize_t numBytesRecieved = recvfrom(server_fd, (void *)recieveBuf, 100, 0, (struct sockaddr*)&emptyAdd, &len);
    
    
    
    
    // send yes to client if buf=ftp
    // sockfd- fd from server socket
    // *buf - message char array which will be saved in a variable
    // len - size of(buf)
    // flags- 0
    // sockaddr- typecast our struct s to this before passing in
    // addrlen - len (from before)
    
    //ssize_t sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen); 
    if (strcmp(recieveBuf, "ftp") == 0){
        char sendBuf[] = "yes";
        ssize_t numBytesSent = sendto(server_fd, (void *)sendBuf, sizeof(sendBuf), 0, (struct sockaddr*)&emptyAdd, sizeof(emptyAdd));
    }
    else{
        char sendBuf[] = "no";
        ssize_t numBytesSent = sendto(server_fd, (void *)sendBuf, sizeof(sendBuf), 0, (struct sockaddr*)&emptyAdd, sizeof(emptyAdd));
    }
    
    
    // 1000 for file data and 200 extra fpr other paramters
    char buf[1200] = {0};
    // define packet and allocate space 
    Packet initial_packet;
    initial_packet.filename = (char*) malloc (100); // allocate space because its a char*
    
    // define file 
    FILE *file = NULL;
    char filePath[1200] = {0};
    bool all_recieved = false;
    bool transfer_start = false;
    int current_frag = 0;
    
    while(!all_recieved){
        
        // receive message in a string buffer
        len = sizeof(emptyAdd);
        numBytesRecieved = recvfrom(server_fd, (void*)buf, 1200, 0, (struct sockaddr*)&emptyAdd, &len);
        if (numBytesRecieved == -1){
            printf("\nError in recvfrom\n");
            exit(1);
        }
        
        // should we process the packet or not?
        if (uniform_rand() > 0.01){
                // convert string to a packet and populate packet so pass in by reference
            convertStringToPacket(buf, &initial_packet);


            // take care of recieving duplicate packets
            if (initial_packet.frag_no <= current_frag){
                printf("Dropping the duplicate packet\n");
                continue;
            }
            else{
                current_frag = initial_packet.frag_no;


                // if we are on the first packet or still need to initialize file name and stuff
                if (!transfer_start){
                    printf("Setting file name\n");
                    printf("Initial_packet filename: %s\n", initial_packet.filename);
                    char* my_file_name = initial_packet.filename;

                    //char my_file_name[100] = initial_packet.filename;
                    printf("Transfer has started for %s\n", my_file_name);
                    // need to open it the first time
                    file = fopen(my_file_name, "wb");
                }
                // write without declaring anything
                fwrite(initial_packet.fileData, 1, initial_packet.size,  file);


                // acknowledge
                char* ack = "success";
                ssize_t numBytesSent = sendto(server_fd, (void*)ack, sizeof(ack), 0, (struct sockaddr*)&emptyAdd, sizeof(emptyAdd));

                // update some variables
                // set to true so it ignores the if statement next time
                transfer_start = true;

                // set break condition, if its last fragment
                if (initial_packet.frag_no == initial_packet.total_frag){
                    all_recieved = true;
                    printf("Transfer has ended.\n");
                }
            }

        }    
        // dont process or ack packet
         else{
             printf("Packet dropped!\n");
             continue;
        }
       
        
        
        
    }
    fclose(file);
    
    return 0;
}

// converts a given string-buf into a packet struct
void convertStringToPacket(char* buf, Packet *pack){
    printf("In convert to packet\n");
    // create an array of strings 
    char* fileInfo[4];
    //char* fileInfo = calloc(4, sizeof(char)*100);
    int start_index = 0;
    int found_colons = 0;
    
    // parse buf
    for (int i=0; i<1200; i++){
        if (buf[i] == ':'){
            int len_of_field = i - start_index;
            // need to first allocate space for any element in fileInfo array
            fileInfo[found_colons] = malloc(sizeof(char)*len_of_field);
            memcpy(fileInfo[found_colons], buf+start_index, len_of_field);
            // next time start from the character after :
            start_index = i + 1;
            found_colons++;
            // need to add this because the data of file can also have : but want to break before it
            if (found_colons == 4){
                break;
            }
        }
    }
    printf("Iterated the buf\n");
    //printf("Printinf fileInfo: %d %d", pack->total_frag, pack->frag_no);
    // now we can transfer items from fileInfo into packet struct
    pack->total_frag = atoi(fileInfo[0]);
    printf("\nTotal frag: %d\n", pack->total_frag);
    pack->frag_no = atoi(fileInfo[1]);
    printf("\nFrag no: %d\n", pack->frag_no);
    pack->size = atoi(fileInfo[2]);
    printf("\nSize: %d\n", pack->size);
    
    //memcpy(&(pack->filename), &fileInfo[3], pack->size);
    pack->filename = fileInfo[3];
    printf("Name: %s\n", pack->filename);
    
    memcpy(&(pack->fileData), buf + start_index, pack->size);
    
    for (int i=0; i<3; i++){
        free(fileInfo[i]);
    }
    
    printf("Conversion finished\n");
}