/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/main.c to edit this template
 */

/* 
 * File:   server.c
 * Author: dasadias
 *
 * Created on March 16, 2022, 7:51 p.m.
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

//user user1;
//strcpy(user1.username, "user1");
//strcpy(user1.password, "user1");
//user1.sockfd = -1;
//strcpy(user1.ip_add, "");
//user1.logged_on = 0;
//user1.connected_session = 0;
//strcpy(user1.sessionID, "");



user user_list[NUM_USERS] = {
    {.username = "user1", .password = "user1", .sockfd = -1, .ip_add = "", .logged_on = 0, .connected_session = 0, .sessionID = ""},
    {.username = "user2", .password = "user2", .sockfd = -1, .ip_add = "", .logged_on = 0, .connected_session = 0, .sessionID = ""},
    {.username = "user3", .password = "user3", .sockfd = -1, .ip_add = "", .logged_on = 0, .connected_session = 0, .sessionID = ""},
    {.username = "user4", .password = "user4", .sockfd = -1, .ip_add = "", .logged_on = 0, .connected_session = 0, .sessionID = ""}

};
session* sessions_list = NULL;
char* user_already_logged_in = "User is already logged on";
char* wrong_user_pass = "Wrong username or password";
char* user_already_in_sess = "User is already in a session";
char* user_not_logged_in = "User is not logged in";
char* session_not_found = "Session not found";
char* session_already_exists = "Session already exists";












int main2(int argc, char** argv) {

    if (argc != 2){
        printf("usage: server <port number>\n");
        exit(1);
    }
    char* portNum = argv[1];
    
    
    // from Beej's guide
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    
    int rv;
    if ((rv = getaddrinfo(NULL, portNum, &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // create a socket to then bind it to a port
    // res points to the structs so bind to first one we can
    int sockfd;
    int yes = 1;
    for (p = res; p!=NULL; p = p->ai_next){
        
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("server: socket");
            continue;
        }
        
        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
            perror("setsockopt");
            exit(1);
        }
        
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            perror("server: bind");
            continue;
        }
        break;
    }
    
    freeaddrinfo(res);
    
    if(p == NULL){
        fprintf(stderr, "server: failed to bind\n");
    }
    
    if (listen(sockfd, BACKLOG) == -1){
        perror("listen");
        exit(1);
    }
    printf("server is waiting for a connection...\n");
    
    // from source in tut handout
    fd_set master_set; // master fd list
    fd_set read_fds;   // temp fd list
    int fdmax;         // max fd number
    
    // clear
    FD_ZERO(&master_set);
    FD_ZERO(&read_fds);
    
    // add to master set
    FD_SET(sockfd, &master_set);
    fdmax = sockfd;   // keep track of the largest fd
    
    socklen_t size_sin;
    struct sockaddr_storage remote_addr; // clients address
    int newfd; // newly accepted descriptor for the socket
    int number_bytes = 0;
    char buf[PACKET_LEN] = {0}; // buffer for data sent by client
    char remoteIP[INET_ADDRSTRLEN]; // connectors IP in IPv4
    
    // variables for messages/packets recieved from client
    message *msg_got, *msg_sent;
    char* msg_src;
    
    while(1){
        printf("in while loop\n");
        read_fds = master_set;
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
            perror("select");
            exit(4);
        }
        
        // run through existing connections to read some data
        printf("running through the fd's\n");
        for (int i = 0; i<= fdmax; i++){
            // can read from a socket
            if (FD_ISSET(i, &read_fds)){
                printf("\nPOINT A\n");
                // we have a new connection
                if (i == sockfd){
                    // handling new connections
                    printf("we have new connection\n");
                    size_sin = sizeof(remote_addr);
                    newfd = accept(sockfd, (struct sockaddr *)&remote_addr, &size_sin); //accept connection
                    if (newfd == -1){
                        perror("accept");
                    }
                    else{
                        FD_SET(newfd, &master_set); // add to master set of fd's
                        // keeping track of the max fd
                        if(newfd > fdmax){
                            fdmax = newfd;
                        }
                        
                        
                        printf("selectserver: new connection from %s on socket %d\n", inet_ntop(remote_addr.ss_family, get_in_addr((struct sockaddr*)&remote_addr),remoteIP, sizeof(remoteIP)), newfd);
                    }
                }
                // we dont have a new connection but are receiving data
                else{
                    printf("\nPOINT B\n");
                    //memset(buf, 0, 1100);
                    if ((number_bytes = recv(i, buf, sizeof(buf), 0)) <= 0){
                        printf("\nPOINT C\n");
                        // error in recv or connection closed by client
                        if (number_bytes == 0){
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        }else{
                            perror("recv");
                        }
                        // close client connection
                        close(i);
                        // remove from master
                        FD_CLR(i, &master_set);
                    }
                    // actually got data from the client
                    else{
                        printf("\ngot data from client\n");
                        printf("\nnumbytes: %d\n", number_bytes);
                        // need to process the string to a message struct
                        msg_got = string_to_packet(buf, number_bytes);
                        msg_src = msg_got->source;
                        memset(buf, 0, PACKET_LEN);
                        printf("\nPOINT J\n");
                        char username[100];
                        char password[100];
                        strcpy(username, msg_got->source);
                        if (msg_got->type == LOGIN){
                            printf("\nPOINT K\n");
                            // get username and password from msg_got->data
                            // username and password will be seperated by a space
                            // eg: sanjana pass
                            char* token = strtok(msg_got->data, " ");
                            //strcpy(username, token);
                            token = strtok(NULL, " ");
                            strcpy(password, token);
                            bool valid = false;
                            //strcpy(username, msg_got->source);
                            printf("\nUsername:%s\n", username);
                            printf("\nPassword:%s\n", password);
                            
                            // check if anyone with same username exists already
                            for (int j = 0; j<NUM_USERS; j++){
                                printf("\nPoint L\n");
                                // if the username exists, check the password
                                if ((strcmp(user_list[j].username, username) == 0) && (strcmp(user_list[j].password, password) == 0)){
                                    printf("\nPoint M\n");
                                    // check if they are already logged in or not
                                    if (user_list[j].logged_on){
                                        // user already logged in so send LO_NACK
                                        respond_to_client(i, LO_NAK, "server",  user_already_logged_in);
                                    }
                                    else{
                                        // acknowledge successful login
                                        respond_to_client(i, LO_ACK, "server", "");
                                        // update parameters
                                        user_list[j].logged_on = 1;
                                        user_list[j].sockfd = newfd;
                                        strcpy(user_list[j].ip_add, remoteIP);
                                        valid = true;
                                        printf("Client %s connected to server\n", username);
                                    }
                                    break;
                                }
                                else if (j == NUM_USERS - 1){
                                    printf("Wrong username or password\n");
                                    respond_to_client(i, LO_NAK, "server", wrong_user_pass);
                                }
                                
                            }
                            if (!valid){
                                close(i);
                                FD_CLR(i, &master_set);
                            }
                            
                        }
                        else if (msg_got->type == EXIT){
                            // set state to logged out and remove from sessions
                            for (int j = 0; j<NUM_USERS; j++){
                                if (strcmp(user_list[j].username, username) == 0){
                                    if (user_list[j].logged_on){
                                        exit_user(j, msg_got->source);
                                        printf("User %s has been logged out", msg_got->source);
                                        close(user_list[j].sockfd);
                                        FD_CLR(user_list[j].sockfd, &master_set);
                                        user_list[j].sockfd = -1;
                                    }
                                    break;
                                }
                            }
                            
                        }
                        else if(msg_got->type == JOIN){
                            for (int j = 0; j<NUM_USERS; j++){
                                if (strcmp(user_list[j].username, username) == 0){
                                    // check if they are logged on
                                    if (user_list[j].logged_on){
                                        char* sessionID = msg_got->data;
                                        join_sess(j, sessionID, msg_got->source, i);
                                    }
                                    else{
                                        printf("User %s is not logged in\n", user_list[j].username);
                                        respond_to_client(i, JN_NAK, "server",  user_not_logged_in);
                                    }
                                    break;
                                }
                            }
                        }
                        else if (msg_got->type == LEAVE_SESS){
                            for (int j = 0; j<NUM_USERS; j++){
                                if (strcmp(user_list[j].username, username) == 0){
                                    // check if they are logged on
                                    if (user_list[j].logged_on){
                                        if (user_list[j].connected_session == 0){
                                            // user is not even part of a session nack
                                            printf("User %s not part of a session\n", user_list[j].username);
                                        }
                                        else{
                                            // they are part of a session so make them leave
                                            printf("User %s removed from session %s\n", user_list[j].username, user_list[j].sessionID);
                                            leave_sess(j, msg_got->source);
                                        }
                                    }
                                    break;
                                }
                            }
                            
                        }
                        else if (msg_got->type == NEW_SESS){
                            // check if session exists already
                            for (int j = 0; j<NUM_USERS; j++){
                                if (strcmp(user_list[j].username, username) == 0){
                                    printf("\nusername in new_sess if statement:%s\n", username);
                                    printf("\nuser_list[j] in new_sess if statement:%s\n", user_list[j].username);
                                    char* sessionID = msg_got->data;
                                    // check if they are logged on
                                    if (user_list[j].logged_on){
                                        printf("\nsession id in server.c:%s\n", sessionID);
                                        new_session(j, sessionID, msg_got->source, i);
                                    }
                                    break;
                                }
                            }
                        }
                        else if (msg_got->type == QUERY){
                            char *db = get_user_db();
                            respond_to_client(i, QU_ACK, "server", db);
                            free(db);
                        }
                        else if(msg_got->type == MESSAGE){
                            for (int j = 0; j<NUM_USERS; j++){
                                if (strcmp(user_list[j].username, username) == 0){
                                    // check if they are logged on
                                    if (user_list[j].logged_on){
                                        // get a pointer to the users session
                                        session* curr_sess = search_session(sessions_list, user_list[j].sessionID);
                                        assert(curr_sess);
                                        user_node* user_head = curr_sess->users;
                                        // convert the packet to a string
                                        char* msg_string = packet_to_string(msg_got);
                                        
                                        // traverse inner linked list of the users
                                        while(user_head != NULL){
                                            int k = find_sock(user_head->username);
                                            if (k != -1){
                                                if (FD_ISSET(k, &master_set)){
                                                    // send to everyone except for server and the client who wants to send msg
                                                    if (k != sockfd && k != i){
                                                        if (send(k, msg_string, number_bytes, 0) == -1){
                                                            perror("send");
                                                        }
                                                    }
                                                }
                                            }
                                            user_head = user_head->next;
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                        else if (msg_got->type == INVITE){
                            // msg_got->data is our dest username
                            // have to check if src user is in a session, else nak
                            // have to check if dest user is logged on, if not then nak
                            // if dest user is logged on, get their sockfd and send an invite string
                            // call recv on that sockfd because waiting for yes or no
                            // if response is yes, ack to original client and add user to session
                            // if response is no, nak to original client
                            printf("\nPoint K\n");
                            
                            int dest_idx = 0;
                            for (int k =0; k<NUM_USERS; k++){
                                if (strcmp(user_list[k].username, msg_got->data) == 0){
                                    dest_idx = k;
                                    break;
                                }
                            }
                            for (int j = 0; j<NUM_USERS; j++){
                                if (strcmp(user_list[j].username, username) == 0){
                                    if (user_list[j].connected_session){
                                        // check if dest user logged 
                                        if (user_list[dest_idx].logged_on){
                                            if (user_list[dest_idx].connected_session){
                                                // dest user already part of session send nak to original client
                                                respond_to_client(i, INV_NAK, "server", user_already_in_sess);
                                            }
                                            else{
                                                printf("\nPoint L\n");
                                                //char* invitation = "You are invited to join ";
                                                char* invitation = malloc(sizeof(char)*1000);
                                                strcpy(invitation, "");
                                                strcpy(invitation, "You are invited to join ");
                                                strcat(invitation, user_list[j].sessionID);
                                                printf("\nPoint M\n");
                                                //strcat(invitation, user_list[j].sessionID);
                                                printf("\nPoint N\n");
                                                respond_to_client(user_list[dest_idx].sockfd, INVITE, msg_got->source, invitation);
                                                free(invitation);
                                                
                                                char response_buf[PACKET_LEN] = {0};
                                                int bytes_rec = recv(user_list[dest_idx].sockfd, response_buf, sizeof(response_buf), 0);
                                                printf("\nResponse buf: %s, bytes_rec: %d\n", response_buf, bytes_rec);
                                                if (strcmp(response_buf, "yes") == 0){
                                                    // add them to sess
                                                    session* found_sess = search_session(sessions_list, user_list[j].sessionID);
                                                    if (found_sess != NULL){
                                                        // add user to session
                                                        add_to_sess(found_sess, msg_got->data);
                                                        set_sess_id(dest_idx, user_list[j].sessionID);
                                                        // send ack to original client
                                                        respond_to_client(i, INV_ACK, "server", "Invitation was accepted");
                                                    }
                                                }
                                                else{
                                                    // nak original client
                                                    respond_to_client(i, INV_NAK, "server", "User does not want to join");
                                                }
                                            }
                                            
                                        }
                                        else{
                                            // send nack
                                            respond_to_client(i, INV_NAK, "server", "User not logged on");
                                        }
                                    }
                                    else{
                                        // send nack
                                        respond_to_client(i, INV_NAK, "server", "User not in a session");
                                    }
                                    break;
                                }
                            }
                        }
                        else if (msg_got->type == PM){
                            // a client wants to send a message to another client
                            // data will contain "dest_userid <text>"
                            // need to strtok to extract information separetely
                            char destuserID[100];
                            char msg[PACKET_LEN] = {0};
                            char* token = strtok(msg_got->data, " ");
                            strcpy(destuserID, token);
                            strcpy(msg, "");
                            
                            printf("\nusername from token: %s\n", destuserID);
                            
                            int first_item = 1;
                            int counter = 1;
                            
                            while(token != NULL) { //printing each token
                              
                              printf("\nToken %d: %s", counter, token);
                              if (counter == 1 && first_item != 1){
                                  printf("\nin strcpy()");
                                  strcpy(msg, token);
                              }
                              if (first_item != 1 && counter >= 2){
                                  printf("\nin strcat");
                                  strcat(msg, " ");
                                  strcat(msg, token);
                              }
                              token = strtok(NULL, " ");
                              counter++;
                              first_item = 0;
                            }
                            //token = strtok(NULL, " ");
                            //strcpy(msg, token);
                            printf("\nmsg: %s\n", msg);
                            // find dest user
                            for (int j = 0; j<NUM_USERS; j++){
                                if (strcmp(user_list[j].username, destuserID) == 0){
                                    // check if they are logged in
                                    if (user_list[j].logged_on){
                                        // send msg
                                        respond_to_client(user_list[j].sockfd, PM, msg_got->source, msg);
                                        respond_to_client(i, PM_ACK, "server", "Private message was sent");
                                    }
                                    else{
                                        respond_to_client(i, PM_NAK, "server", "User is not logged in");
                                    }
                                    break;
                                } 
                            }
                        }
                        
                    }
                }
            }
            
        }
    }
    
 
    
    return (EXIT_SUCCESS);
}