#include "transport_layer.h"
#include "uart.h"
#include "hal.h"

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>


static uint8_t tx_sequence = 1, rx_sequence = 1;
static uint8_t tick = 0;
static uint8_t attempts = 0;
bool tl_retransmit_flag = 0;


transport_queue create_transport_queue(size_t capacity)
{
    transport_queue queue;
    queue.start = queue.size = 0;
    queue.capacity = capacity;
    queue.end = queue.capacity - 1;
    queue.buf = (transport*)malloc(queue.capacity*sizeof(transport));
    return queue;
}

bool transport_queue_isfull(transport_queue *queue)
{
    return (queue->size == queue->capacity);
}

bool transport_queue_isempty(transport_queue *queue)
{
    return (queue->size == 0);
}

void transport_enqueue(transport_queue *queue, transport trans)
{
    if (transport_queue_isfull(queue))
    {
        return;
    }
    queue->end = (queue->end+1)%queue->capacity;
    queue->buf[queue->end] = trans;
    ++queue->size;
}

transport transport_dequeue(transport_queue *queue)
{
    transport trans = queue->buf[queue->start];
    queue->start = (queue->start+1)%queue->capacity;
	--queue->size;
    return trans;
}

static uint16_t sum_checksum (const uint8_t len, const uint8_t* buf)
{
    uint16_t sum = 0;
	uint8_t i;
    for (i = 0; i < len; ++i)
    {
        sum += buf[i];
    }
    return sum;
}

void increment_txsequence()
{
    if (tx_sequence >= 256 || tx_sequence == 0)
    {
        tx_sequence = 1;
    } else
    {
        ++tx_sequence;
    }
}

void increment_rxsequence()
{
    if (rx_sequence >= 256 || rx_sequence == 0)
    {
        rx_sequence = 1;
    } else
    {
        ++rx_sequence;
    }
}


ISR(TIMER2_COMPA_vect)
{
    // max tick = internal f_c / (prescaler * max of 8-bit) 12*10^6/(1024*256)
    // Timeout in transport layer = 1s
    ++tick;
    if (tick == 46 && attempts < MAXIMUM_TRY_NUM)
    {
        tl_retransmit_flag = 1;
        tick = 0; // reset tick
    }
	
	if (attempts >= MAXIMUM_TRY_NUM)
	{
        put_str("\r\nGiving up resending the message now...\r\n");
#if(DEBUG_MSG)
		disable_timeout();
		reset_attempts();
		tl_retransmit_flag = tick = 0;
#endif
	}
}

void increment_attempts()
{
	++attempts;
}

void reset_attempts()
{
	attempts = 0;
}

transport TL_send (const application app)
{
	transport trans;
    uint16_t checksum;
    uint8_t len = app.tx_data.len, i;

    trans.buf.buf = (uint8_t *)malloc((len+7)*sizeof(uint8_t));

    // Header of the segment
    trans.buf.buf[0] = tx_sequence;
    trans.buf.buf[1] = 0x00;
    trans.buf.buf[2] = app.src_port;
    trans.buf.buf[3] = app.dest_port;
    trans.buf.buf[4] = len;

    // Data of the segment
    for (i = 5; i < len+5; ++i)
    {
        trans.buf.buf[i] =  app.tx_data.buf[i-5];
    }

    // Checksum of the segment
    checksum = sum_checksum(len+5, trans.buf.buf);
    trans.buf.buf[5+len] = (uint8_t)(checksum >> 4);
    trans.buf.buf[6+len] = (uint8_t)checksum;
    trans.buf.len = len + 7;
    trans.dev = app.dest_dev;

#if(DEBUG_MSG)
	char buf[5];
    for (i = 0; i < trans.buf.len ; ++i)
    {
        put_str("TL send segment ");
        put_str(itoa(i, buf, 10));
        put_str(" :  ");
        put_str(itoa(trans.buf.buf[i], buf, 10));
        put_str("\r\n");
    }
	put_str("Destination Device : ");
    put_str(itoa(trans.dev, buf, 10));
    put_str("\r\n");
#endif
	return trans;
}

void TL_receive (const uint8_t dev, const tl_segment* rx_seg, transport* trans, al_data_t* app_data)
{
    uint16_t checksum = 0;
    uint8_t len = rx_seg->len, i;
    app_data->len = rx_seg->buf[4];
	trans->update_app = false;
	
    /* Handle ACK segment*/
    if ((int8_t)rx_seg->buf[1])
    {
		put_str("\n\rTRANS:   RECEIVED ACK\n\r");
        /* Check the sequence number is correct*/
        if ((int8_t)rx_seg->buf[1] != tx_sequence)
        {
            return;
        }

        /* Check if the checksum is correct */
        checksum = sum_checksum(len-2, rx_seg->buf);

        if (rx_seg->buf[len-2] != (uint8_t)(checksum >> 4) || rx_seg->buf[len-1] != (uint8_t)(checksum))
        {
            // Oh no, wrong checksum.put_str("\r\n");..
            return;
        }

        /* Correct ACK, disable the interrupt */
        disable_timeout();
		tl_retransmit_flag = 0;
		increment_txsequence();
        return;
    }
	
	put_str("\n\rTRANS:   PACKAGING ACK\n\r");
    /* Handle other segments */
    checksum = sum_checksum(len-2, rx_seg->buf);

    if (rx_seg->buf[len-2] != (uint8_t)(checksum >> 4) || rx_seg->buf[len-1] != (uint8_t)(checksum))
    {
        // Oh no, wrong checksum... wait for the other end to time out
		return;
    }

    trans->buf.buf = (uint8_t *)malloc((len)*sizeof(uint8_t));
    trans->buf.buf[0] = rx_sequence;
    trans->buf.buf[1] = rx_seg->buf[0];
    trans->buf.buf[2] = rx_seg->buf[3];
    trans->buf.buf[3] = rx_seg->buf[2];
    trans->buf.buf[4] = rx_seg->buf[4];
    trans->buf.len = rx_seg->len;
    for (i = 0; i < app_data->len; ++i)
    {
        app_data->buf[i] = rx_seg->buf[i+5];
        trans->buf.buf[i+5] = 0;
    }
    
    // Checksum of the segment
    checksum = sum_checksum(len-2, trans->buf.buf);
    trans->buf.buf[len-2] = (uint8_t)(checksum >> 4);
    trans->buf.buf[len-1] = (uint8_t)checksum;
    trans->dev = dev;
	trans->send_ack = true;
    trans->update_app = true;
#if(DEBUG_MSG)    
    for (i = 0; i < trans->buf.len ; ++i)
    {
        char buf[5];
        put_str("TL receive segment ");
        put_str(itoa(i, buf, 10));
        put_str(" :  ");
        put_str(itoa(trans->buf.buf[i], buf, 10));
        put_str("\r\n");
    }
    put_str("\r\n");
#endif

}

void TL_free_buffer(tl_segment *buf)
{
    free(buf->buf);
    free(buf);
}