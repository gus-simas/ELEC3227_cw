/* Declaration of all struc */
#pragma once
#include <stdint.h>
#include <stdbool.h>

#define DEBUG_MSG 1

extern bool tl_retransmit_flag; // transport layer retransmit flag
typedef enum {ERROR=-1, SUCCESS=0}Status;

/* App layer wrapper */
typedef struct
{
    uint8_t buf[2];
    uint8_t len;
}al_data_t;

typedef struct
{
    
    bool transmit_flag;
    uint8_t src_port;
    uint8_t dest_port;
    uint8_t dest_dev;
    al_data_t tx_data;
    al_data_t rx_data;

}application;


/* Transport layer wrapper */
typedef struct
{
    uint8_t *buf;
    uint8_t len;
}tl_segment;

typedef struct
{
    tl_segment buf;
    uint8_t dev;
    bool flag;
	bool ack;
}transport;





/* Network layer wrapper */
/* Data link layer wrapper */