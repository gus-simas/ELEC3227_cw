/* Declaration of all struc */
#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#define DEBUG_MSG 1

extern bool tl_retransmit_flag; // transport layer retransmit flag

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
	bool send_ack;
    bool update_app;
}transport;


typedef struct
{
    size_t start, end, size;
    size_t capacity;
    transport *buf;
}transport_queue;




/* Network layer wrapper */
/* Data link layer wrapper */