#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "packet.h"
#include <time.h>
#include <sys/time.h>

//these values were given in lecture
#define ALPHA 0.125
#define BETA 0.25 


void convertPacketToString(char* buf, Packet *pack);
void send_to_server(char* fname, int client_fd, struct sockaddr_in sockAddrIn, double sampleRTT);

void breakTimeout(struct timeval * time, double timeoutValue){
    time->tv_sec = timeoutValue/1; //integer division to set the whole number second
    time->tv_usec = (timeoutValue - time->tv_sec)*100000;  //setting the microsecond part
}
void main(int argc, char** argv){
    
    
    // to listen need socket- https://man7.org/linux/man-pages/man2/socket.2.html
    // domain- AF_INET
    // type- SOCK_DGRAM
    // protocol- 0
    int client_fd = socket(AF_INET, SOCK_DGRAM, 0);
    
    
    // prompt ftp <filename>
    printf("Input a message in the following form -> ftp <filename>: ");
    
    char input[100];
    char fname[100];

    fgets(input, 100, stdin);
    
    //extract just the filename from input
    int size = strlen(input) - 4;
    memcpy(fname, &input[4], size);
    printf("\npoint B");
    
    //get rid of any newline or tab
    strtok(fname, "\n\t"); 
    
    printf("\npoint C");
    if (access(fname, F_OK) != 0){
        printf("\nFile does not exist!");
        exit(0);
    }
    printf("\nFile exists!");
               

    
    struct sockaddr_in sockAddrIn;
    sockAddrIn.sin_family = AF_INET;
    uint16_t portNumber = atoi (argv[2]);
    sockAddrIn.sin_port = htons(portNumber); //from command line
    //sockAddrIn.sin_addr = htonl(argv[1]); //from command line 
    inet_pton(AF_INET, argv[1], &(sockAddrIn.sin_addr));
    memset(sockAddrIn.sin_zero, 0, sizeof(sockAddrIn.sin_zero) * sizeof(char));
    socklen_t len = sizeof(sockAddrIn);
    
    char sendBuf[] = "ftp";
    
    //clock_t start_time, end_time;
    //start_time = clock();
    
    //declaring and initializing the  start_day, and end_day is initalized after the recv
    struct timeval start_day, end_day;
    gettimeofday(&start_day, NULL);
    
 
    ssize_t numBytesSent = sendto(client_fd, (void *)sendBuf, sizeof(sendBuf) * sizeof(char), 0, (struct sockaddr*) &sockAddrIn, len);

    
    char recvBuf[100] = {0}; 
    struct sockaddr_storage empty;
    size_t lenRecvBuf = 100;
    socklen_t addrLen = sizeof(empty);
    int flags = 0;
    
    ssize_t numBytesRecvd = recvfrom(client_fd, (void *)recvBuf, lenRecvBuf, flags, (struct sockaddr*) &empty, &addrLen); //this could be wrong for the last arg because might not have to pass in the &addrlen but just addrlen
    
    //end_time = clock();
    gettimeofday(&end_day, NULL);

    //double total_time = (double)(end_time - start_time)/CLOCKS_PER_SEC;
    
    //initializing a total time val struct 
    struct timeval total_time_day;
    total_time_day.tv_sec = 0;
    total_time_day.tv_usec = 99999; //initializing it to approximately 100ms
    
    //finding the total RTT
    double sec_conv_usec = (double)(end_day.tv_sec*1000000 - start_day.tv_sec*1000000);
    double usec = (double)(end_day.tv_usec - start_day.tv_usec);
    
    //setting the total RTT in to the total_time_day struct
    total_time_day.tv_sec = 0;
    total_time_day.tv_usec = usec + sec_conv_usec;
    
    double RTT =  (usec + sec_conv_usec);
    printf("\nRTT is: %f", RTT);
    
    
    
    
    

    //printf("\nTotal round trip time is (seconds): %f\n", total_time_day.tv_sec);
    //printf("\nTotal round trip time is (useconds): %f\n", total_time_day.tv_usec);
    
    if (strcmp(recvBuf, "yes") == 0){
        printf("\nA file transfer can start.");
        

    }else{
        //exit the program
        exit(0);
    }

    
    printf("\nCalling send to server");
    //sending the file to server **note that empty will get populated by the servers info - so might have to pass that in instead 
    //printf("\nTotal round trip time is (useconds) 2nd time: %f\n", RTT);
    send_to_server(fname, client_fd, sockAddrIn, 5*RTT);

    close(client_fd);
    
}

void send_to_server(char* fname, int client_fd, struct sockaddr_in sockAddrIn, double sampleRTT){
    FILE *file_ptr; 
    file_ptr = fopen(fname, "r");
    
    //move ptr to end of file
    fseek(file_ptr, 0, SEEK_END);
    
    int file_size = ftell(file_ptr);
   
    //move ptr back to beginning of file
    fseek(file_ptr, 0, SEEK_SET);
    //printf("\nCalculating total packets\n");
    int total_frag = 0;
    
    if(file_size%1000 == 0){
       total_frag = file_size/1000;
    }
    else{
        total_frag = file_size/1000 + 1;
    }
    
    //printf("\ntotal_frag is %d\n", total_frag);
    
    int frag_num = 1;
    
    //printf("\nSampleRTT sec: %f\n", sampleRTT.tv_sec);
    printf("\nSampleRTT usec: %f\n", sampleRTT);
    //sampleRTT = 2;
    //calculating values for timeout
    
//    double devRTT = sampleRTT/2;
//    double timeout_time = sampleRTT + 4*devRTT;
//    
    //timer setup for timeout
    struct timeval retran_timeout;
//    retran_timeout.tv_sec = 0;
//    retran_timeout.tv_usec = timeout_time;
    
    retran_timeout.tv_usec = (suseconds_t) sampleRTT;
    retran_timeout.tv_sec = (time_t) 0;

    printf("\nretran sec: %d", retran_timeout.tv_sec);
    printf("\nretran usec: %d", retran_timeout.tv_usec);
    //printf("Point D\n");
    //setting the values for the timeout
    //breakTimeout(&time, 25);
    //printf("Point E\n");
    
    fd_set fdset;
    int n; 
    
    FD_ZERO(&fdset);
    //FD_SET(client_fd, &fdset);
    
    
    
    //clock_t start_time, end_time;
//    struct timeval start_time_day, end_time_day;
    int retransmittedPacket = 0;
    
    //printf("before while loop\n");
    while(frag_num<=total_frag){
        //printf("\nEntering loop\n");
        
        Packet pack; 
        
        //char fileData[1000];
        
        //memset(fileData, 0, 1000);
        //printf("\nPoint Al\n");
        //read from file
        if(!retransmittedPacket){
            //setting the entire packet to 0s
            memset(pack.fileData, 0, 1000);
            int test = fread((void*)pack.fileData, sizeof(char), 1000, file_ptr);
            printf("Read %d bytes on frag %d\n", test, frag_num);
        }
        
        //printf("\nPoint Bl\n");
        //populate the packet
        pack.total_frag = total_frag;
        pack.frag_no = frag_num;
        pack.filename = fname;
        //pack.fileData = fileData;
                
        //check if last packet
        if (frag_num == total_frag){ 
            //takes care of corner case if last packet is also of size 1000
            if (file_size%1000 == 0){
                pack.size = file_size%1000; 
            }else{
                pack.size = file_size%1000; 
            }
            
        }else{
            pack.size = 1000;
        }        
        
        //printf("\nConverting packet to string\n");
        char converted_str[5000];
        //convert packet into string
        int size_converted_str = sprintf(converted_str, "%d:%d:%d:%s:", pack.total_frag, pack.frag_no, pack.size, pack.filename);
        
        //can have converted_str + size_converted_str becuase of pointer aritmetic - and chars are size 1... 
        memcpy(converted_str + size_converted_str, pack.fileData, pack.size);
        //printf("\nConverted String: %s\n", converted_str);
        //sned pack to server
        socklen_t len = sizeof(sockAddrIn);
        
        //printf("\nSending packet to server");
        
        printf("\n fragment number: %d", pack.frag_no);
        
        //to be able to update the timeout time to make it more accurate
        //start_time = clock();
//        gettimeofday(&start_time_day, NULL);
        
        ssize_t numBytesSent = sendto(client_fd, (void *)converted_str, pack.size + size_converted_str, 0, (struct sockaddr*) &sockAddrIn, len);
        
        FD_ZERO(&fdset);
        FD_SET(client_fd, &fdset);
        
        retran_timeout.tv_usec = (suseconds_t) sampleRTT;
        retran_timeout.tv_sec = (time_t) 0;
        
        n = select(client_fd+1, &fdset, NULL, NULL, &retran_timeout);
        
        //printf("\nn is: %d", n);
        if (n == 0){
            printf("\n\nTimed out! Starting retransmission\n");
//            fseek(file_ptr, -pack.size, SEEK_CUR);
            //frag_num = 10000;
            retransmittedPacket = 1;
            continue; //to make sure it goes back to the while loop and retransmits the same frag(note it does not get incremented)
        }
        
        
        //receive ACK from server
        char recvAckBuf[100] = {0}; 
        struct sockaddr_storage empty;
        size_t lenRecvAckBuf = 100;
        socklen_t addrLen = sizeof(empty);
        int flags = 0;

        printf("\nReceiving packet from server");
        ssize_t numBytesRecvd = recvfrom(client_fd, (void *)recvAckBuf, lenRecvAckBuf, flags, (struct sockaddr*) &empty, &addrLen); //this could be wrong for the last arg because might not have to pass in the &addrlen but just addrlen
        
        
        //end_time = clock();
//        gettimeofday(&end_time_day, NULL);
        
//        if(retransmittedPacket != 1){
//            //find better timeout value by making RTT more accurate
//            //double newSampleRTT  = (double)(end_time - start_time)/CLOCKS_PER_SEC;
//             //initializing a total time val struct 
//                        
//            double newSampleRTT = (double)(end_time_day.tv_sec*1000000 - start_time_day.tv_sec*1000000);
//            newSampleRTT = newSampleRTT + (double) (end_time_day.tv_usec - start_time_day.tv_usec);
//            devRTT = (1- BETA) * devRTT + BETA * abs(sampleRTT - newSampleRTT);
//            sampleRTT = (1-ALPHA)*sampleRTT + ALPHA * newSampleRTT;
//            timeout_time = sampleRTT + 4*devRTT;
//            retran_timeout.tv_usec = timeout_time;
//
//            /*if (timeout<1){
//                timeout = 1;
//            }*/
//            //update the timeout value
//            //time.tv_sec = timeout;
//            //time.tv_usec = 0;
//            //breakTimeout(&time, timeout);
//        }
//        
        retransmittedPacket = 0;
        
        if (strcmp(recvAckBuf, "success") == 0){
            printf("Ack received!\n");
        }else{
            printf("Ack not recieved! Program exiting...\n");
            exit(0);
        }
        
        frag_num++;
              
                
    }
    
    
    

}
