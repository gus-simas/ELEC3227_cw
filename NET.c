/*
 * NET.c
 *
 *  Created on: 9 Nov 2022
 *      Author: gusta
 */

#include "NET.h"

void NET_receive_TRAN(Network *packet, uint8_t TRAN_Segment[], uint8_t seg_len, uint8_t DEST)
{
	uint8_t i;
	memset(&packet, 0, sizeof(packet));
	packet->table_no = 0;

    switch (seg_len)
    {
        case 8:
            packet->packet_len = 15;
            break;
        case 9:
            packet->packet_len = 16;
            break;
        case 10:
            packet->packet_len = 17;
            break;
        case 11:
            packet->packet_len = 18;
            break;
    }

    printf("Segment length: %u\n", seg_len);
    printf("Packet length: %u\n\n", packet->packet_len);

    packet->sPacket = (uint8_t *)calloc(packet->packet_len, sizeof(uint8_t));

    /* CALCULATE ROUTE */
    packet->broadpack[0] = 0xFF;
    packet->broadpack[1] = 0x00;
    packet->broadpack[2] = LOCAL_ADDRESS;
    packet->broadpack[3] = 0xFF;
    packet->broadpack[4] = 4;
    for(i = 0; i < 2; i++){
        packet->broadpack[5+i] = 0x00;
    }
    packet->broadpack[seg_len+5] = 0x00;
    packet->broadpack[seg_len+6] = 0x00;



    packet->sPacket[0] = 0xFF;
    packet->sPacket[1] = CHECKSUM1;
    packet->sPacket[2] = LOCAL_ADDRESS;
    packet->sPacket[3] = DEST;
    packet->sPacket[4] = seg_len;
    for(i = 0; i < seg_len; i++){
        packet->sPacket[5+i] = TRAN_Segment[i];
    }
    packet->sPacket[seg_len+5] = 0x00;
    packet->sPacket[seg_len+6] = 0x00;

    /* CREATE CHECKSUM */
    uint8_t i, j, temp = 0, count = 0;

    for(i = 0; i < (seg_len + 7); i++){
    	temp = packet->sPacket[i];
    	for(j = 0; j < sizeof(temp); j++){
    		if(temp & 0x01) count++;
    		temp >>= 1;
    	}
    }
    if(count%2 != 0){
    	packet->sPacket[seg_len+6] = 0x01;		// parity bit added
    }

    packet->DEST = DEST;

    packet->broadcast = true;
    packet->rebroadcast = false;
    packet->send = false;
    packet->receive = false;

    /* define dest_mac_address for DLL use */

}

void NET_receive_DLL(Network *packet, uint8_t rPacket[], uint8_t src_mac_address, uint8_t pack_len)
{
    if(rPacket[0] < HOP_LIMIT){
        /* discard data*/
    }

    if(rPacket[3] == 0xFF){         // received broadcast/rebroadcast
    	packet->table_no = 0;
        if(rPacket[1] == 0x00){     // received a broadcast
            rPacket[1] = 0x01;      // turn to rebroadcast
            rPacket[2] = LOCAL_ADDRESS;


            for(int i = 0; i < 9; i++){
                packet->broadpack[i] = rPacket[i];
            }

            packet->broadpack[0] -= 1;

            packet->broadcast = false;
            packet->rebroadcast = true;
            packet->send = false;
            packet->receive = false;
        }
        else if(rPacket[1] == 0x01){        // received a rebroadcast

            /* UPDATE TABLE */
        	if(rPacket[2] == NEIGHBOUR_ADDRESS1){
        		if(packet->table_no == 0){
					packet->first_place = NEIGHBOUR_ADDRESS1;
        		}
        		else if(packet->table_no == 1){
        			packet->second_place = NEIGHBOUR_ADDRESS1;
        		}
        		packet->table_no += 1;
        	}
        	else if(rPacket[2] == NEIGHBOUR_ADDRESS2){
        		if(packet->table_no == 0){
					packet->first_place = NEIGHBOUR_ADDRESS2;
        		}
        		else if(packet->table_no == 1){
        			packet->second_place = NEIGHBOUR_ADDRESS2;
        		}
        		packet->table_no += 1;
        	}

                /* set dest_mac_address */
        	packet->dest_mac_address = packet->first_place;

            /* SEND DATA */
            if(rPacket[2] == packet->DEST){     // DEST could be replaced with sPacket[3]
                packet->broadcast = false;
                packet->rebroadcast = false;
                packet->send = true;
                packet->receive = false;
            }
        }
    }
    else{
        /* PERFORM ERROR CHECK */
    	uint8_t i, temp = 0, count = 0;

    	for(i = 0; i < sizeof(rPacket); i++){
    		temp = rPacket[i];
    		for(int j = 0; j < sizeof(rPacket[i]); j++){
    			if(temp & 0x01) count++;
    			temp >>= 1;
    		}
    	}
    	if(count%2 != 0){
    		/* ERROR */
    		/* discard data */
    	}
    	else{

			if(rPacket[3] == LOCAL_ADDRESS){        // message is meant to be here
				/* PASS TO TRAN */
				switch(pack_len)
				{
					case 15:
						packet->seg_length = 8;
						break;
					case 16:
						packet->seg_length = 9;
						break;
					case 17:
						packet->seg_length = 10;
						break;
					case 18:
						packet->seg_length = 11;
						break;
				}
				//packet->TRAN_Segment = (uint8_t *)calloc(packet->seg_length, sizeof(uint8_t));
				for(i = 0; i < packet->seg_length; i++){
					packet->TRAN_Segment[i] = rPacket[5+i];
				}
				packet->broadcast = false;
				packet->rebroadcast = false;
				packet->send = false;
				packet->receive = true;
			}
			else{
				if(rPacket[2] == LOCAL_ADDRESS){       // data shouldn't be here
					/* DISCARD DATA */
				}
				else{                                  // this is a hop

					/* set next dest_mac_address */
					if(rPacket[2] == NEIGHBOUR_ADDRESS1){
						packet->dest_mac_address = NEIGHBOUR_ADDRESS2;
					}
					else if(rPacket[2] == NEIGHBOUR_ADDRESS2){
						packet->dest_mac_address = NEIGHBOUR_ADDRESS1;
					}


					packet->broadcast = false;
					packet->rebroadcast = false;
					packet->send = true;
					packet->receive = false;
				}
			}
    	}
    }
}
