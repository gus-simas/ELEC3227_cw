// Transport Layer
# pragma once

#include "header.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAXIMUM_TRY_NUM 5

transport TL_send (const application app);
void TL_receive (const uint8_t dev, const tl_segment* rx_seg, transport* trans, al_data_t* app_data);
void TL_free_buffer(tl_segment *buf);
void increment_txsequence();
void increment_rxsequence();
void increment_attempts();
void reset_attempts();

transport_queue create_transport_queue(size_t capacity);
bool transport_queue_isfull(transport_queue *queue);
bool transport_queue_isempty(transport_queue *queue);
void transport_enqueue(transport_queue *queue, transport trans);
transport transport_dequeue(transport_queue *queue);

// Trasport Layer