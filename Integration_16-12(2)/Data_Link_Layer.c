#include "Data_Link_Layer.h"
#include "uart.h"


void LLC_Net_Interface(uint8_t Dest_MAC, uint8_t Packet_Data[], uint8_t Net_Leng, Main_Frame * Pass_Frames){  
   Frame_Struct * Frames;   
   uint8_t L1, L2, Curr_Byte, Bytes_Left;
   bool * Store;

   Frames = (Frame_Struct*)calloc(0x02,sizeof(Frame_Struct));
   put_str("DLL   :    Encaplsulating\r\n\n");
   
   put_str("Input: ");
   for(L1=0; L1<8; L1++){
	  put_hex(Packet_Data[L1]);
   }
   put_str("\r\n");

   L2 = Net_Leng % 0x08;
   if(L2 == 0){
      Net_Leng = Net_Leng/0x08;
   }else{
      Net_Leng = Net_Leng/0x08 + 1;
   }
   Bytes_Left = L2;
   
   put_str("Number of frames: ");
   put_hex(Net_Leng);
   put_str("\r\n");
   
   for(L1=0; L1<Net_Leng; L1++){
      Frames[L1].Control[0] = L1;
      Frames[L1].Control[1] = Net_Leng;
      Frames[L1].Source_MAC = Local_MAC;
      Frames[L1].Dest_MAC = Dest_MAC;
      Frames[L1].Length = 0x08;
      Frames[L1].checksum[1] = 0;
      Frames[L1].checksum[2] = 0;
   }
   
   put_str("Data: ");
   
   for(L1=0; L1<Net_Leng; L1++){
      if(L1 == Net_Leng-0x01 && Bytes_Left != 0){
         for(L2=0; L2<=Bytes_Left; L2++){
			 Curr_Byte = L1*0x08 +L2;
			put_hex(Packet_Data[Curr_Byte]);
            Frames[L1].NET_Packet[L2] = Packet_Data[Curr_Byte];
         }
         Frames[L1].Length = Bytes_Left;
      }else{
         for(L2=0; L2<=0x07; L2++){
			Curr_Byte = L1*0x08 +L2;
			put_hex(Packet_Data[Curr_Byte]);
            Frames[L1].NET_Packet[L2] = Packet_Data[Curr_Byte];
         }
      }

   }
   put_str("\r\n");
   Print_Frames(Frames,0); 
   Store = (bool*)calloc(0x1E,sizeof(bool));
   //for(L1=0; L1<Net_Leng; L1++){
      /*Find_Check(Frames, Store, L1, 0x01); 
      Frames[L1].checksum[0] = Get_Ints(Store,0x01);
      Frames[L1].checksum[1] = Get_Ints(Store,0x09);*/
	  put_str("Final: ");
      Add_Flags(Frames, Store, L1);
      for(L2=0; L2<0x022; L2++){
		 put_hex(Store[L2]);
         Pass_Frames[0].Data[L2] = Store[L2];
         Store[L2] = 0;
      }
   //}
   Pass_Frames[0].Size = Net_Leng;
   free(Frames);
   free(Store);
}

void Packet_Breakdown(Frame_Struct * Frames, uint8_t * Packet_Data, uint8_t Packet_Num, uint8_t Bytes_Left){
   uint8_t L1, L2, Curr_Byte;
   for(L1=0; L1<Packet_Num; L1++){
      if(L1 == Packet_Num-0x01 && Bytes_Left != 0){
         for(L2=0; L2<=Bytes_Left; L2++){
			 Curr_Byte = L1*0x08 +L2;
			put_hex(Packet_Data[Curr_Byte]);
            Frames[L1].NET_Packet[L2] = Packet_Data[Curr_Byte];
         }
         Frames[L1].Length = Bytes_Left;
      }else{
         for(L2=0; L2<=0x07; L2++){
			Curr_Byte = L1*0x08 +L2;
			put_hex(Packet_Data[Curr_Byte]);
            Frames[L1].NET_Packet[L2] = Packet_Data[Curr_Byte];
         }
      }

   }
}

void Find_Check(Frame_Struct * Frames, bool * CRC_Sum, uint8_t Frame_Num, bool Add_To_Self){
   uint8_t L1, L2, Count = 0;
   bool * Frame_Bit_Val;

   Frame_Bit_Val = (bool*)calloc(121,sizeof(bool));

   Get_Bits(Frames[Frame_Num].Control[0], Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].Control[1], Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].Source_MAC, Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].Dest_MAC, Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].Length, Frame_Bit_Val, Count);
   for(L1 = 0; L1 < Frames[Frame_Num].Length; L1++){
      Count += 8;
      Get_Bits(Frames[Frame_Num].NET_Packet[L1], Frame_Bit_Val, Count);
   }
   Count += 8;
   Get_Bits(Frames[Frame_Num].checksum[0], Frame_Bit_Val, Count);
   Count += 8;
   Get_Bits(Frames[Frame_Num].checksum[1], Frame_Bit_Val, Count);
   Count += 8;

   Calc_CRC(Frame_Bit_Val, CRC_Sum, Count);/*Calculates the CRC*/

   L2 = 1;
   if(Add_To_Self == 1){
      for(L1 = Count-0x10; L1 <= Count; L1++){
         CRC_Sum[L2] = Frame_Bit_Val[L1] ^ CRC_Sum[L2];
         L2 += 1;
      }
   }
   free(Frame_Bit_Val);
}

void Calc_CRC(bool * Bit_Value, bool * CRC_Sum, uint8_t Length){
   bool Genarator[0x11] = {0, 1,1,1,1, 1,1,1,1, 1,1,1,1, 0,0,0,1};
   uint8_t L1, L2, CRC_Loop = 1, Count, New_Count;

   Count = Find_Count(Bit_Value, 0);
   
   for(L1=0; L1<sizeof(Genarator);  L1++){
      CRC_Sum[L1] = Bit_Value[L1+Count] ^ Genarator[L1];
   }

   while(CRC_Loop == 1){
      New_Count = Find_Count(CRC_Sum, 0);
      if(Count+0x11==Length){
         New_Count -= 0;
      }else{
         New_Count = 1;
      }
      for(L1=New_Count; L1<sizeof(Genarator);  L1++){
         CRC_Sum[L1-New_Count] = CRC_Sum[L1];
      }

      L1 = 0;
      for(L2 = New_Count; L2>0; L2--){
         CRC_Sum[0x11-L2] = Bit_Value[Count+0x11+L1];
         L1 += 1;
      } 

      for(L1=0; L1<=sizeof(Genarator)-1;  L1++){
         CRC_Sum[L1] = CRC_Sum[L1] ^ Genarator[L1];
      }

      Count += New_Count;
      if(Count+0x11 >= Length){CRC_Loop = 0;}
   }
}

uint8_t Find_Count(bool * Bit_Value, uint8_t Count){
   uint8_t L1 = 0x00;
   while(L1 == 0x00){
      if(Bit_Value[Count] == 0x00){
         Count += 0x01;
      }else{
         L1 = 0x01;
      }
   }
   return Count;
}

void Get_Bits(uint8_t Value, bool * Bit_Value, uint8_t Count){
   int L1, Result = 0x00;
   int L2 = 0x00;
   for(L1=0x07; L1>=0x00; L1--){
      Result = Value - pow(0x02,L1);
      if(Result>=0x00){
         Bit_Value[L2 + Count] = 0x01;
         Value = Value - pow(0x02,L1);
      }
      L2 +=0x01;
   }
}

uint8_t Get_Ints(bool * Bit_Value, uint8_t Count){
   uint8_t Result = 0x00;
   int L1, L2 = 0x07;
   for(L1=0x00; L1<=0x07; L1++){
      if(Bit_Value[L1+Count] ==0x01){
         Result += pow(0x02,L2);
      }
      L2 -=0x01;
   }
   return Result;
}

void Add_Flags(Frame_Struct * Frames, uint8_t * Finalized_Frame, uint8_t Frame_Num){
   uint8_t L1 = 0, L2 = 0, L3;
   put_str("HEX:");
   Finalized_Frame[0] = Flag_Byte;
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Control[0];
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Control[1];
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Source_MAC;
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Dest_MAC;
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].Length;
   L1 += 1;
   for(L2=0;L2<Frames[Frame_Num].Length;L2++){
      Finalized_Frame[L1] = Frames[Frame_Num].NET_Packet[L2];
      L1 += 1;
   }

   Finalized_Frame[L1] = Frames[Frame_Num].checksum[0];
   L1 += 1;
   Finalized_Frame[L1] = Frames[Frame_Num].checksum[1];
   L1 += 1;
   Finalized_Frame[L1] = Flag_Byte;
   L3 = L1;
   for(L2=1;L2<L3-1;L2++){
      if(Finalized_Frame[L2] == Flag_Byte || Finalized_Frame[L2] == Esc_Byte){
         for(L1=L3+1; L1>L2;L1--){
            Finalized_Frame[L1] = Finalized_Frame[L1-1];
         }
         Finalized_Frame[L2] = Esc_Byte;
         L2+=1;
         L3+=1;
      }
   }
}



/*                        RECEIVING                        */
void Check_Reciever(uint8_t * MAC_Address, uint8_t * NET_Packet, uint8_t * Packet_Len, uint8_t * Data_Flag, Main_Frame * Pass_Frames){
   uint8_t L1,L2;
   Frame_Struct * Frames;  
   uint8_t * Store;

   Frames = (Frame_Struct*)calloc(0x03,sizeof(Frame_Struct));
   Store = (uint8_t*)calloc(0x1E,sizeof(uint8_t));

   Frames[0].Source_MAC = 0xff;

   for (L1 = 0; L1 < Pass_Frames[0].Size; L1++){
      if(Pass_Frames[L1].Data[0] != 0xFF){
         for (L2 = 0; L2 < 0x1E; L2++){
            Store[L2] = Pass_Frames[L1].Data[L2];
         }

         MAC_LLC_Interface(Frames, Store, L1, Frames[0].Source_MAC);

         if(L1 == Pass_Frames[0].Size-1){

            Receiving(Frames,NET_Packet,Packet_Len);
            *MAC_Address = Frames[0].Source_MAC;
            *Data_Flag = 1;

         }
      }
   }
   free(Frames);
}

void Receiving(Frame_Struct * Frames, uint8_t * Full_Frame, uint8_t * Packet_Len){
   uint8_t L1, L2;
   for (L1 = 0; L1 < Frames[0].Control[1]; L1++){
      for (L2= 0;  L2<Frames[L1].Length ; L2++){
         Full_Frame[(Frames[L1].Control[0]*0x08)+L2] = Frames[L1].NET_Packet[L2];
         *Packet_Len += 1;
      }
   }
}

void MAC_LLC_Interface(Frame_Struct * Frames, uint8_t * Rec_Data, uint8_t Frame_Num, uint8_t Source){
   uint8_t L1 = 0, L2 = 0;
   uint8_t * CRC_Sum;

   CRC_Sum = (uint8_t*)calloc(17,sizeof(uint8_t));

   if(Rec_Data[0] == Flag_Byte){
      L2 += 1;
      while(L1 == 0){
         if(Rec_Data[L2] == Esc_Byte){
            L2 += 2;
         }else if(Rec_Data[L2] == Flag_Byte){
            L2 += 1;
            L1 = 1;
         }else{
            L2 += 1;
         }
      }
      Remove_Flags(Rec_Data, L2, Flag_Byte, Esc_Byte);
      if(Source == Rec_Data[3] || Source == 0xFF){
         Frames[Frame_Num].Control[0] = Rec_Data[1];
         Frames[Frame_Num].Control[1] = Rec_Data[2];
         Frames[Frame_Num].Source_MAC = Rec_Data[3];
         Frames[Frame_Num].Dest_MAC   = Rec_Data[4];
         Frames[Frame_Num].Length     = Rec_Data[5];
         for(L1=0; L1<Rec_Data[5]; L1++){
            Frames[Frame_Num].NET_Packet[L1] = Rec_Data[L1+6];
         }
         Frames[Frame_Num].checksum[0] = Rec_Data[Rec_Data[5]+6];
         Frames[Frame_Num].checksum[1] = Rec_Data[Rec_Data[5]+7];

         Find_Check(Frames, CRC_Sum, Frame_Num, 0);

         L2 = 0;
         for(L1 = 1; L1 <17; L1++){
            if(CRC_Sum[L1] == 1){
               L2 = 1;
            }
         }
         free(CRC_Sum);
         if(L2 == 1){
            Frames[Frame_Num].Control[0] = 0;
            Frames[Frame_Num].Control[1] = 0;
            Frames[Frame_Num].Source_MAC = 0;
            Frames[Frame_Num].Dest_MAC   = 0;
            Frames[Frame_Num].Length     = 0;
            for(L1=0; L1<Rec_Data[5]; L1++){
               Frames[Frame_Num].NET_Packet[L1] = 0;
            }
            Frames[Frame_Num].checksum[0] = 0;
            Frames[Frame_Num].checksum[1] = 0;
         }else{
            Frame_Num +=1;
         }
      }
   }
}

void Remove_Flags(uint8_t * In_Frame, uint8_t Length, uint8_t Flag, uint8_t Esc){
   uint8_t L1 = 0, L2 = 0;
   for (L1 = 0; L1 < Length; L1++){
      if(In_Frame[L1] == Flag_Byte){
         In_Frame[L1] = 0;
      }else if(In_Frame[L1] == Esc_Byte){
         for(L2=L1; L2 < Length; L2++){
            In_Frame[L2] = In_Frame[L2+1];
         }
         L2+=1;
         Length -= 1;
      }
   }
}

void Print_Frames(Frame_Struct * Frames, uint8_t Frame_Num){
   uint8_t L1;
   put_str("Frame: ");
   put_hex(Frames[Frame_Num].Header);
   put_hex(Frames[Frame_Num].Control[0]);
   put_hex(Frames[Frame_Num].Control[1]);
   put_hex(Frames[Frame_Num].Source_MAC);
   put_hex(Frames[Frame_Num].Dest_MAC);
   put_hex(Frames[Frame_Num].Length);
   for(L1=0; L1<Frames[Frame_Num].Length; L1++){
      put_hex(Frames[Frame_Num].NET_Packet[L1]);
   }
   put_hex(Frames[Frame_Num].checksum[0]);
   put_hex(Frames[Frame_Num].checksum[1]);
   put_hex(Frames[Frame_Num].Footer);
   put_str("\r\n");
}

void put_hex(uint8_t inp){
   uint8_t char1 = (inp&0xF0)>>4;
   uint8_t char2 = inp&0x0F;
   if(char1<0x0A){
      put_ch(char1+48);
   }
   else{
      put_ch(char1+55);
   }
   if(char2<0x0A){
      put_ch(char2+48);
   }
   else{
      put_ch(char2+55);
   }
   put_str(" ");
}
