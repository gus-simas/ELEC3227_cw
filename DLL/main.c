#include "Data_Link_Layer.h"
#include "uart.h"

int main(){
    uint8_t Len = 16;
    uint8_t Pac[18] = {1,2,3,4,5,125,7,8,9,10,11,12,13,14,15,16,17,18};

    uint8_t MAC = 2;

    uint8_t Rec_Data[30]= {125, 0, 2, 1, 2, 8, 1, 2, 3, 4, 5, 126, 125, 7, 8, 170, 175, 125};
    uint8_t Rec_Data2[30]= {125, 1, 2, 1, 2, 8, 9, 10, 11, 12, 13, 14, 15, 16, 170, 175, 125};
    uint8_t  Packet_Len = 0, Data_Flag = 0;

    uint8_t L1;

    Main_Frame * Pass_Frames;  
    Pass_Frames = (Main_Frame*)calloc(0x05,sizeof(Main_Frame));


    for (L1 = 0; L1 < sizeof(Rec_Data); L1++){
       Pass_Frames[0].Data[L1] = Rec_Data[L1];
       Pass_Frames[1].Data[L1] = Rec_Data2[L1];
    }
    Pass_Frames[0].Size = 2;

    init_uart0();
    put_str("\n\nInitiating\n\r");

    LLC_Net_Interface(MAC, Pac, Len, Pass_Frames);


    for (L1 = 0; L1 < sizeof(Rec_Data); L1++){
       Rec_Data[L1] =0;
    }

    Check_Reciever(&MAC, Rec_Data, &Packet_Len, &Data_Flag, Pass_Frames);
  
    return 0;
}