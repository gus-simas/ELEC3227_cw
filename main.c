
#include "NET.h"
#include "stdbool.h"
#include "uart.h"

#define TEST_SCENARIO 1



Network pack;

uint8_t TRAN_Segment[9] = {0xFF, 0x00, 0x01, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00};
uint8_t Packet_SC1[16] = {0xFF, 0x00, 0x01, 0x02, 0x09, 0xFF, 0x00, 0x01, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00};
uint8_t BROADCAST_PACKET[9] = {0xFF, 0x00, 0x02, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00};
uint8_t REBROADCAST_PACKET1[9] = {0xFE, 0x01, 0x03, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00};
uint8_t REBROADCAST_PACKET2[9] = {0xFE, 0x01, 0x03, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00};
uint8_t HOP_PACKET[16] = {0xFF, 0x00, 0x02, 0x03, 0x09, 0xFF, 0x00, 0x01, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00};
uint8_t AT_HOME[16] = {0xFE, 0x00, 0x02, 0x01, 0x09, 0xFF, 0x00, 0x01, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00};




// int main()
// {
	// init_uart0();
	// put_str("HEEEEEEEY");
	
	// while(1)
	// {
		// if(TEST_SCENARIO == 1){
			// NET_receive_TRAN(&pack, TRAN_Segment, 9, 2);
			// if(pack.broadcast){
				// put_str("\n\n\rBROADCASTING");
				// if(pack.broadpack[0] == 0xFF)
					// if(pack.broadpack[1] == 0x00)
						// if(pack.broadpack[2] == 1)
							// if(pack.broadpack[3] == 0xFF)
								// if(pack.broadpack[4] = 2)
									// if((pack.broadpack[5] && pack.broadpack[6]) == 0x00)
										// if((pack.broadpack[7] && pack.broadpack[8]) == 0x00)
											// put_str("\n\rCORRECT");
			// }
			// put_str("\n\n\rDATA SAVED: ");
			
			// int i;
			// for(i = 0; i < 15; i++){
				// if(pack.sPacket[i] == Packet_SC1[i])
					// put_str("\n\rCOUNT HERE");
			// }

			// if(pack.sPacket[15] == 0x01)
				// put_str("\n\rCHECKSUM = 0x01");
			// else if(pack.sPacket[15] == 0x00)
				// put_str("\n\rCHECKSUM = 0x00");
		// }
		
		// else if(TEST_SCENARIO == 2){		// change rPacket[2] in NET_receive_DLL()
			// NET_receive_DLL(&pack, BROADCAST_PACKET, 2, 9);
			// if(pack.rebroadcast){
				// put_str("\n\n\rRECEIVED BROADPACK    REBROADCASTING");
				// if(pack.broadpack[0] == 0xFE)
					// if(pack.broadpack[1] == 0x01)
						// if(pack.broadpack[2] == 0x02)
							// if(pack.broadpack[3] == 0xFF)
								// if(pack.broadpack[4] = 2)
									// if((pack.broadpack[5] && pack.broadpack[6]) == 0x00)
										// if((pack.broadpack[7] && pack.broadpack[8]) == 0x00)
											// put_str("\n\rCORRECT");
										
			// }
		// }
		
		// else if(TEST_SCENARIO == 3){
			// NET_receive_TRAN(&pack, TRAN_Segment, 9, 2);
			// NET_receive_DLL(&pack, REBROADCAST_PACKET1, 2, 9);
			// NET_receive_DLL(&pack, REBROADCAST_PACKET2, 3, 9);
			// if(pack.send){
				// put_str("\n\rRECEIVED REBROADCAST   TABLE UPDATED     \n\rSENDING\n\n\n");
			// }
		// }
		// else if(TEST_SCENARIO == 4){
			// NET_receive_DLL(&pack, HOP_PACKET, 2, 16);
			// if(pack.send){
				// put_str("\n\n\rRECEIVED A HOP");
			// }
		// }
		// else if(TEST_SCENARIO == 5){
			// NET_receive_DLL(&pack, AT_HOME, 2, 16);
			// if(pack.receive){
				// put_str("\n\n\rRECEIVING DATA    PASSING TO TRAN");
			// }
		// }
		// else if(TEST_SCENARIO == 6){
			// NET_receive_TRAN(&pack, TRAN_Segment, 9, 2);
			// if(pack.broadcast){
				// put_str("\n\n\rBROADCASTING");
				// if(pack.broadpack[0] == 0xFF)
					// if(pack.broadpack[1] == 0x00)
						// if(pack.broadpack[2] == 1)
							// if(pack.broadpack[3] == 0xFF)
								// if(pack.broadpack[4] = 2)
									// if((pack.broadpack[5] && pack.broadpack[6]) == 0x00)
										// if((pack.broadpack[7] && pack.broadpack[8]) == 0x00)
											// put_str("\n\rCORRECT");
			// }
			// put_str("\n\n\rDATA SAVED: ");
			
			// int i;
			// for(i = 0; i < 15; i++){
				// if(pack.sPacket[i] == Packet_SC1[i])
					// put_str("\n\rCOUNT HERE");
			// }
			
			// if(pack.sPacket[15]%2 == 0)
				// put_str("\n\rCHECKSUM = ODD");
			// if(pack.sPacket[15] == 52)
				// put_str("\n\rCHECKSUM = 0x0B");
		// }
		
	// }
// }

void main()
{
	init_uart0();
	put_str("Initiating\n\r");
	
	while(1)
	{
		/* APP INPUTS */
		if(/*TRAN transmit flag*/){
			NET_receive_TRAN(&pack, tran.TRAN_Segment, tran.seg_len, tran.DEST);
			/*TRAN transmit flag = false */
		}
		else{
			/* APP OUTPUTS */
		}
		if(pack.broadcast){
			//DLL_receive(&dll, pack.broadpack, pack.packet_len, 0xFF);
			put_str("\n\rBROADCASTING");
			pack.broadcast = false;
		}
		else if(pack.rebroadcast){
			//DLL_receive(&dll, pack.broadpack, pack.packet_len, 0xFF);
			put_str("\n\rREBROADCASTING");
			pack.rebroadcast = false;
		}
		else if(pack.send){
			//DLL_receive(&dll, pack.sPacket, pack.packet_len, dest_mac_address);
			put_str("\n\rSENDING DATA");
			pack.send = false;
		}
		else if(pack.receive){
			//TRAN_receive(&tran, pack.TRAN_Segment, pack.seg_length);
			put_str("\n\rRECEIVING");
			pack.receive = false;
		}
	}
}

/*
if(trans->TL_transmit){
            NET_receive_TRAN(&pack, tran->TRAN_Segment, len, DEST);
            trans->TL_transmit = false;
        }
        else{
            /* APP OUTPUTS 
        }
        if(pack.broadcast){
            DLL_receive(&dll, pack.broadpack, pack.packet_len, 0xFF);
			//printf("BROADCASTING");
        }
        else if(pack.rebroadcast){
            DLL_receive(&dll, pack.broadpack, pack.packet_len, 0xFF);
        }
        else if(pack.send){
            DLL_receive(&dll, pack.sPacket, pack.packet_len, pack.dest_mac_address);
			//printf("SENDING DATA");
        }
        else if(pack.receive){
            TRAN_receive(&tran, pack.TRAN_Segment, pack.seg_length);
        }*/