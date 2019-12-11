#include <string.h>

#include "communication.h"

static struct AES_ctx aes_ctx;

static packet_recv_cb_t packet_handler = NULL;

void communication_init(packet_recv_cb_t handler)
{
	extern const uint8_t blob_cipher[AES_KEYLEN];

	AES_init_ctx(&aes_ctx, (const uint8_t *) &blob_cipher);
}

void communication_send(uint8_t *buffer, size_t length)
{
	static uint8_t packet[COMM_MAX_PACKET_SIZE];

	memset(&packet, 0, COMM_MAX_PACKET_SIZE);
	memcpy(&packet, buffer, length < COMM_MAX_PACKET_SIZE ? length : COMM_MAX_PACKET_SIZE);
	AES_CBC_encrypt_buffer(&aes_ctx, packet, COMM_MAX_PACKET_SIZE);

	/* TODO: send the prepared packet */
}

static void internal_recv(uint8_t *buffer, size_t length)
{
	static uint8_t packet[COMM_MAX_PACKET_SIZE];

	/* only accept 16 byte packages */
	if (length != AES_BLOCKLEN) {
		return;
	}

	AES_CBC_decrypt_buffer(&aes_ctx, packet, COMM_MAX_PACKET_SIZE);

	if (packet_handler) {
		packet_handler(packet, length);
	}
}
