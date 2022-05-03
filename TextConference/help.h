/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.h to edit this template
 */

/* 
 * File:   help.h
 * Author: dasadias
 *
 * Created on March 19, 2022, 3:41 p.m.
 */

#ifndef HELP_H
#define HELP_H

#ifdef __cplusplus
extern "C" {
#endif
    
    
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
#include <assert.h>
    
#define BACKLOG 10
#define PACKET_LEN 1100
#define SOURCE_LEN 30
#define NUM_USERS 4
#define DATA_LEN 1000
#define COMMAND_INPUT_LEN 15
#define USERNAME_LEN 30
#define PASSWORD_LEN 10
#define IPADDR_LEN 20
#define PORT_NUM_LEN 10
#define SESSION_ID_LEN 10
#define LOGIN 1
#define LO_ACK 2
#define LO_NAK 3
#define EXIT 4
#define JOIN 5
#define JN_ACK 6
#define JN_NAK 7
#define LEAVE_SESS 8
#define NEW_SESS 9
#define NS_ACK 10
#define MESSAGE 11
#define QUERY 12
#define QU_ACK 13
#define NS_NAK 14
#define PM 15
#define INVITE 16
#define PM_ACK 17
#define PM_NAK 18
#define INV_ACK 19
#define INV_NAK 20
#define INV_MSG 21
    


struct message { 
    //MIGHT NEED TO ADD SESSION ID IN HERE BECAUSE SOME MESSAGE TYPES HAVE THIS
    unsigned int type; 
    unsigned int size; 
    unsigned char source[SOURCE_LEN]; 
    unsigned char data[PACKET_LEN]; 
}; typedef struct message message;

struct user{
    char username[SOURCE_LEN];
    char password[SOURCE_LEN];
    int sockfd;
    char ip_add[INET_ADDRSTRLEN];
    int logged_on;
    int connected_session;
    char sessionID[100];
}; typedef struct user user;

struct user_node{
    char username[SOURCE_LEN];
    struct user_node* next;
}; typedef struct user_node user_node;


struct session{
    char sessionID[100];
    int num_users;
    user_node* users;
    struct session *next;
}; typedef struct session session;












extern user user_list[NUM_USERS];
extern session* sessions_list;
extern char* user_already_logged_in;
extern char* wrong_user_pass;
extern char* user_already_in_sess;
extern char* user_not_logged_in;
extern char* session_not_found;
extern char* session_already_exists;
//will get set once a user logs in, so just setting it to a negative value for now
extern int sockfd_client;
extern int isConnected;
extern unsigned int size_send;

void *get_in_addr(struct sockaddr *sa);
message* string_to_packet (char* buf, int number_bytes);
char* packet_to_string (message *msg);
int respond_to_client (int sockfd, int msg_type, char* username,  char* msg_buf);
message* create_packet(int type, int len, char* source, char* data);
session* search_session(session* list, char* sessionID);
void remove_from_session(session* main_sess, char* id);
void delete_sess(session **list, char *id);
void exit_user(int j, char* src);
user_node* add_new_user(char* user_id);
void add_to_sess(session* sess, char* user_id);
void set_sess_id(int j, char* sess_id);
void join_sess(int j, char* sessionID, char* source, int sockfd);
void leave_sess(int j, char* src);
session* create_session(session** sessions_list, char* sessionID);
void new_session(int j, char* sessionID, char* source, int sockfd);
char* get_user_db();
int find_sock(char* user_id);
void recieve_message_from_server();
int connect_client(char IP_addr[], char port_number_str[]);
void login_process(char username[], char password[]);


#ifdef __cplusplus
}
#endif

#endif /* HELP_H */