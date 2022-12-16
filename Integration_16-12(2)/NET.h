/*
 * NET.h
 *
 *  Created on: 9 Nov 2022
 *      Author: gusta
 */

#ifndef NET_H_
#define NET_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define LOCAL_ADDRESS 1
#define NEIGHBOUR_ADDRESS1 0
#define NEIGHBOUR_ADDRESS2 2
#define HOP_LIMIT 0xFA
#define CHECKSUM 0

typedef struct
{
	/*uint8_t control[2];                 // control[0] is the hop counter
      uint8_t src_address;
      uint8_t dest_address;
      uint8_t len;                        // length of the TRAN Segment
      uint8_t TRAN_Segment[];
      uint8_t checksum[2];*/

      uint8_t DEST;

      uint8_t TRAN_Segment[15];

      uint8_t seg_length;

      uint8_t src_mac_address;
	  uint8_t dest_mac_address;

      uint8_t broadpack[8];

      uint8_t packet_len;

      bool broadcast;
      bool rebroadcast;
      bool send;
      bool receive;

      uint8_t first_place;
      uint8_t second_place;
      uint8_t table_no;

      uint8_t sPacket[22];

} Network;

void NET_receive_TRAN(Network *packet, uint8_t TRAN_Segment[], uint8_t seg_len, uint8_t DEST);

void NET_receive_DLL(Network *packet, uint8_t rPacket[], uint8_t src_mac_address, uint8_t pack_len);

void create_checksum(Network *packet, uint8_t seg_len, uint8_t check);

bool checksum(Network *packet, uint8_t rPacket[], uint8_t check, uint8_t pack_len);

#endif /* NET_H_ */
