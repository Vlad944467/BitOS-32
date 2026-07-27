#include <stdint.h>

extern unsigned char inb(unsigned short port);
extern void outb(unsigned short port, unsigned char value);
extern uint16_t inw(uint16_t port);
extern void outw(uint16_t port, uint16_t value);

// ATA func

void ata_wait() {
    while ((inb(0x1F7) & 0xC0) != 0x40);
}

void ata_read_sector(uint32_t lba, uint8_t* buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x20);
    ata_wait();
    for (int i = 0; i < 256; i++) {
        ((uint16_t*)buffer)[i] = inw(0x1F0);
    }
}

void ata_write_sector(uint32_t lba, uint8_t* buffer) {
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F7, 0x30);
    ata_wait();
    for (int i = 0; i < 256; i++) {
        outw(0x1F0, ((uint16_t*)buffer)[i]);
    }
    ata_wait();
}

int ata_detect(int port) {
    outb(port + 6, 0xA0);
    outb(port + 2, 0xEC);
    for (int i = 0; i < 1000; i++) {
        if ((inb(port + 7) & 0x80) == 0) break;
    }
    if (inb(port + 7) == 0x00) return 0;
    return 1;
}

void ata_get_model(int port, char *model) {
    unsigned short buffer[256];
    outb(port + 6, 0xA0);
    outb(port + 2, 0xEC);
    for (int i = 0; i < 256; i++) {
        buffer[i] = inw(port);
    }
    for (int i = 0; i < 20; i++) {
        model[i * 2] = buffer[27 + i] >> 8;
        model[i * 2 + 1] = buffer[27 + i] & 0xFF;
    }
    model[40] = '\0';
}
