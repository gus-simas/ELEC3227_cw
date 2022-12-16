#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>


#ifndef DATA_LINK_LAYER_H
#define DATA_LINK_LAYER_H
#define Local_MAC 0x01
#define Flag_Byte 0x7D
#define Esc_Byte 0x7E
#define Trans_P 0x05


typedef struct Frame_Struct{
	uint8_t Header;
	uint8_t Control[2];
	uint8_t Source_MAC;
	uint8_t Dest_MAC;
	uint8_t Length;
	uint8_t NET_Packet[8];
	uint8_t checksum[2];
	uint8_t Footer;
} Frame_Struct;

typedef struct Main_Frame{
	uint8_t Data[30];
	uint8_t Size;
} Main_Frame;

void LLC_Net_Interface(uint8_t Dest_MAC, uint8_t Packet_Data[], uint8_t Net_Leng, Main_Frame * Pass_Frames);
void Packet_Breakdown(Frame_Struct * Frames, uint8_t * Packet_Data, uint8_t Packet_Num, uint8_t Bytes_Left);
void Add_Flags(Frame_Struct * Frames, uint8_t * Finalized_Frame, uint8_t Frame_Num);

void Find_Check(Frame_Struct * Frames, bool * CRC_Sum, uint8_t Frame_Num, bool Add_To_Self);
void Calc_CRC(bool * Bit_Value, bool * CRC_Sum, uint8_t Length);
uint8_t Find_Count(bool * Bit_Value, uint8_t Count);
void Get_Bits(uint8_t Value, bool * Bit_Value, uint8_t Count);
uint8_t Get_Ints(bool * Bit_Value, uint8_t Count);

void MAC_Send(uint8_t * Full_Frame);

void Check_Reciever(uint8_t * MAC_Address, uint8_t * NET_Packet, uint8_t * Packet_Len, uint8_t * Data_Flag, Main_Frame * Pass_Frames);
void Receiving(Frame_Struct * Frames, uint8_t * Full_Frame, uint8_t * Packet_Len);
void MAC_LLC_Interface(Frame_Struct * Frames, uint8_t * Rec_Data, uint8_t Frame_Num, uint8_t Source);
void Remove_Flags(uint8_t * In_Frame, uint8_t Length, uint8_t Flag, uint8_t Esc);

void MAC_Listen(uint8_t * NET_Packet);

void Print_Frames(Frame_Struct * Frames, uint8_t Frame_Num);
void put_hex(uint8_t inp);

#endif