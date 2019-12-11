#ifndef COMMUNICATION_H
#define COMMUNICATION_H

/**
 * Basic communication layer to receive and send small packets via radio
 *
 * Packets are encrypted/decrypted using AES. A packet handler can be
 * registered during initialization.
 */

#include <stdint.h>

#include "tinyaes/aes.h"

#define COMM_MAX_PACKET_SIZE AES_BLOCKLEN

typedef void (*packet_recv_cb_t) (uint8_t *packet, size_t packet_length);

void communication_init(packet_recv_cb_t handler);

void communication_send(uint8_t *buffer, size_t length);

#endif
