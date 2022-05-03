/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   packet.h
 * Author: dasadias
 *
 * Created on February 6, 2022, 2:04 PM
 */

#ifndef PACKET_H
#define PACKET_H

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* PACKET_H */



typedef struct pack{
    unsigned int total_frag;
    unsigned int frag_no;
    unsigned int size;
    char* filename;
    char fileData[1000];
}Packet;