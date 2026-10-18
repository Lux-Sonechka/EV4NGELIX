#include "net.h"
#include "../../lib/libc/mem.h"
#include "../../kernel/low_level.h"
#include <stdint.h>
#include <stddef.h>

// Exemplo de dispositivo global
static net_device_t net_dev;

// Buffer fictício para simular envio/recebimento
#define NET_BUFFER_SIZE 2048
static uint8_t net_buffer[NET_BUFFER_SIZE];
static uint32_t net_buffer_len = 0;

void net_init() {
    // Inicializa campos fictícios do dispositivo de rede
    net_dev.io_base = 0xC000; // Exemplo de endereço base
    net_dev.irq = 10;         // Exemplo de IRQ
    // MAC fictício
    net_dev.mac[0] = 0xDE;
    net_dev.mac[1] = 0xAD;
    net_dev.mac[2] = 0xBE;
    net_dev.mac[3] = 0xEF;
    net_dev.mac[4] = 0x00;
    net_dev.mac[5] = 0x01;
    net_buffer_len = 0;
}

void net_send(const void* data, uint32_t len) {
    // Simula envio: copia para buffer fictício
    if (len > NET_BUFFER_SIZE) len = NET_BUFFER_SIZE;
    memcpy(net_buffer, data, len);
    net_buffer_len = len;

    // Opcional: imprimir para debug
    // printf("net_send: %u bytes enviados\n", len);
}

uint32_t net_receive(void* buffer, uint32_t maxlen) {
    // Simula recebimento: copia do buffer fictício
    uint32_t to_copy = (net_buffer_len < maxlen) ? net_buffer_len : maxlen;
    memcpy(buffer, net_buffer, to_copy);
    net_buffer_len = 0; // Limpa buffer após leitura
    return to_copy;
}