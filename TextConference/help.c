/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cFiles/file.c to edit this template
 */
#include "help.h"





void* get_in_addr(struct sockaddr *sa){
    // IPv4
    if (sa->sa_family == AF_INET){
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    // IPv6
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



// convert string to packet 
message* string_to_packet (char* buf, int number_bytes){
    printf("\nIn string to packet: %s\n", buf);
    message* our_msg = malloc(sizeof(message));
    // initialize as empty strings
/*
    printf("\nPOINT D\n");
    strcpy(our_msg->source, "");
    strcpy(our_msg->data, "");
    our_msg->type = -1;
    our_msg->size = 0;
    unsigned int size_unsigned_int = sizeof(unsigned int);
    unsigned int diff = 0;
    unsigned int i = 0;
    
    char current_char;
    unsigned char source_copy[SOURCE_LEN];
    unsigned char data_copy[PACKET_LEN];
    printf("\nPOINT E\n");
    // read type- type is a number
    memcpy(&(our_msg->type), buf, size_unsigned_int);
    diff = diff + size_unsigned_int + 1;
    printf("\nPOINT F TYPE: %d\n", our_msg->type);
    // read size
    memcpy(&(our_msg->size), buf+diff, size_unsigned_int);
    diff = diff + size_unsigned_int + 1;
    printf("\nPOINT G MSG_SIZE: %d\n", our_msg->size);
    // read source
    while(buf[diff] != ':'){
        our_msg->source[i++] = buf[diff++];
        // i++;
        // diff++;
    }
    diff = diff + 1; // to go to start of next term
    printf("\nPOINT H\n");
    // read rest which is data
    memcpy(&(our_msg->data), buf+diff, our_msg->size);
    printf("\nPOINT I\n");
    return our_msg;
*/
    // 4 elements in message
    int num_colons = 3;
    char* fileInfo[num_colons];
    int start_index = 0;
    int found_colons = 0;
    
    for (int i=0; i<sizeof(message); i++){
        if (buf[i] == ':'){
            int len_of_field = i - start_index;
            // need to first allocate space for any element in fileInfo array
            fileInfo[found_colons] = malloc(sizeof(char)*len_of_field + 1);
            strncpy(fileInfo[found_colons], buf+start_index, len_of_field);
            fileInfo[found_colons][len_of_field] = '\0';
            // skip the colon :
            start_index = i + 1;
            found_colons++;
            if (found_colons == num_colons){
                break;
            }
        }
    }
/*
    if (found_colons != num_colons){
        printf("\npacket not formatted correctly\n");
        return(1);
    }
*/
    
    our_msg->type = atoi(fileInfo[0]);
    our_msg->size = atoi(fileInfo[1]);
    strcpy(our_msg->source, fileInfo[2]); // this should work size we added \0 at end of it
    strncpy(our_msg->data, buf + start_index, our_msg->size);
    our_msg->data[our_msg->size] = '\0';
    
    printf("\n type:%d\n", our_msg->type);
    printf("\n size:%d\n", our_msg->size);
    printf("\n source:%s\n", our_msg->source);
    printf("\n data:%s\n", our_msg->data);
    
    for (int i=0; i<num_colons; i++){
        free(fileInfo[i]);
    }
    return our_msg;
}



char* packet_to_string (message *msg){
    char*converted_str = malloc (PACKET_LEN* sizeof(char));
    //char converted_str[PACKET_LEN];
    converted_str[PACKET_LEN-1] = '\0'; 
    //char* null_char = "\0";
    //strcat(converted_str, null_char);
    
    size_send = 2*sizeof(unsigned int) + (strlen(msg->source)+1)*sizeof(char) + msg->size + 3*sizeof(char);
    printf("\nIn pack to str\n");
    //char converted_str[5000];
    //sprintf(converted_str, "%d:%d:%s:%s", msg->type, msg->size, msg->source, msg->data); //don't think this can be used because of char array, need to deal with seperately
    printf("\nb4 sprintf\n");
    printf("\nsource:%send\n", msg->source);
    int len = sprintf(converted_str, "%d:%d:%s:", msg->type, msg->size, msg->source);
    printf("\nAfter sprintf\n");
    printf("\nlen: %d", len);
    //not sure if will have issue with character arithmetic since its char* and not char[]
    memcpy(converted_str + len, msg->data, msg->size);
    printf("\nString sent is: %s\n", converted_str);
    return converted_str;
//    
//    printf("\nmtype: %d\n", msg->type);
//    printf("msize: %d\n", msg->size);
//    printf("msource: %s\n", msg->source);
//    printf("mdata: %s\n\n", msg->data);
//    
//    size_send = 2*sizeof(unsigned int) + (strlen(msg->source)+1)*sizeof(char) + msg->size + 3*sizeof(char);
//     
//    unsigned int diff = 0;
//    
//    char*converted_str = malloc (size_send);
//    memset(converted_str, 0, size_send);
//    
//    printf("\nstr1: %s\n", converted_str);
//    memcpy(converted_str, (char*)&(msg->type), sizeof(unsigned int));
//    diff += sizeof(unsigned int);
//    diff++; //to add the colon
//    converted_str[diff] = ':';
//    
//    printf("str2: %s\n", converted_str);
//    memcpy(converted_str + diff, (char*)&(msg->size), sizeof(unsigned int));
//    diff += sizeof(unsigned int);
//    diff++; //to add the colon
//    converted_str[diff] = ':';
//    
//    printf("str3: %s\n", converted_str);
//    memcpy(converted_str + diff, (char*)&(msg->source), strlen(msg->source)+1);
//    diff += strlen(msg->source)+1;
//    diff++; //to add the colon
//    converted_str[diff] = ':';
//    
//    printf("str4: %s\n", converted_str);
//    memcpy(converted_str + diff, msg->data, msg->size);
//    
//    printf("str5: %s\n\n", converted_str);
//    
//    return converted_str;
    
    
    
    
}


message* create_packet(int type, int len, char* source, char* data){
    message*our_msg = malloc(sizeof(message));
    
    //clearing out the malloced space
    strcpy(our_msg->data, "");
    strcpy(our_msg->source, "");
    
    strcpy(our_msg->data, data);
    our_msg->size = len;
    strcpy(our_msg->source, source);
    our_msg->type = type;
    
    printf("\ntypeCP:%dend\n", our_msg->type);
    printf("\nsizeCP:%dend\n", our_msg->size);
    printf("\nsourceCP:%send\n", our_msg->source);
    printf("\ndataCP:%send\n", our_msg->data);
   
    
    return our_msg;
    
    
}




int respond_to_client (int sockfd, int msg_type, char* username, char* msg_buf){
    // create message packet
    printf("\nstrlen(msg_buf):%d\n", strlen(msg_buf));
    message* msg_sent = create_packet(msg_type, strlen(msg_buf), username, msg_buf);
    // serialize the packet- sweni will write
    char* msg_string = packet_to_string(msg_sent);
    printf("\nprinting buf that i send to client:%s\n", msg_string);
    printf("\nsize_send:%d\n", size_send);
    int bytes_sent = send(sockfd, msg_string, size_send, 0);
    if (bytes_sent == -1){
        printf("error in respond to client\n");
        return (1);
    }
    
    free(msg_sent);
    free(msg_string);
}

session* search_session(session* list, char* sessionID){
    assert(sessionID);
    printf("\nin search session:%s\n", sessionID);
    if (list == NULL){
        printf("\nList is null\n");
        return NULL;
    }
    session* curr = list;
    while(curr != NULL){
        printf("Node:%s, num users:%d\n", curr->sessionID, curr->num_users);
        if (strcmp(curr->sessionID, sessionID) == 0){
            // found session
            printf("\nfound session in search\n");
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

void remove_from_session(session* main_sess, char* id){
    assert(main_sess);
    assert(id);
    
    user_node* head = main_sess->users;
    user_node* curr = NULL;
    user_node* previous = head;
    
    // iterate the sessions and find matchig client id in list of users joined
    if (strcmp(head->username, id) == 0){
        // if we need to remove head edge case
        curr = head->next;
        free(head);
        main_sess->users = curr;
    }
    else{
        while(previous->next != NULL && (strcmp(previous->next->username, id) != 0)){
            previous = previous->next;
        }
        if(previous->next == NULL){
            printf("User id %s not found in session %s\n", id, main_sess->sessionID);
        }
        curr = previous->next;
        previous->next = previous->next->next;
        free(curr);
    }
    main_sess->num_users--;
}

void delete_sess(session **list, char *id){
    assert(*list);
    assert(id);
    
    session* head = *list;
    session* current = NULL;
    session* previous = head;
    
    // find the session
    if ((strcmp(head->sessionID, id) == 0)){
        // if to be deleted is head
        if (head->num_users == 0){
            // delete this session
            current = head->next;
            free(head);
            (*list) = current;
        }
    }
    else{
        while((previous->next != NULL) && (strcmp(previous->next->sessionID, id) != 0 )){
            previous = previous->next;
        }
        if (previous->next == NULL){
            printf("Didn't find the session id %s in list\n", id);
        }
        current = previous->next;
        if (current->num_users == 0){
            previous->next = previous->next->next;
            free(current);
        }
            
    }
    
}


void exit_user(int j, char* src){
    // reset some parameters
    user_list[j].logged_on = 0;
    strcpy(user_list[j].ip_add, "");
    // if currently in a session, remove them
    if (user_list[j].connected_session){
        session* curr_sess = search_session(sessions_list, user_list[j].sessionID);
        char* sessionID = (char*)malloc(strlen(user_list[j].sessionID+1)*sizeof(char));
        strcpy(sessionID, user_list[j].sessionID);
        remove_from_session(curr_sess, src);
        user_list[j].connected_session = 0;
        strcpy(user_list[j].sessionID, "");
        delete_sess(&sessions_list, sessionID);
        free(sessionID);
    }
}


user_node* add_new_user(char* user_id){
    assert(user_id);
    user_node* new_user = malloc(sizeof(user_node));
    strcpy(new_user->username, user_id);
    new_user->next = NULL;
    return new_user;
}



void add_to_sess(session* sess, char* user_id){
    assert(sess);
    assert(user_id);
    
    // point to head of the clients list in that session
    user_node* current = sess->users;
    if (current == NULL){
        // session is empty
        sess->users = add_new_user(user_id);
        
    }
    else{
        // traverse and add at tail
        while(current->next != NULL){
            current = current->next;
        }
        current->next = add_new_user(user_id);
    }
    sess->num_users++;
}

void set_sess_id(int j, char* sess_id){
    assert(sess_id);
    user_list[j].connected_session = 1;
    strcpy(user_list[j].sessionID, sess_id);
}

void join_sess(int j, char* sessionID, char* source, int sockfd){
    assert(sessionID);
    assert(source);
    
    
    // check if they are already in a session first
    if (user_list[j].connected_session == 1){
        // nack
        respond_to_client(sockfd, JN_NAK, "server",  user_already_in_sess);
        printf("User %s could not join session %s\n", user_list[j].username, sessionID);
    }
    // they are not part of a session so add them to desired session if it exists
    else{
        // check if session exists
        session* found_sess = search_session(sessions_list, sessionID);
        if (found_sess == NULL){
            // session was not found so nack
            respond_to_client(sockfd, JN_NAK, "server",  session_not_found);
            printf("Session %s not found\n", sessionID);
        }
        else{
            // session exists, add them and ack
            respond_to_client(sockfd, JN_ACK, "server",  sessionID);
            printf("User joined session %s\n", sessionID);
            // add user to session
            add_to_sess(found_sess, source);
            set_sess_id(j, sessionID);

        }
    }

}


void leave_sess(int j, char* src){
    // if currently in a session, remove them
    if (user_list[j].connected_session){
        printf("\nIn leave_sess\n");
        session* curr_sess = search_session(sessions_list, user_list[j].sessionID);
        printf("\nCurrent session returned:%s\n", curr_sess->sessionID);
        //char* sessionID = (char*)malloc(strlen(user_list[j].sessionID)*sizeof(char));
        char sessionID[100] = {0};
        strcpy(sessionID, user_list[j].sessionID);
        printf("\nSession Id in leave_sess:%s", sessionID);
        remove_from_session(curr_sess, src);
        user_list[j].connected_session = 0;
        strcpy(user_list[j].sessionID, "");
        delete_sess(&sessions_list, sessionID);
        //free(sessionID);
    }
}

session* create_session(session** sessions_list, char* sessionID){
    assert(sessionID);
    printf("\nSession ID in create_sess:%s\n", sessionID);
    session *current = NULL;
    if ((*sessions_list) == NULL){
        *sessions_list = malloc(sizeof(session));
        current = *sessions_list;
    }
    else{
        // get last session
        current = *sessions_list;
        while(current->next != NULL){
            current = current->next;
        }
        current->next = malloc(sizeof(session));
        current = current->next;
    }
    printf("curren->sessionID b4 is: %s", current->sessionID);
    strcpy(current->sessionID, sessionID);
    printf("curren->sessionID after set is: %s", current->sessionID);

    current->num_users = 0;
    current->users = NULL;
    current->next = NULL;
    return current;
}

void new_session(int j, char* sessionID, char* source, int sockfd){
    printf("\nuser_list[j].connected_session:%d\n", user_list[j].connected_session);
    printf("\nj:%d\n", j);
    if (search_session(sessions_list, sessionID) != NULL){
        // session exists already so nak
        respond_to_client(sockfd, NS_NAK, "server", session_already_exists);
        printf("Session with the id %s already exists\n", sessionID);
    }
    else if (user_list[j].connected_session){
        // they are already in a session so cannot create new one, nak
        // remove from existing session
        printf("\ncalling leave session\n");
        leave_sess(j, source);
        // create new sess 
        session* session = create_session(&sessions_list, sessionID);
        // add client to the session - similar code to join
        add_to_sess(session, source);
        set_sess_id(j, sessionID);
        respond_to_client(sockfd, NS_ACK, "server", "");
        printf("User %s has created and joined %s\n",user_list[j].username, sessionID);
    }
    else{
        // all is well, create the session and move them in it
        session* session = create_session(&sessions_list, sessionID);
        // add client to the session - similar code to join
        add_to_sess(session, source);
        set_sess_id(j, sessionID);
        respond_to_client(sockfd, NS_ACK, "server", "");
        printf("User %s has created and joined %s\n",user_list[j].username, sessionID);
    }
}


char* get_user_db(){
    char* db = malloc(sizeof(char)*DATA_LEN);
    strcpy(db, "");
    for (int i=0; i<NUM_USERS; i++){
        if (user_list[i].logged_on){
            strcat(db, user_list[i].username);
            if (user_list[i].connected_session){
                strcat(db, ": ");
                strcat(db, user_list[i].sessionID);
            }
            strcat(db, "\n");
        }
    }
    return db;
}

int find_sock(char* user_id){
    for (int j = 0; j<NUM_USERS; j++){
        if (strcmp(user_list[j].username, user_id) == 0){
            return user_list[j].sockfd;
        }
    }
    return -1;
}

void recieve_message_from_server(){
    //write the code to receive a message from server
    //call receive and idk what else - read lab and figure out what will be sent to you and if theres different cases that need to be dealt with.
    
    
    char buf[PACKET_LEN];
    int num_bytes_recvd = recv(sockfd_client, buf, sizeof(buf), 0);

    message*msg_recvd = string_to_packet(buf, num_bytes_recvd);
    
    //not sure what messages server can send to client - check this 
}

int connect_client(char IP_addr[], char port_number_str[]){
    printf("\nIn connect client\n");
    // from Beej's guide
    struct addrinfo hints, *res, *p;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    printf("\npoint A");
    int rv;
    if ((rv = getaddrinfo(IP_addr, port_number_str, &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    printf("\npoint B");

    // loop through all the results and connect to the first we can
    for (p = res; p!=NULL; p = p->ai_next){

        if ((sockfd_client = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            perror("client: socket");
            continue;
        }

        if (connect(sockfd_client, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd_client);
            perror("client: connect");
            continue;
        }
        break;
    }

    if (p == NULL){
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    printf("\n leaving connect client");
    
}

void login_process(char username[], char password[]){
    printf("\nIn login process\n");
    //now processing info that can be used to create packet
    //for login the data is assumed to be: username (space) password, so the len has a +1 for the space
//    int len = strlen(username) + strlen(password) + 1;
    int len = strlen(username) + strlen(password) + 2;
    
    printf("\nFound len\n");
    printf("\nLen is: %d\n", len);
    
    //char data[len];
    char* data = malloc (len * sizeof(char));
    
    //strcpy(data, " ");
    strcpy(data, "");
    printf("\ncopied empty string in data\n", len);

    printf("\n point A LP\n");
    //strcat will concatenate username + data and store it in username 
    
//    //username_cpy added so that original username does not get modified
//    char username_cpy[USERNAME_LEN];
//    strcpy(username_cpy, username);
//    //strcat(username, data);
//    strcat(username_cpy, data);
//    strcpy(data, username_cpy); //doing this so final result can be stored in data later
//    strcat(data, password);
    
    strcat(data, username);
    strcat(data, " ");
    strcat(data, password);

    printf("\n point B LP\n");
    //create the packet
    message*msg = create_packet(LOGIN, len, username, data);
    
    printf("\n point C LP\n");
    char*conv_str = packet_to_string(msg);
    //int flags = 0;
    printf("\n conv_str is: %s\n", conv_str);
    
//    char conv_str_send[PACKET_LEN];
//    strcpy(conv_str_send, conv_str);
    //strcat(conv_str_send, '\0');
    
    printf("Len of conv is sent to server is: %d",  strlen(conv_str));
    //int len_conv_str = 2*sizeof(unsigned int) + (strlen(msg->source)+1)*sizeof(char) + msg->size + 3*sizeof(char);
    
//    send(sockfd_client, conv_str, strlen(conv_str), flags);
    //send(sockfd_client, conv_str, len_conv_str, flags);
    
    send(sockfd_client, conv_str, size_send, 0);
    
    //reset size_send
    size_send = 0;
    
    free(data);
    free(conv_str);
    //free(msg);
    
    printf("\n point D LP\n");
    //recv from server
    char buf[PACKET_LEN] = {0};
    //buf[PACKET_LEN -1] = '\0';
    
    printf("\n b4 recv from server\n");
    int num_bytes_recvd = recv(sockfd_client, buf, sizeof(buf), 0);

    printf("\n after recv from server\n");
    printf("\n num recvd: %d\n", num_bytes_recvd);
    
    printf("\nbuf client: %s\n", buf);
    
    
    message*msg_recvd = string_to_packet(buf, num_bytes_recvd);
    
    
    
    /*DOES THE ABOVE NEED TO BE FREED CHECK!!!!!!!*/
    
    printf("\nafter conv string to pack\n");
    
    if (msg_recvd->type == LO_NAK){
        printf("Login unsuccessful: %s\n", msg_recvd->data);
        isConnected = 0;
    }else if (msg_recvd->type == LO_ACK){
        printf("Login successful!\n");
        isConnected = 1;
    }else{
        printf("Invalid message received!\n");
    }

    free(msg_recvd);
    //not sure if I need to do this
    
}