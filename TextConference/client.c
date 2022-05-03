/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/main.c to edit this template
 */

/* 
 * File:   main.c
 * Author: shahswen
 *
 * Created on March 20, 2022, 2:39 p.m.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include "help.h"
#include <string.h>

#define STDIN 0



/*
 * 
 */

//GLOBAL VARIABLES

//set this to 1 when a successful login is made
int isConnected = 0; 
int sockfd_client = -1;
unsigned int size_send = 0;


int main(int argc, char** argv) {
    
    //so that it can be used in other requests made by user too!
    char commandName[COMMAND_INPUT_LEN];
    char username [USERNAME_LEN];
    char password [PASSWORD_LEN];
    char IP_addr [IPADDR_LEN];
    //might have to convert this to an int using atoi-check this
    char port_number_str [PORT_NUM_LEN];
    
    printf("Hello, client is running\n");
    if (argc != 1){
        printf("usage: client\n");
        exit(1);
    }
    
    fd_set fd;
    FD_ZERO (&fd);
    while (1){
        //zero out the fd each time - otherwise will cause problems
        FD_ZERO (&fd);
        
        //ensure that there is a user connected
//        if(sockfd_client != -1 ){
//            printf("\n In Sockfs not -1 if\n");
//            FD_SET(sockfd_client, &fd);
//            //last field is NULL for now because no timer needed
//            select(sockfd_client + 1, &fd, NULL, NULL, NULL);
//            
//            if (isConnected && FD_ISSET(sockfd_client, &fd)){
//                recieve_message_from_server();
//            }
//        }
        FD_SET(STDIN, &fd);
        
       
        //last field is NULL for now because no timer needed
        
       
        
        if(isConnected &&sockfd_client != -1){
            //calling select on the largest fd!!
            FD_SET(sockfd_client, &fd);
            select(sockfd_client + 1, &fd, NULL, NULL, NULL); 
        }else{
            select(STDIN + 1, &fd, NULL, NULL, NULL);
        }
        
       
       
        
        if (FD_ISSET(STDIN, &fd)){
            printf("Received user input\n");
            //check what command from user input and process accordingly
//            char commandName[COMMAND_INPUT_LEN];
//            commandName = argv[0];
            scanf("%s", commandName);
            printf("command name is: %s\n", commandName);
            //check for the different commandNames 
            if (strcmp(commandName, "login") == 0){
                //extracting the other arguments
//                char username [USERNAME_LEN] = argv[1];
//                char password [PASSWORD_LEN] = argv[2];
//                char IP_addr [IPADDR_LEN] = argv[3];
//                //might have to convert this to an int using atoi-check this
//                char port_number_str [PORT_NUM_LEN] = argv[4];
                
//                char username [USERNAME_LEN];
//                char password [PASSWORD_LEN];
//                char IP_addr [IPADDR_LEN];
//                //might have to convert this to an int using atoi-check this
//                char port_number_str [PORT_NUM_LEN];
                
                //read in from command line
                scanf(" %s %s %s %s", username, password, IP_addr, port_number_str);
                
                printf("username: %send, pass: %s, IP: %s, PN: %s\n", username, password, IP_addr, port_number_str);
                
                printf("\ncalling connect client");
                connect_client(IP_addr, port_number_str);
                
                printf("\n about to call login process");
                login_process(username, password);
            }
            else if (strcmp(commandName, "logout") == 0){
                //no recieveing from server here - server does not send anything
                //no command line inputs either
                //don't exit client
                
                //check if a user logged in
                if (!isConnected){
                    fprintf(stderr, "No user is logged in currently!\n");
                    continue;
                }
                
                int len = 0; 
                char data [PACKET_LEN] = "";
                
                //create message struct and then convert it to string
                message*msg = create_packet(EXIT, len, username, data);
                char*conv_str = packet_to_string(msg);
                
                //send to server
                send(sockfd_client, conv_str, strlen(conv_str), 0);
                free(conv_str);
                
                //close socket and reset sockfd_client to invalid
                close(sockfd_client);
                sockfd_client = -1;
                continue;
                //might not need this print because server implements prints
                //printf("Successfully logged out!\n");            
                
            }
            else if (strcmp(commandName, "joinsession") == 0){
                //check if a user logged in
                if (!isConnected){
                    fprintf(stderr, "No user is logged in currently!\n");
                    continue;
                }
                
                char sessionID[SESSION_ID_LEN];
                scanf(" %s", sessionID);
                
                //data is just sessionID
                char data[PACKET_LEN];
                strcpy(data, sessionID);
                int len = strlen(sessionID);
                
                //create message struct and then convert it to string
                message*msg = create_packet(JOIN, len, username, data);
                char*conv_str = packet_to_string(msg);
                
                //send to server
                send(sockfd_client, conv_str, strlen(conv_str), 0);
                free(conv_str);
                
                //recv from server
                char buf[PACKET_LEN];
                int num_bytes_recvd = recv(sockfd_client, buf, sizeof(buf), 0);
                
                //conv string to pack
                message*msg_recvd = string_to_packet(buf, num_bytes_recvd);
                
                if(msg_recvd->type == JN_NAK){
                    printf("Unable to join session: %s\n", msg_recvd->data);
                }else if (msg_recvd->type == JN_ACK){
                    printf("Session joined!\n");
                }else{
                    printf("Invalid message received!\n");
                }
                
            }
            else if(strcmp(commandName, "leavesession") == 0){
                if (!isConnected){
                    fprintf(stderr, "No user is logged in currently!\n");
                    continue;
                }
                
                //no data
                int len = 0; 
                char data [PACKET_LEN] = "";
                
                //create message struct and then convert it to string
                message*msg = create_packet(LEAVE_SESS, len, username, data);
                char*conv_str = packet_to_string(msg);
                
                //send to server
                send(sockfd_client, conv_str, strlen(conv_str), 0);
                printf("\nThe session has been left\n");
                free(conv_str);
                
            }
            else if(strcmp(commandName, "createsession") == 0){
                //check if a user logged in
                if (!isConnected){
                    fprintf(stderr, "No user is logged in currently!\n");
                    continue;
                }
                
                char sessionID[SESSION_ID_LEN];
                scanf(" %s", sessionID);
                
                //data is just sessionID
                char data[PACKET_LEN];
                strcpy(data, sessionID);
                int len = strlen(sessionID);
                
                //create message struct and then convert it to string
                message*msg = create_packet(NEW_SESS, len, username, data);
                char*conv_str = packet_to_string(msg);
                
                //send to server
                send(sockfd_client, conv_str, strlen(conv_str), 0);
                free(conv_str);
                
                //recv from server
                char buf[PACKET_LEN];
                int num_bytes_recvd = recv(sockfd_client, buf, sizeof(buf), 0);
                
                //conv string to pack
                message*msg_recvd = string_to_packet(buf, num_bytes_recvd);
                
                if(msg_recvd->type == NS_NAK){
                    printf("Unable to create session: %s\n", msg_recvd->data);
                }else if (msg_recvd->type == NS_ACK){
                    printf("Session created!\n");
                }else{
                    printf("Invalid message received!\n");
                }
            }
            else if(strcmp(commandName, "list") == 0){
                if (!isConnected){
                    fprintf(stderr, "No user is logged in currently!\n");
                    continue;
                }
                
                //no data
                int len = 0; 
                char data [PACKET_LEN] = "";
                
                //create message struct and then convert it to string
                message*msg = create_packet(QUERY, len, username, data);
                char*conv_str = packet_to_string(msg);
                
                //send to server
                send(sockfd_client, conv_str, strlen(conv_str), 0);
                free(conv_str);
                
                char buf[PACKET_LEN];
                int num_bytes_recvd = recv(sockfd_client, buf, sizeof(buf), 0);
                
                //conv string to pack
                message*msg_recvd = string_to_packet(buf, num_bytes_recvd);
                
//                if(msg_recvd->type == NS_NAK){
//                    printf("Unable to create session: %s\n", msg_recvd->data);
//                }
                if (msg_recvd->type == QU_ACK){
                    printf("Here is the list of the active users and sessions: %s!\n", msg_recvd->data);
                }else{
                    printf("Invalid message received!\n");
                }   
            }
            else if(strcmp(commandName, "quit") == 0){
             
                //logging the user out
                //check if a user logged in
                if (!isConnected){
                    fprintf(stderr, "No user is logged in currently!\n");
                    continue;
                }
                
                int len = 0; 
                char data [PACKET_LEN] = "";
                
                //create message struct and then convert it to string
                message*msg = create_packet(EXIT, len, username, data);
                char*conv_str = packet_to_string(msg);
                
                //send to server
                send(sockfd_client, conv_str, strlen(conv_str), 0);
                free(conv_str);
                
                //close socket and reset sockfd_client to invalid
                close(sockfd_client);
                sockfd_client = -1;
                
                //at this point user has been logged out
                
                printf("Text conferencing program quitting!\n");
                exit(0);  
            }
            else if(strcmp(commandName, "pm") == 0){//input format <commandname usertomessage messageitself>
                
                char username_pm [USERNAME_LEN];
                char message_to_send[5000];
                
                scanf(" %s", username_pm);
                fgets(message_to_send, 5000, stdin);//reading in from commad line could just use scanf but used this in the message part
                
                //SERVER CAN CHECK IF THE USERNAME IS VALID!!!!!!!!-TELL SANJANA
                
                int len = 0; 
                char data [PACKET_LEN] = "";
                
                //processing data to send to server, format: <usernametosendto (space) message to be sent>  
                strcpy(data, "");
                strcat(data, username_pm);
                strcat(data, " ");
                strcat(data, message_to_send);
                
                len = strlen(data);
                
                
                
                //create message struct and then convert it to string
                message*msg = create_packet(PM, len, username, data);
                char*conv_str = packet_to_string(msg);
                
                //send to server
                send(sockfd_client, conv_str, strlen(conv_str), 0); //might have to make size_send instead of strlen(conv_str), and if size_send used, reset it after right after its sent to 0
                //if use size_send reset it to 0 here!
                free(conv_str);
                
                
                
                //recv from server
                char buf[PACKET_LEN];
                int num_bytes_recvd = recv(sockfd_client, buf, sizeof(buf), 0);
                
                //conv string to pack
                message*msg_recvd = string_to_packet(buf, num_bytes_recvd);
                
                //CHANGE THE MESSAGES IN THIS FOR PM BECAUSE RIGHT NOW THEY ARE NOT
                
                if(msg_recvd->type == PM_NAK){
                    printf("Unable to send pm: %s\n", msg_recvd->data);
                }else if (msg_recvd->type == PM_ACK){
                    printf("PM sent successfully!\n");
                }else{
                    printf("Invalid message received!\n");
                }
                        
                
            }
            else if(strcmp(commandName, "invite") == 0){//input command <userbeinginvited>
                //SERVER WILL HAVE TO CHECK IF THE USER INVITING THE OTHER USER IS IN A SESS FIRST, AND 
                //IF THE USER BEING INVITED EXISTS
                char username_invited [USERNAME_LEN];
                scanf(" %s", username_invited);
                
                //data sent to server will be just the name of the user thats being invited! ->username_invited
                int len = strlen(username_invited);
                
                //create message struct and then convert it to string
                message*msg = create_packet(INVITE, len, username, username_invited);
                char*conv_str = packet_to_string(msg);
                
                //send to server
                send(sockfd_client, conv_str, strlen(conv_str), 0); //might have to make size_send instead of strlen(conv_str), and if size_send used, reset it after right after its sent to 0
                //if use size_send reset it to 0 here!
                free(conv_str);
                
                //recv from server
                char buf[PACKET_LEN];
                int num_bytes_recvd = recv(sockfd_client, buf, sizeof(buf), 0);
                
                //conv string to pack
                message*msg_recvd = string_to_packet(buf, num_bytes_recvd);
                
                //CHANGE THE MESSAGES IN THIS FOR PM BECAUSE RIGHT NOW THEY ARE NOT
                
                if(msg_recvd->type == INV_NAK){
                    printf("Unable to send invite: %s\n", msg_recvd->data);
                }else if (msg_recvd->type == INV_ACK){
                    printf("Invite sent successfully!\n");
                }else{
                    printf("Invalid message received!\n");
                }
                
                
                
            }
            
            else{ //the case that only text is entered, need to send to server so they can broadcast it
                char data [5000];
               
                //since extracted the command earlier need to add it back into the message
                strcpy(data, commandName);
                int lenCommand = strlen(commandName);

                //read from commad line
                fgets(data + lenCommand, 5000 - lenCommand, stdin);

                int len = strlen(data);

                //create message struct and then convert it to string
                message*msg = create_packet(MESSAGE, len, username, data);
                char*conv_str = packet_to_string(msg);

                //send to server
                send(sockfd_client, conv_str, strlen(conv_str), 0); //should the strlen(conv_str) be size_send - found in paacket to string?
                free(conv_str);
            }
               
               
               
                 
               
            
            
        }
        
        

        if (isConnected && FD_ISSET(sockfd_client, &fd)){
            //recieve_message_from_server();
            char buf [5000];
            int num_bytes_recvd = recv(sockfd_client, buf, 5000, 0);
            
            message*msg_recvd = string_to_packet(buf, num_bytes_recvd);
            
            if(msg_recvd->type == MESSAGE){ 
                printf("\nMessage from %s: %s\n", msg_recvd->source,msg_recvd->data);
            }else if (msg_recvd->type == PM){
                printf("\nPrivate message from %s: %s\n", msg_recvd->source,msg_recvd->data);
            }
            else if (msg_recvd->type == INVITE){
                printf("\nAn invitation was received from %s:\n\t%s", msg_recvd->source, msg_recvd->data);
                printf("\n\tRespond with y to accept or n to decline the invite: ");
                
                char response[5];
                printf("\nPoint A\n");
                //fgets(response, 5, stdin);
                scanf("%s", response);
                //printf("\nresponse is: %s\n", response);
                
                if(strcmp(response, "y") == 0){
                    //send to server
                    //printf("\npoint B\n");
                    
                    int num_bytes_sent = send(sockfd_client, "yes", strlen("yes"), 0);
                    //printf("Num bytes sent is: %d", num_bytes_sent);
                    printf("\nYou have been successfully added to the session!");
                }else{
                    printf("\n Point C\n");
                    send(sockfd_client, "no", strlen("no"), 0);
                    //printf("\nYou have not been added to the session!\n");
                }
            }    
            
            //buf[5000-1] = '\0';
            
            
            
//            //since extracted the command earlier need to add it back into the message
//            strcpy(data, commandName);
//            int lenCommand = strlen(commandName);
//
//            //read from commad line
//            fgets(data + lenCommand, 5000 - lenCommand, stdin);
//
//            int len = strlen(data);
//
//            //create message struct and then convert it to string
//            message*msg = create_packet(MESSAGE, len, username, data);
//            char*conv_str = packet_to_string(msg);
//
//            //send to server
//            send(sockfd_client, conv_str, strlen(conv_str), 0);
//            free(conv_str);
        }
        
        
    }
    
    //wait for user input on command line
        //put the input in packets then convert pack to string, and send it to the server
    
    //client should be able to login (using username and password)
    //once a client has logged in, it can join a session 
    //once session joined need to wait for messages from server AND also input from command line at the same time 
    //non blocking sockets or multi threading to deal with two sources (said above) simultaneously
    
    
    
    
    return (EXIT_SUCCESS);
}