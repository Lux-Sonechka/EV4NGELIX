#ifndef NET_H
#define NET_H

#include <stdint.h>

typedef struct {
    uint8_t mac[6];
    uint32_t io_base;
    uint8_t irq;
    // Buffers, status, etc.
} net_device_t;

// Inicializa o driver de rede
void net_init();

// Envia um pacote
void net_send(const void* data, uint32_t len);

// Recebe um pacote (deve ser chamada pelo handler de IRQ)
uint32_t net_receive(void* buffer, uint32_t maxlen);

#endif