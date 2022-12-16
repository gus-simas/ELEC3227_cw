

#include "NET.h"
#include "stdbool.h"
#include "uart.h"
#include "header.h"
#include "hal.h"
#include "transport_layer.h"
#include "Data_Link_layer.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>

#define TEST_SCENARIO 1
#define TEST_TRANSPORT 1
#define DEVICE_NUMBER 1


static Network pack;
static transport_queue trans_queue;
static application app;

uint8_t TRAN_Segment[9] = {0xFF, 0x00, 0x01, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00};
uint8_t Packet_SC1[16] = {0xFF, 0x00, 0x01, 0x02, 0x09, 0xFF, 0x00, 0x01, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00};
uint8_t BROADCAST_PACKET[9] = {0xFF, 0x00, 0x03, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00};
uint8_t REBROADCAST_PACKET1[9] = {0xFE, 0x01, 0x02, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00};
uint8_t REBROADCAST_PACKET2[9] = {0xFE, 0x01, 0x03, 0xFF, 0x02, 0x00, 0x00, 0x00, 0x00};
uint8_t HOP_PACKET[16] = {0xFF, 0x00, 0x02, 0x03, 0x09, 0xFF, 0x00, 0x01, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00};
uint8_t AT_HOME[16] = {0xFE, 0x00, 0x02, 0x01, 0x09, 0xFF, 0x00, 0x01, 0x02, 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00};



ISR (INT0_vect)
{
    app.src_port = 0; // button 0
    app.dest_port = 2;
#if (DEVICE_NUMBER == 0)
    app.dest_dev = 1;
#elif (DEVICE_NUMBER == 1)
    app.dest_dev = 0;
#elif (DEVICE_NUMBER == 2)
    app.dest_dev = 1;
#endif
    app.tx_data = read_adc();
    app.transmit_flag = 1;
}


ISR (INT1_vect)
{
    app.src_port = 1; // button 1
    app.dest_port = 2;
#if (DEVICE_NUMBER == 0)
    app.dest_dev = 2;
#elif (DEVICE_NUMBER == 1)
    app.dest_dev = 2;
#elif (DEVICE_NUMBER == 2)
    app.dest_dev = 0;
#endif
    app.tx_data = read_adc();
    app.transmit_flag = 1;
}


// int main()
// {
// 	init_uart0();
// 	put_str("HEEEEEEEY");
	
// 	while(1)
// 	{
// 		if(TEST_SCENARIO == 1){
// 			NET_receive_TRAN(&pack, TRAN_Segment, 9, 2);
// 			if(pack.broadcast){
// 				put_str("\n\n\rBROADCASTING");
// 				if(pack.broadpack[0] == 0xFF)
// 					if(pack.broadpack[1] == 0x00)
// 						if(pack.broadpack[2] == 1)
// 							if(pack.broadpack[3] == 0xFF)
// 								if(pack.broadpack[4] = 2)
// 									if((pack.broadpack[5] && pack.broadpack[6]) == 0x00)
// 										if((pack.broadpack[7] && pack.broadpack[8]) == 0x00)
// 											put_str("\n\rCORRECT");
// 			}
// 			put_str("\n\n\rDATA SAVED: ");
			
// 			int i;
// 			for(i = 0; i < 15; i++){
// 				if(pack.sPacket[i] == Packet_SC1[i])
// 					put_str("\n\rCOUNT HERE");
// 			}

// 			if(pack.sPacket[15] == 0x01)
// 				put_str("\n\rCHECKSUM = 0x01");
// 			else if(pack.sPacket[15] == 0x00)
// 				put_str("\n\rCHECKSUM = 0x00");
// 		}
		
// 		else if(TEST_SCENARIO == 2){		// change rPacket[2] in NET_receive_DLL()
// 			NET_receive_DLL(&pack, BROADCAST_PACKET, 2, 9);
// 			if(pack.rebroadcast){
// 				put_str("\n\n\rRECEIVED BROADPACK    REBROADCASTING");
// 				if(pack.broadpack[0] == 0xFE)
// 					if(pack.broadpack[1] == 0x01)
// 						if(pack.broadpack[2] == 0x02)
// 							if(pack.broadpack[3] == 0xFF)
// 								if(pack.broadpack[4] = 2)
// 									if((pack.broadpack[5] && pack.broadpack[6]) == 0x00)
// 										if((pack.broadpack[7] && pack.broadpack[8]) == 0x00)
// 											put_str("\n\rCORRECT");
										
// 			}
// 		}
		
// 		else if(TEST_SCENARIO == 3){
// 			NET_receive_TRAN(&pack, TRAN_Segment, 9, 2);
// 			NET_receive_DLL(&pack, REBROADCAST_PACKET1, 2, 9);
// 			NET_receive_DLL(&pack, REBROADCAST_PACKET2, 3, 9);
// 			if(pack.send){
// 				put_str("\n\rRECEIVED REBROADCAST   TABLE UPDATED     \n\rSENDING\n\n\n");
// 			}
// 		}
// 		else if(TEST_SCENARIO == 4){
// 			NET_receive_DLL(&pack, HOP_PACKET, 2, 16);
// 			if(pack.send){
// 				put_str("\n\n\rRECEIVED A HOP");
// 			}
// 		}
// 		else if(TEST_SCENARIO == 5){
// 			NET_receive_DLL(&pack, AT_HOME, 2, 16);
// 			if(pack.receive){
// 				put_str("\n\n\rRECEIVING DATA    PASSING TO TRAN");
// 			}
// 		}
// 		else if(TEST_SCENARIO == 6){
// 			NET_receive_TRAN(&pack, TRAN_Segment, 9, 2);
// 			if(pack.broadcast){
// 				put_str("\n\n\rBROADCASTING");
// 				if(pack.broadpack[0] == 0xFF)
// 					if(pack.broadpack[1] == 0x00)
// 						if(pack.broadpack[2] == 1)
// 							if(pack.broadpack[3] == 0xFF)
// 								if(pack.broadpack[4] = 2)
// 									if((pack.broadpack[5] && pack.broadpack[6]) == 0x00)
// 										if((pack.broadpack[7] && pack.broadpack[8]) == 0x00)
// 											put_str("\n\rCORRECT");
// 			}
// 			put_str("\n\n\rDATA SAVED: ");
			
// 			int i;
// 			for(i = 0; i < 15; i++){
// 				if(pack.sPacket[i] == Packet_SC1[i])
// 					put_str("\n\rCOUNT HERE");
// 			}
			
// 			if(pack.sPacket[15]%2 == 0)
// 				put_str("\n\rCHECKSUM = ODD");
// 			if(pack.sPacket[15] == 52)
// 				put_str("\n\rCHECKSUM = 0x0B");
// 		}
		
// 	}
// }

void main()
{
	uint8_t Data_Flag = 0, L1, L2, MAC, Packet_Len;
	uint8_t * Rec_Data;
    Rec_Data = (uint8_t*)calloc(0x1E,sizeof(uint8_t));
	
	init_uart0();
	put_str("\n\rInitiating\r\n\r\n\r\n");
	
	/* TRAN Init */
	trans_queue = create_transport_queue(20);
	device_setup();
    transport rx_trans, tx_trans;
    tl_segment rx;
	rx_trans.update_app = rx_trans.send_ack = 0;
	
	/* DLL Init */
	
	Main_Frame * Pass_Frames;
	Pass_Frames = (Main_Frame*)calloc(0x05,sizeof(Main_Frame));
	
	while(1)
	{
		/* APP INPUTS */
		if (app.transmit_flag)
		{
			put_str("\n\rAPP  :   BUTTON PRESSED\n\r");
			transport_enqueue(&trans_queue, TL_send(app));
			app.transmit_flag = false;
		}
		if(rx_trans.update_app)
		{
			put_str("\n\rAPP  :   UPDATE LED\n\r");
            set_pwm(app.rx_data);
		}
		if (!transport_queue_isempty(&trans_queue) && !read_timer_status())
		{
			put_str("\n\rTRANS:   TRANSMIT\n\r");
			tx_trans = transport_dequeue(&trans_queue);
			enable_timeout();
			NET_receive_TRAN(&pack, tx_trans.buf.buf, tx_trans.buf.len, tx_trans.dev);
			_delay_ms(10);
		} else if(tl_retransmit_flag){
			put_str("\n\rTRANS:   RE-TRANSMIT\n\r");
			increment_attempts();
			NET_receive_TRAN(&pack, tx_trans.buf.buf, tx_trans.buf.len, tx_trans.dev);
			tl_retransmit_flag = false;
		} 
		
		if (rx_trans.send_ack)
		{
			put_str("\n\rTRANS:   SEND ACK\n\r");
			NET_receive_TRAN(&pack, rx_trans.buf.buf, rx_trans.buf.len, rx_trans.dev);
			rx_trans.send_ack = false;
			increment_rxsequence();
		}
		Check_Reciever(&MAC, Rec_Data, &Packet_Len, &Data_Flag, Pass_Frames);
		
		if(pack.broadcast){
			/*DLL_receive(&dll, pack.broadpack, pack.packet_len, 0xFF);*/
			put_str("\n\rNET  :   BROADCASTING\n\r");
			LLC_Net_Interface(0xFF, pack.broadpack, 8, Pass_Frames);  			
			pack.broadcast = false;
			/*pack.sPacket[0] = 0xFE;*/
			//pack.sPacket[2] = 2;
			//pack.sPacket[3] = 1;
			//NET_receive_DLL(&pack, pack.sPacket, 2, 16);
			//NET_receive_DLL(&pack, BROADCAST_PACKET, 2, 9);
		}
		else if(pack.rebroadcast){
			/*DLL_receive(&dll, pack.broadpack, pack.packet_len, 0xFF);*/
			put_str("\n\rNET  :   REBROADCASTING\r\n");
			LLC_Net_Interface(0xFF, pack.broadpack, 8, Pass_Frames);
			pack.rebroadcast = false;
			//NET_receive_DLL(&pack, REBROADCAST_PACKET1, 2, 9);
		}
		else if(pack.send){
			/*DLL_receive(&dll, pack.sPacket, pack.packet_len, dest_mac_address);*/
			put_str("\n\rNET  :   SENDING DATA\r\n");
			LLC_Net_Interface(pack.dest_mac_address, pack.sPacket, pack.packet_len, Pass_Frames);
			pack.send = false;
			// pack.sPacket[2] = 2;
			// pack.sPacket[3] = 1;
			// NET_receive_DLL(&pack, pack.sPacket, 3, 16);
		}
		else if(Data_Flag){
			Data_Flag = 0;
			/*if(Rec_Data[3] == 2){
				Rec_Data[2] = 2;
			}
			else if(Rec_Data[3] == 0){
				Rec_Data[2] = 0;
			}
			Rec_Data[3] = 1;*/
			NET_receive_DLL(&pack, Rec_Data, MAC, Packet_Len);
			for(L1 = 0; L1<5;L1++){
				for(L2 = 0; L2<30; L2++){
					Pass_Frames[L1].Data[L2] = 0;
				}
			}
		}
		else if(pack.receive){
			put_str("\n\rNET  :   RECEIVING");
			tl_segment segment;
			segment.buf = pack.TRAN_Segment;
			segment.len = pack.seg_length;
			TL_receive(pack.DEST, &segment, &rx_trans, &app.rx_data);
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