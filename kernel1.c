#include <stddef.h>

#define VIDEO_MEMORY 0xB8000
#define BLUE_ON_WHITE 0xF1
#define GREEN_ON_WHITE 0xF2
#define RED_ON_WHITE 0xF4
#define BLACK_ON_WHITE 0xF0

void print_int(int num, int color);

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

static inline void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

void putchar(char c, int color) {
    static int cursor = 0;
    char* video = (char*) VIDEO_MEMORY;
    if (c == '\n') {
        cursor += 80 - (cursor % 80);
        return;
    }
    video[cursor * 2] = c;
    video[cursor * 2 + 1] = color;
    cursor++;
    if (cursor >= 80 * 25) {
        for (int i = 0; i < 80 * 25; i++) {
            video[i * 2] = ' ';
            video[i * 2 + 1] = BLUE_ON_WHITE;
        }
        cursor = 0;
    }
}

void print(const char* str, int color) {
    while (*str) putchar(*str++, color);
}

char read_keyboard() {
    while ((inb(0x64) & 1) == 0);
    unsigned char sc = inb(0x60);
    if (sc >= 0x80) return 0;
    switch (sc) {
        case 0x02: return '1'; case 0x03: return '2';
        case 0x04: return '3'; case 0x05: return '4';
        case 0x06: return '5'; case 0x07: return '6';
        case 0x08: return '7'; case 0x09: return '8';
        case 0x0A: return '9'; case 0x0B: return '0';
        case 0x0C: return '-'; case 0x0D: return '=';
        case 0x10: return 'q'; case 0x11: return 'w';
        case 0x12: return 'e'; case 0x13: return 'r';
        case 0x14: return 't'; case 0x15: return 'y';
        case 0x16: return 'u'; case 0x17: return 'i';
        case 0x18: return 'o'; case 0x19: return 'p';
        case 0x1A: return '['; case 0x1B: return ']';
        case 0x1E: return 'a'; case 0x1F: return 's';
        case 0x20: return 'd'; case 0x21: return 'f';
        case 0x22: return 'g'; case 0x23: return 'h';
        case 0x24: return 'j'; case 0x25: return 'k';
        case 0x26: return 'l'; case 0x27: return ';';
        case 0x28: return '\''; case 0x29: return '`';
        case 0x2B: return '\\'; case 0x2C: return 'z';
        case 0x2D: return 'x'; case 0x2E: return 'c';
        case 0x2F: return 'v'; case 0x30: return 'b';
        case 0x31: return 'n'; case 0x32: return 'm';
        case 0x33: return ','; case 0x34: return '.';
        case 0x35: return '/'; case 0x39: return ' ';
        case 0x1C: return '\n';
        default: return 0;
    }
}

int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return *a - *b;
}

void print_int(int num, int color) {
    if (num == 0) {
        putchar('0', color);
        return;
    }
    if (num < 0) {
        putchar('-', color);
        num = -num;
    }
    char buf[16];
    int i = 0;
    while (num) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i > 0) putchar(buf[--i], color);
}

void main(void) {
    char* video = (char*) VIDEO_MEMORY;
    for (int i = 0; i < 80 * 25; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = BLUE_ON_WHITE;
    }
    
    // Звук
    outb(0x61, inb(0x61) | 3);
    outb(0x42, 0xFF);
    outb(0x42, 0xFF);
    for (int i = 0; i < 100000; i++);
    outb(0x61, inb(0x61) & 0xFC);
    for (int i = 0; i < 50000; i++);
    outb(0x61, inb(0x61) | 3);
    outb(0x42, 0x80);
    outb(0x42, 0x80);
    for (int i = 0; i < 100000; i++);
    outb(0x61, inb(0x61) & 0xFC);
    
    // WELCOME
    for (int i = 0; i < 80; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x1F;
    }
    char* welcome = "WELCOME TO BITOS-32";
    int len = 20;
    int start = (80 - len) / 2;
    for (int i = 0; i < len; i++) {
        video[(start + i) * 2] = welcome[i];
        video[(start + i) * 2 + 1] = 0x1F;
    }
    print("\n", BLUE_ON_WHITE);
    
    print("BitOS-32 Kernel v1.0\n", 0x71);
    print("=======================================================================\n", 0x70);
    print("Commands: help, sysinfo, clear, about, ping, mode, cat, win, reboot, echo, version, fetch, text, say, rand, tree\n", 0x70);
    print("=======================================================================\n", 0x70);
    print("> ", GREEN_ON_WHITE);
    
    char cmd[256];
    int idx = 0;
    
    while (1) {
        char ch = read_keyboard();
        if (ch == 0) continue;
        if (ch == '\n') {
            cmd[idx] = '\0';
            print("\n", BLUE_ON_WHITE);
            
            if (strcmp(cmd, "help") == 0) {
                print("Commands: help, sysinfo, clear, about, ping, mode, cat, win, reboot, echo, version, fetch, text, say, rand, tree\n", BLUE_ON_WHITE);
            }
            else if (strcmp(cmd, "sysinfo") == 0) {
                print("==========================\n", 0x70);
                print("BitOS-32 Kernel v1.0     \n", 0x71);
                print("Architecture: x86 (32-bit)\n", 0x70);
                print("Text mode: 80x25          \n", 0x70);
                print("Colors: 16                \n", 0x70);
                print("==========================\n", 0x70);
            }
            else if (strcmp(cmd, "clear") == 0) {
                for (int i = 0; i < 80 * 25; i++) {
                    video[i * 2] = ' ';
                    video[i * 2 + 1] = BLUE_ON_WHITE;
                }
            }
            else if (strcmp(cmd, "about") == 0) {
                print("================================================================\n", 0x72);
                print("|BitOS-32 is a 32-bit operating system written in C and Assembly|\n", 0x72);
                print("|                                                              |\n", 0x72);
                print("|Main features:                                                |\n", 0x72);
                print("| -80x25 text mode                                             |\n", 0x72);
                print("| -Color output (VGA attributes)                               |\n", 0x72);
                print("| -PS/2 keyboard support                                       |\n", 0x72);
                print("| -Sound via PC Speaker                                        |\n", 0x72);
                print("| -Pseudo windows and frames                                   |\n", 0x72);
                print("| -Built-in commands                                           |\n", 0x72);
                print("|                                                              |\n", 0x72);
                print("|System requirements:                                          |\n", 0x72);
                print("| -Processor: i386 or higher                                   |\n", 0x72);
                print("| -Memory: from 4 MB                                           |\n", 0x72);
                print("| -Video card: VGA compatible                                  |\n", 0x72);
                print("|                                                              |\n", 0x72);
                print("|Version: 1.0                                                  |\n", 0x72);
                print("================================================================\n", 0x72);
            }
            else if (strcmp(cmd, "ping") == 0) {
                print("pong\n", GREEN_ON_WHITE);
            }
            else if (strcmp(cmd, "mode") == 0) {
                print("Protected mode (32-bit)\n", BLUE_ON_WHITE);
            }
            else if (strcmp(cmd, "cat") == 0) {
                print(" /\\_/\\\n", GREEN_ON_WHITE);
                print("( o.o )\n", GREEN_ON_WHITE);
                print(" > ^ <\n", GREEN_ON_WHITE);
            }
            else if (strcmp(cmd, "win") == 0) {
                print("+------------------+\n", 0x70);
                print("|    BitOS-32 OS    |\n", 0x71);
                print("|                  |\n", 0x70);
                print("|   Hello, User!   |\n", 0x70);
                print("+------------------+\n", 0x70);
            }
            else if (strcmp(cmd, "say") == 0) {
                char* words[] = {
                    "hello", "world", "kernel", "system", "code", "debug", "compile", "run",
                    "asm", "c language", "pointer", "memory", "stack", "heap", "interrupt",
                    "vga", "ata", "pci", "usb", "ps2", "cmos", "dma", "pit", "pic",
                    "bitos", "vladislav", "os dev", "32-bit", "protected mode", "real mode",
                    "help", "clear", "reboot", "echo", "version", "fetch", "text", "win", "cat", "ping",
                    "red", "green", "blue", "yellow", "cyan", "magenta", "white", "black",
                    "random", "rand", "destiny", "fate", "luck", "chance", "chaos", "order",
                    "you are awesome", "keep coding", "debugging is fun", "almost there",
                    "nice job", "try again", "so close", "perfect", "done", "fail", "success",
                    "bitos os", "32-bit kernel", "c and assembly", "text mode", "80x25",
                    "colorful output", "keyboard input", "beep sound", "pseudo windows", "frames",
                    "help system", "system info", "clear screen", "about bitos", "ping pong",
                    "show mode", "ascii cat", "reboot system", "echo text", "show version"
                };
                int count = sizeof(words) / sizeof(words[0]);
                static int oracle_counter = 0;
                oracle_counter++;
                int r = (oracle_counter * 12345) % count;
                print(words[r], 0xF2);
                print("\n", 0x72);
            }
            else if (strcmp(cmd, "edit") == 0) {
                char text[1000];
                int pos = 0;
                print("Enter text. Type '\\e' on new line to exit.\n", 0xF2);
                while (1) {
                    char ch = read_keyboard();
                    if (ch == '\n') {
                        if (pos > 0 && text[pos-1] == '\\' && text[pos-2] == '\n') {}
                        text[pos++] = '\n';
                        print("\n", 0x72);
                    } else {
                        text[pos++] = ch;
                        putchar(ch, 0xF2);
                    }
                    if (ch == 0) continue;
                    if (ch == 0x1B) {
                        print("\nExiting.\n", 0x72);
                        break;
                    }
                    if (ch == '\n' && pos >= 4 &&
                        text[pos-4] == 'S' && text[pos-3] == 'A' &&
                        text[pos-2] == 'V' && text[pos-1] == 'E') {
                        pos -= 4;
                        print("\nSaved! Exiting.\n", 0xF2);
                        break;
                    }
                }
            }
            else if (strcmp(cmd, "script") == 0) {
                print(" var x = 10  // variables\n", 0xF2);
                print(" var name = 'Vlad'\n", 0xF2);
                print("                  \n", 0xF2);
                print(" out('Hello') // output\n", 0xF2);
                print(" out('x')\n", 0xF2);
                print("                  \n", 0xF2);
                print(" var sum = x + 5 //arithmetic\n", 0xF2);
                print("                  \n", 0xF2);
                print(" if x > 5 { //conditions\n", 0xF2);
                print("     out('its big')\n", 0xF2);
                print(" }                \n", 0xF2);
            }
            else if (strcmp(cmd, "version") == 0) {
                print("BitOS-32 v1.0 (32-bit monolithic kernel)\n", GREEN_ON_WHITE);
            }
            else if (strcmp(cmd, "fetch") == 0) {
                print("  .--.\n", GREEN_ON_WHITE);
                print(" |o_o |\n", GREEN_ON_WHITE);
                print(" |:_/ |\n", GREEN_ON_WHITE);
                print("//   \\ \\\n", GREEN_ON_WHITE);
                print("(|     | )\n", GREEN_ON_WHITE);
                print("/'\\_   _/`\\\n", GREEN_ON_WHITE);
                print("\\___)=(___/\n", GREEN_ON_WHITE);
            }
            else if (strcmp(cmd, "text") == 0) {
                print("╔════════════════════════════════════════════════════════════════╗\n", 0x72);
                print("║                      BitOS-32 v1.0                            ║\n", 0x72);
                print("╠════════════════════════════════════════════════════════════════╣\n", 0x72);
                print("║  32-bit monolithic kernel | C + Assembly                     ║\n", 0x72);
                print("║  Commands: help, clear, version, reboot, echo, win, fetch    ║\n", 0x72);
                print("╚════════════════════════════════════════════════════════════════╝\n", 0x72);
            }
            else if (strcmp(cmd, "rand") == 0) {
                static int r = 0;
                r = (r * 1103515245 + 12345) & 0x7FFFFFFF;
                print_int(r % 1000, GREEN_ON_WHITE);
                print("\n", GREEN_ON_WHITE);
            }
            else if (strcmp(cmd, "tree") == 0) {
                print("BitOS-32\n", GREEN_ON_WHITE);
                print("├─ kernel.c\n", GREEN_ON_WHITE);
                print("├─ boot.s\n", GREEN_ON_WHITE);
                print("├─ linker.ld\n", GREEN_ON_WHITE);
                print("└─ Makefile\n", GREEN_ON_WHITE);
            }
            else if (strcmp(cmd, "reboot") == 0) {
                print("Rebooting...\n", RED_ON_WHITE);
                outb(0x64, 0xFE);
            }
            else if (cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o' && cmd[4] == ' ') {
                print(cmd + 5, BLACK_ON_WHITE);
                print("\n", BLACK_ON_WHITE);
            }
            else if (cmd[0] != '\0') {
                print("Unknown command: ", RED_ON_WHITE);
                print(cmd, RED_ON_WHITE);
                print("\n", BLUE_ON_WHITE);
            }
            print("> ", GREEN_ON_WHITE);
            idx = 0;
        } else if (idx < 255) {
            cmd[idx++] = ch;
            putchar(ch, BLUE_ON_WHITE);
        }
    }
}
