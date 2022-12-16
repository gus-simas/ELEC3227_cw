/*
 * NET.c
 *
 *  Created on: 9 Nov 2022
 *      Author: gusta
 */

#include "NET.h"
#include "uart.h"


void NET_receive_TRAN(Network *packet, uint8_t TRAN_Segment[], uint8_t seg_len, uint8_t DEST)
{
	uint8_t i;
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


    //packet->sPacket = (uint8_t *)malloc(packet->packet_len * sizeof(uint8_t));

    /* CALCULATE ROUTE */
    packet->broadpack[0] = 0xFF;
    packet->broadpack[1] = 0x00;
    packet->broadpack[2] = LOCAL_ADDRESS;
    packet->broadpack[3] = 0xFF;
    packet->broadpack[4] = 2;
    for(i = 0; i < 2; i++){
        packet->broadpack[5+i] = 0x00;
    }
    packet->broadpack[7] = 0x00;
    packet->broadpack[8] = 0x00;


	/* POPULATE SEND PACKET */
    packet->sPacket[0] = 0xFF;
    packet->sPacket[1] = CHECKSUM;
    packet->sPacket[2] = LOCAL_ADDRESS;
    packet->sPacket[3] = DEST;
    packet->sPacket[4] = seg_len;
    for(i = 0; i < seg_len; i++){
        packet->sPacket[5+i] = TRAN_Segment[i];
    }
    packet->sPacket[seg_len+5] = 0x00;
    packet->sPacket[seg_len+6] = 0x00;
	

    /* CREATE CHECKSUM */
	create_checksum(packet, seg_len, CHECKSUM);

    packet->DEST = DEST;

    packet->broadcast = true;
    packet->rebroadcast = false;
    packet->send = false;
    packet->receive = false;
	
	// if(packet->send == true)
		// put_str("\n\rTRUE2");
	// else if(packet->send == false)
		// put_str("\n\rFALSE2");
	// else
		// put_str("\n\rIDK MATE2");
	


}

void NET_receive_DLL(Network *packet, uint8_t rPacket[], uint8_t src_mac_address, uint8_t pack_len)
{
    if(rPacket[0] < HOP_LIMIT){
        /* discard data*/
		put_str("\n\rHOP LIMIT REACHED");
		return;
    }

    if(rPacket[3] == 0xFF){         // received broadcast/rebroadcast
    	//packet->table_no = 0;
        if(rPacket[1] == 0x00){     // received a broadcast
            rPacket[1] = 0x01;      // turn to rebroadcast
            rPacket[2] = LOCAL_ADDRESS /*2*/;		// 2 for testing purposes TEST_SCENARIO 2

			int i;
            for(i = 0; i < 9; i++){
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
        		if((packet->table_no == 0) && (src_mac_address == NEIGHBOUR_ADDRESS1)){
					packet->first_place = NEIGHBOUR_ADDRESS1;
					put_str("\n\rfirst place: 2");
        		}
				else if((packet->table_no == 0) && (src_mac_address == NEIGHBOUR_ADDRESS2)){
					packet->first_place = NEIGHBOUR_ADDRESS2;
					put_str("\n\rfirst place: 3");
					put_str("\n\rwill hop");
				}
        		else if((packet->table_no == 1) && (src_mac_address == NEIGHBOUR_ADDRESS1)){
        			packet->second_place = NEIGHBOUR_ADDRESS1;
					put_str("\n\rsecond place: 2\n");
        		}
				else if((packet->table_no == 1) && (src_mac_address == NEIGHBOUR_ADDRESS2)){
					packet->second_place = NEIGHBOUR_ADDRESS2;
					put_str("\n\rsecond place: 3\n");
				}
        		packet->table_no += 1;
        	}
        	else if(rPacket[2] == NEIGHBOUR_ADDRESS2){
        		if((packet->table_no == 0) && (src_mac_address == NEIGHBOUR_ADDRESS2)){
					packet->first_place = NEIGHBOUR_ADDRESS2;
					put_str("\n\rfirst place: 3");
        		}
				else if((packet->table_no == 0) && (src_mac_address == NEIGHBOUR_ADDRESS1)){
					packet->first_place = NEIGHBOUR_ADDRESS1;
					put_str("\n\rfirst place: 2");
					put_str("\n\rwill hop");
				}
        		else if((packet->table_no == 1) && (src_mac_address == NEIGHBOUR_ADDRESS2)){
        			packet->second_place = NEIGHBOUR_ADDRESS2;
					put_str("\n\rsecond place: 3\n");
        		}
				else if((packet->table_no == 1) && (src_mac_address == NEIGHBOUR_ADDRESS1)){
					packet->second_place = NEIGHBOUR_ADDRESS1;
					put_str("\n\rsecond place: 2\n");
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
		if(rPacket[3] == LOCAL_ADDRESS){        // message is meant to be here
			/* PERFORM ERROR CHECK */
			bool check = checksum(packet, rPacket, rPacket[1], pack_len);
			if(check == false){
				/* discard data */
				put_str("\n\n\r          DATA CORRUPTED -- DISCARD\n\n\r");
				return;
			}
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
			int i;
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
				put_str("\n\n\rSHOULD NOT BE HERE");
				return;
			}
			else{                                  // this is a hop

				/* set next dest_mac_address */
				if(rPacket[2] == NEIGHBOUR_ADDRESS1){
					packet->dest_mac_address = NEIGHBOUR_ADDRESS2;
				}
				else if(rPacket[2] == NEIGHBOUR_ADDRESS2){
					packet->dest_mac_address = NEIGHBOUR_ADDRESS1;
				}
				
				rPacket[0] -= 1;


				packet->broadcast = false;
				packet->rebroadcast = false;
				packet->send = true;
				packet->receive = false;
			}
		}
    }
}

void create_checksum(Network *packet, uint8_t seg_len, uint8_t check)
{
	if(check == 0){
		uint8_t i, j, temp = 0, count = 0;
		
		/*for(i = 0; i < (seg_len + 7); i++){
			temp = packet->sPacket[i];
			if(temp & 0xFF) count++;
		}*/

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
		

	}
	else if(check == 2){
		int temp;
		
		temp = packet->sPacket[0] ^ packet->sPacket[3];
		packet->sPacket[seg_len+5] = temp;
		temp = packet->sPacket[2] ^ packet->sPacket[4];
		packet->sPacket[seg_len+6] = temp;
	}
}

bool checksum(Network *packet, uint8_t rPacket[], uint8_t check, uint8_t pack_len)
{
	if(check == 0){
		uint8_t i, j, temp = 0, count = 0;

		for(i = 0; i < pack_len; i++){
			//put_str("\n\rCOUNT HERE");
			temp = rPacket[i];
			for(j = 0; j < sizeof(rPacket[i]); j++){
				if(temp & 0x01) count++;
				temp>>1;
			}
		}
		if(count%2 != 0){
			/* ERROR */
			/* discard data */
			return false;
		}
		else{
			put_str("\n\n\r            NET CHECKSUM PASSED\n\n\r");
			return true;
		}
	}
	else if(check == 2){
		uint8_t temp, temp2;
		
		temp = rPacket[0] ^ rPacket[3];
		temp2 = rPacket[2] ^ rPacket[4];
		
		if((temp == rPacket[pack_len-2]) && (temp2 == rPacket[pack_len-1])){
			put_str("\n\n\r            NET CHECKSUM PASSED\n\n\r");
			return true;
		}
		else{
			return false;
		}
		/* uint8_t temp, temp2, count;
		int i;
		
		temp = rPacket[0];
		temp2 = rPacket[3];
		
		for(i = 0; i < 8; i++){
			temp>>=i;
			temp2>>=2;
			
			if(i == 0){
				if(temp ^ temp2){
					count = 1;
				}
				else{
					count = 0;
				}
			}
			else{
				count<<=1;
				if(temp ^ temp2){
					count++;
				}
			}
		}
		
		uint8_t comp1 = count;
		
		temp = packet->sPacket[2];
		temp2 = packet->sPacket[4];
		count = 0;
		
		for(i = 0; i < 8; i++){
			temp>>=i;
			temp2>>=2;
			
			if(i == 0){
				if(temp ^ temp2){
					count = 1;
				}
				else{
					count = 0;
				}
			}
			else{
				count<<=1;
				if(temp ^ temp2){
					count++;
				}
			}
		}
		
		uint8_t comp2 = count;
		
		if((comp1 == rPacket[pack_len-2]) && (comp2 == rPacket[pack_len-1])){
			return true;
		}
		else{
			return false; 
		}*/
		/* uint8_t temp, temp2;
		
		temp = rPacket[0] ^ rPacket[3];
		temp2 = rPacket[2] ^ rPacket[4];
		
		if((temp == rPacket[pack_len-2]) && (temp2 == rPacket[pack_len-1])){
			return true;
		}
		else{
			return false;
		} */
		/*uint8_t i, j, temp = 0, temp2 = 0, count = 0;
		
		for(j = 7; j >= 0; j--){
			for(i = 1; i < 5; i++){
				temp = rPacket[i];
				temp<<=j;
				if(temp & 0x01) count++;
			}
			if(j == 7){
				if(count%2 != 0){
					temp2 = 1;
				}
			}
			else if(j != 7){
				temp2<<=1;
				if(count%2 != 0){
					temp2++;
				}
			}
		}
		if(temp2 != rPacket[pack_len-1]){
			/* ERROR *
			/* discard data *
			return false;
		}
		else{
			return true;
		}*/
	}
}