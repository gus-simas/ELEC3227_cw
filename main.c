
#include "NET.h"
#include "stdbool.h"

Network pack;

int main()
{
    while(1)
    {
        /* APP INPUTS */

        printf("HEEEEEEEEY");

        if(trans->TL_transmit){
            NET_receive_TRAN(&pack, tran->TRAN_Segment, len, DEST);
            trans->TL_transmit = false;
        }
        else{
            /* APP OUTPUTS */
        }
        if(pack.broadcast){
            DLL_receive(&dll, pack.broadpack, pack.packet_len, 0xFF);
        }
        else if(pack.rebroadcast){
            DLL_receive(&dll, pack.broadpack, pack.packet_len, 0xFF);
        }
        else if(pack.send){
            DLL_receive(&dll, pack.sPacket, pack.packet_len, pack.dest_mac_address);
        }
        else if(pack.receive){
            TRAN_receive(&tran, pack.TRAN_Segment, pack.seg_length);
        }
    }
}
