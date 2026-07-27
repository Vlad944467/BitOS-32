#include <stdint.h>
#include <stddef.h>

#define VIDEO_MEMORY 0xB8000

// Цвета для чёрного фона
#define WHITE_ON_BLACK  0x0F
#define GREEN_ON_BLACK  0x02
#define RED_ON_BLACK    0x04
#define YELLOW_ON_BLACK 0x0E
#define CYAN_ON_BLACK   0x03
#define BLUE_ON_BLACK   0x01

#define OS_NAME "BITOS-32"
#define MAX_HIST 20

#define cls clear_screen()

#define p(s, c) print(s, c)
#define say(s, c) print(s, c)

#define loop while
#define e else
#define sayi print_int
#define cycle for
#define cyc for
#define ret return
#define stop break

#define is ==
#define not !=
#define or ||
#define and &&

typedef unsigned char byte;
typedef unsigned int ui;
typedef int numm;
typedef void empty;

empty print(const char* str, int color);
empty clear_screen(void);
empty save_note(int slot, char* name);
empty load_note(int slot);

void print_int(int num, int color);
void putchar(char c, int color);
void print(const char* str, int color);
char read_keyboard();
numm strcmp(const char* a, const char* b);
numm strncmp(const char* a, const char* b, int n);

char current_dir[32] = "/home";
char* strcpy(char* dest, const char* src);
char notes[10][4096];
char saved_note[4096];
char note_names[10][32];
char *last_error_msg = "";
numm last_status = 0;
numm cursor_pos = 0;
numm note_count = 0;
numm screen_rows = 25;
numm pixel_mode = 0;
numm desktop_mode = 0;
numm cursor_x = 10, cursor_y = 6;
char *history[MAX_HIST];
int hist_count = 0;
int ticks = 0;

// ТЕКУЩИЙ ЦВЕТ ФОНА (по умолчанию чёрный)
int bg_color = 0x00;

//табельные команды
void cmd_h(char *args);
void cmd_hello(char *args);
void cmd_time(char *args);
void cmd_par(char *args);
void parse_table_command(char *cmd);
void cmd_echo(char *args);
void cmd_calc(char *args);
void cmd_reb(char *args);
void cmd_sh(char *args);
void cmd_sleep(char *args);
void cmd_type(char *args);
void cmd_bgcolor(char *args);
void cmd_edit_par(char *args);
void clear_cmd(char *args);


// =структура команд=
typedef struct {
    char *name;
    void (*func)(char *args);
    char *desc;
} command_t;

// ===================== таблица табельных команд  =====================
command_t commands[] = {
    {"help",  cmd_h,     "Show table commands"},
    {"hello", cmd_hello, "Say hello"},
    {"time",  cmd_time,  "Show time"},
    {"echo", cmd_echo,   "echo text"},
    {"calc", cmd_calc,   "calculator"},
    {"reboot", cmd_reb,  "rebooting"},
    {"shutdown", cmd_sh, "shutdowning pc"},
    {"sleep", cmd_sleep, "Wait for N seconds"},
    {"bgcolor", cmd_bgcolor, "Change background color 0-F"},
    {"edit", cmd_edit_par, "Text editor"},
    {"clear", clear_cmd, "clear screen"},
    {NULL, NULL, NULL}
};

//======================ATA драйвера=======================
uint16_t inw(uint16_t port) {
    uint16_t result;
    __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outw(uint16_t port, uint16_t value) {
    __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
}

unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(unsigned short port, unsigned char value) {
    __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

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

void save_note(int slot, char* name) {
    if (slot >= 0 && slot < 10) {
        strcpy(note_names[slot], name);
        strcpy(notes[slot], saved_note);
        ata_write_sector(100 + slot, (uint8_t*)notes[slot]);
        print("Saved: ", GREEN_ON_BLACK);
        print(name, GREEN_ON_BLACK);
        print(" (slot ", GREEN_ON_BLACK);
        print_int(slot, GREEN_ON_BLACK);
        print(")\n", GREEN_ON_BLACK);
    }
}

void load_note(int slot) {
    if (slot >= 0 && slot < 10) {
        ata_read_sector(100 + slot, (uint8_t*)notes[slot]);
        print("Loaded from slot ", GREEN_ON_BLACK);
        print_int(slot, GREEN_ON_BLACK);
        print("\n", GREEN_ON_BLACK);
    }
}

void putchar(char c, int color) {
    static int cursor = 0;
    char* video = (char*) VIDEO_MEMORY;
    if (c == '\n') {
        cursor += 80 - (cursor % 80);
    } else {
        video[cursor * 2] = c;
        video[cursor * 2 + 1] = color;
        cursor++;
    }

    if (cursor >= 80 * 25) {
        for (int i = 80; i < 80 * 25; i++) {
            video[(i - 80) * 2] = video[i * 2];
            video[(i - 80) * 2 + 1] = video[i * 2 + 1];
        }
        for (int i = 80 * 24; i < 80 * 25; i++) {
            video[i * 2] = ' ';
            video[i * 2 + 1] = color;
        }
        cursor -= 80;
    }
}

void print(const char* str, int color) {
    if (pixel_mode) return;
    while (*str) putchar(*str++, color);
}

int strcmp(const char* a, const char* b) {
    while (*a && *a == *b) { a++; b++; }
    return *a - *b;
}

int strncmp(const char* a, const char* b, int n) {
    for (int i = 0; i < n; i++) {
        if (a[i] != b[i]) return a[i] - b[i];
        if (a[i] == '\0') return 0;
    }
    return 0;
}

void print_int(int num, int color) {
    if (num == 0) {
        putchar('0', color);
        return;
    }
    if (num < 0) {
        putchar('-', color);
        if (num == -2147483648) {
            char *min_str = "2147483648";
            while (*min_str) {
                putchar(*min_str++, color);
            }
            return;
        }
        num = -num;
    }
    char buf[16];
    int i = 0;
    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }
    while (i > 0) {
        putchar(buf[--i], color);
    }
}

void save_note_to_disk() {
    ata_write_sector(100, (uint8_t*)saved_note);
    say("Note saved to disk (sector 100)\n", GREEN_ON_BLACK);
}

void load_note_from_disk() {
    ata_read_sector(100, (uint8_t*)saved_note);
    say("Note loaded from disk\n", GREEN_ON_BLACK);
}

void putchar_at(int x, int y, char c, int color) {
    char* video = (char*)0xB8000;
    int pos = y * 80 + x;
    video[pos * 2] = c;
    video[pos * 2 + 1] = color;
}

void clear_screen() {
    char* video = (char*)0xB8000;

    // Синяя полоса сверху
    for (int i = 0; i < 80; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x17;
    }

    // Остальное — чёрный фон
    int default_attr = bg_color | 0x0F;
    for (int i = 80; i < 80 * 25; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = default_attr;
    }
    say("\n", default_attr);
}

void put_pixel(int x, int y, int color) {
    if (pixel_mode && x >= 0 && x < 320 && y >= 0 && y < 200) {
        ((uint8_t*)0xA0000)[y * 320 + x] = color;
    }
}

char* strcpy(char* dest, const char* src) {
    char* d = dest;
    while ((*d++ = *src++));
    return dest;
}

void update_prompt() {
    int color = bg_color | 0x02; // Зелёный текст на текущем фоне
    say(current_dir, color);
    say("> ", color);
}

void cpuid(int code, int *a, int *b, int *c, int *d) {
    asm volatile("cpuid"
                : "=a"(*a), "=b"(*b), "=c"(*c), "=d"(*d)
                : "a"(code));
}

void sysinf() {
    int a, b, c, d;
    char vendor[13] = {0};

    cpuid(0, &a, &b, &c, &d);
    *(int*)(vendor) = b;
    *(int*)(vendor + 4) = d;
    *(int*)(vendor + 8) = c;

    int color = bg_color | 0x02;
    say("+-------------------------+\n", color);
    say("|     CPU information     |\n", color);
    say("+-------------------------+\n", color);
    say("CPU: ", color);
    say(vendor, color);
    say("\n", color);
    say("+-------------------------+\n", color);
}

void delay(int count) {
    cyc (volatile int i = 0; i < count * 100000; i++);
}

void banner() {
    char *text = " BITOS-32 v1.5 ";
    int len = 15;
    int color = bg_color | 0x0F;
    cyc (int i = 0; i < 40; i++) {
        say("\r", color);
        cyc (int j = 0; j < 20; j++) print(" ", color);
        say("\r", color);
        cyc (int j = 0; j < len; j++) {
            say(text[(i + j) % len], color);
        }
        delay(1);
    }
    say("\n", color);
}

void add_history(char *cmd) {
    if (hist_count < MAX_HIST) {
        history[hist_count] = cmd;
        hist_count++;
    }
}

void show_history() {
    int color = bg_color | 0x0F;
    for (int i = 0; i < hist_count; i++) {
        say(history[i], color);
        say("\n", color);
    }
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

void gotoxy(int x, int y) {
    int pos = y * 80 + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char)((pos >> 8) & 0xFF));
}

void wi() {
    gotoxy(20, 8);
    say("+------------------------------------+\n", 0x17);
    gotoxy(20, 9);
    say("|                                    |\n", 0x17);
    gotoxy(20, 10);
    say("|             WINDOW                 |\n", 0x17);
    gotoxy(20, 11);
    say("|                                    |\n", 0x17);
    gotoxy(20, 12);
    say("|       Press any key to close       |\n", 0x17);
    gotoxy(20, 13);
    say("|                                    |\n", 0x17);
    gotoxy(20, 14);
    say("+------------------------------------+\n", 0x17);
}

void wi_close() {
    for (int i = 8; i <= 14; i++) {
        gotoxy(20, i);
        for (int j = 0; j < 40; j++) say(" ", 0x00);
    }
}

void install_os() {
    char confirm[4];
    int color = bg_color | 0x0F;
    int red = bg_color | 0x04;

    say("=======================================\n", color);
    say("         BITOS INSTALLER               \n", color);
    say("=======================================\n", color);
    say("WARNING: This will OVERWRITE your disk!\n", red);
    say("All data will be LOST forever.\n", red);
    say("\n", color);

    say("Type 'yes1' to continue: ", color);
    readline(confirm);

    if (strcmp(confirm, "yes1") != 0) {
        say("Installation cancelled.\n", color);
        return;
    }
    say("Are you REALLY sure? Type 'yes1' again: ", color);
    readline(confirm);

    if (strcmp(confirm, "yes1") != 0) {
        say("Installation cancelled.\n", color);
        return;
    }
    say("\nInstalling BITOS-32 to disk...\n", color);

    ata_write_sector(1, (unsigned char*)0x100000);

    say("Installation complete! Reboot.\n", color);
    read_keyboard();
}

// ============================================
// КОМАНДА СМЕНЫ ФОНА
// ============================================
void cmd_bgcolor(char *args) {
    if (!args) {
        say("Usage: bgcolor <0-F>\n", 0x0F);
        say("0=black 1=blue 2=green 3=cyan\n", 0x0F);
        say("4=red 5=magenta 6=brown 7=light gray\n", 0x0F);
        say("8=dark gray 9=light blue A=light green\n", 0x0F);
        say("B=light cyan C=light red D=light magenta\n", 0x0F);
        say("E=yellow F=white\n", 0x0F);
        return;
    }

    int color = 0;
    if (args[0] >= '0' && args[0] <= '9') {
        color = args[0] - '0';
    } else if (args[0] >= 'A' && args[0] <= 'F') {
        color = args[0] - 'A' + 10;
    } else if (args[0] >= 'a' && args[0] <= 'f') {
        color = args[0] - 'a' + 10;
    } else {
        say("Invalid color\n", 0x0C);
        return;
    }

    bg_color = color << 4;
    clear_screen();
    say("Background color changed\n", bg_color | 0x0A);
    update_prompt();
}

// ============================================
// ТАБЕЛЬНЫЕ КОМАНДЫ
// ============================================

void cmd_h(char *args) {
    int color = bg_color | 0x0F;
    int desc_color = bg_color | 0x0F;
    say("=== Table commands ===\n", 0x0E);
    for (int i = 0; commands[i].name != NULL; i++) {
        say("  ", color);
        say(commands[i].name, color);
        say(" - ", color);
        say(commands[i].desc, desc_color);
        say("\n", color);
    }
}

void cmd_echo(char *args) {
    int color = bg_color | 0x0F;
    if (args) {
        say(args, color);
        say("\n", color);
    }
}

void cmd_hello(char *args) {
    say("Hello, user!\n", bg_color | 0x0F);
}

void clear_cmd(char *args) {
    char* video = (char*) VIDEO_MEMORY;
    for (int i = 0; i < 80 * 25; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x00;
    }
}

void cmd_calc(char *args) {
    if (!args) {
        say("Usage: calc 2+3\n", 0x0F);
        return;
    }
    int a = 0, b = 0;
    char op = '+';
    int i = 0;

    while (args[i] >= '0' && args[i] <= '9') {
        a = a * 10 + (args[i] - '0');
        i++;
    }
    op = args[i++];
    while (args[i] >= '0' && args[i] <= '9') {
        b = b * 10 + (args[i] - '0');
        i++;
    }

    int result = 0;
    switch(op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/': result = a / b; break;
        default: say("Unknown operator\n", 0x0C); return;
    }

    int color = bg_color | 0x0F;
    int green = bg_color | 0x02;
    print_int(a, color);
    say(" ", color);
    putchar(op, color);
    say(" ", color);
    print_int(b, color);
    say(" = ", color);
    print_int(result, green);
    say("\n", green);
}

void cmd_reb(char *args) {
    say("Rebooting...\n", bg_color | 0x04);
    outb(0x64, 0xFE);
}

void cmd_sh(char *args) {
    say("Shutting down...\n", bg_color | 0x0F);
    outw(0x604, 0x2000);
    loop (1);
}

void cmd_time(char *args) {
    int sec = 0, min = 0, hour = 0;
    int day = 7, month = 6, year = 2026;
    int color = bg_color | 0x0F;

    loop (1) {
        say("\r", color);
        print_int(day, color); say(".", color);
        print_int(month, color); say(".", color);
        print_int(year, color); say(" ", color);
        print_int(hour, color); say(":", color);
        print_int(min, color); say(":", color);
        print_int(sec, color); say("   ", color);

        sec++;
        if (sec >= 60) { sec = 0; min++; }
        if (min >= 60) { min = 0; hour++; }
        if (hour >= 24) { hour = 0; day++; }

        if (day > 30) { day = 1; month++; }
        if (month > 12) { month = 1; year++; }

        cyc (int i = 0; i < 1000000; i++);

        char ch = read_keyboard();
        if (ch != 0) break;
    }
    say("\n", color);
}

// ============================================
// КОМАНДА EDIT ДЛЯ ПАРСЕРА
// ============================================

void cmd_edit_par(char *args) {
    char line[256];
    char full_text[4096] = "";
    int color = bg_color | 0x0F;
    int green = bg_color | 0x02;
    int red = bg_color | 0x04;

    say("========== BITOS EDITOR ==========\n", color);
    say("Commands: /wq - save, /q. - exit, /ls - show\n", 0x70);
    say("==================================\n", color);

    loop (1) {
        say("> ", color);

        int line_pos = 0;
        loop (1) {
            char c = read_keyboard();
            if (c == 0) continue;
            if (c == '\n') {
                line[line_pos] = '\0';
                say("\n", 0x0F);
                stop;
            }
            if (c == '\b' && line_pos > 0) {
                line_pos--;
                say("\b \b", 0x0F);
                continue;
            }
            if (line_pos < 255) {
                line[line_pos++] = c;
                putchar(c, 0x0F);
            }
        }

        if (line[0] == '/') {
            if (line[1] == 'w' && line[2] == 'q') {
                int i = 0;
                loop (full_text[i] && i < 4095) {
                    saved_note[i] = full_text[i];
                    i++;
                }
                saved_note[i] = '\0';
                say("\n[OK] Note saved\n", green);
                stop;
            }
            e if (line[1] == 'q' && line[2] == '.') {
                say("\n[OK] Exited\n", green);
                stop;
            }
            e if (line[1] == 'l' && line[2] == 's') {
                say("\n--- TEXT ---\n", 0x0F);
                if (full_text[0] == '\0') print("(empty)\n", 0x0F);
                e say(full_text, 0x0F);
                say("-------------\n", 0x0F);
            }
        } e {
            int i = 0;
            loop (full_text[i]) i++;
            int j = 0;
            loop (line[j] && i < 4095) full_text[i++] = line[j++];
            if (i < 4095) full_text[i++] = '\n';
            full_text[i] = '\0';
        }
    }
}

void parse_table_command(char *cmd) {
    char *args = cmd;
    while (*args && *args != ' ') args++;
    if (*args == ' ') {
        *args = '\0';
        args++;
    } else {
        args = NULL;
    }

    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(commands[i].name, cmd) == 0) {
            commands[i].func(args);
            return;
        }
    }
    say("Table command not found\n", 0x0C);
}

void cmd_par(char *args) {
    if (args == NULL || args[0] == '\0') {
        say("Usage: par <table_command>\n", 0x0F);
        say("Available: ", 0x0F);
        for (int i = 0; commands[i].name != NULL; i++) {
            say(commands[i].name, 0x0E);
            say(" ", 0x0E);
        }
        say("\n", 0x0F);
        return;
    }
    parse_table_command(args);
}

void cmd_sleep(char *args) {
    if (!args) {
        say("Usage: sleep <seconds>\n", 0x0F);
        return;
    }
    int sec = 0;
    while (*args >= '0' && *args <= '9') {
        sec = sec * 10 + (*args - '0');
        args++;
    }
    if (sec == 0) sec = 1;
    for (int i = 0; i < sec * 500000; i++);
    say("Done\n", 0x0A);
}

void cmd_type(char *args) {
    if (!args) {
        say("Usage: type <command>\n", 0x0F);
        return;
    }

    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(commands[i].name, args) == 0) {
            say(args, 0x0A);
            say(" is a table command\n", 0x0A);
            return;
        }
    }

    if (strcmp(args, "help") == 0 || strcmp(args, "clear") == 0 ||
        strcmp(args, "about") == 0 || strcmp(args, "ping") == 0 ||
        strcmp(args, "reboot") == 0 || strcmp(args, "shutdown") == 0 ||
        strcmp(args, "edit") == 0 || strcmp(args, "notes") == 0) {
        say(args, 0x0A);
        say(" is a builtin command\n", 0x0A);
        return;
    }

    say(args, 0x0C);
    say(": command not found\n", 0x0C);
}

// ============================================
// КОНЕЦ ТАБЕЛЬНЫХ КОМАНД
// ============================================

void readline(char *buf) {
    int i = 0;
    char c;
    char str[2] = {0, 0};
    int color = bg_color | 0x0F;
    while (1) {
        c = read_keyboard();
        if (c == '\n' || c == '\r') {
            buf[i] = '\0';
            say("\n", color);
            break;
        }
        if (c == '\b' && i > 0) {
            i--;
            say("\b \b", color);
            continue;
        }
        if (i < 63 && c >= ' ' && c <= '~') {
            buf[i++] = c;
            str[0] = c;
            say(str, color);
        }
    }
}

void init() {
    char* video = (char*) VIDEO_MEMORY;
    for (int i = 0; i < 80 * 25; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x00;
    }

    // Цвет для сообщения (зелёный на чёрном)
    int color = 0x02;
    say("                              [ OK ] Loading kernel...", color);
    for (int i = 0; i < 3000000; i++);
}

void main(void) {
    init();

    char* video = (char*) VIDEO_MEMORY;

    // Синяя полоса сверху
    for (int i = 0; i < 80; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = 0x17;
    }

    // Остальное — чёрный фон (bg_color = 0x00)
    bg_color = 0x00;
    int default_attr = bg_color | 0x0F;
    for (int i = 80; i < 80 * 25; i++) {
        video[i * 2] = ' ';
        video[i * 2 + 1] = default_attr;
    }

    char* welcome = "WELCOME TO BITOS-32";
    int len = 19;
    int start = (80 - len) / 2;
    for (int i = 0; i < len; i++) {
        video[(start + i) * 2] = welcome[i];
        video[(start + i) * 2 + 1] = 0x1F;
    }

    saved_note[0] = '\0';

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

    say("\n", default_attr);
    say("BitOS Kernel v2.0\n", bg_color | 0x02);

    update_prompt();

    char cmd[256];
    int idx = 0;

    loop (1) {

        char ch = read_keyboard();

        if (ch == 0) continue;

        if (ch == 0x3B) {
            int color = bg_color | 0x0F;
            say("\n============== HELP ================\n", color);
            say("Commands: help, clear, edit, notes,   \n", color);
            say("load, reboot, music, datetime, echo   \n", color);
            say("======================================\n", color);
            say("/home> ", bg_color | 0x02);
            continue;
        }

        if (ch == 0x3C) {
            int color = bg_color | 0x0F;
            say("\n======== SYSTEM INFO ========\n", color);
            say("BITOS-32 | 32-bit\n", color);
            say("ATA: OK | BFS: ", color);
            say(" files\n", color);
            say("===============================\n", color);
            say("/home> ", bg_color | 0x02);
            continue;
        }

        if (ch == '\n') {
            cmd[idx] = '\0';
            int color = bg_color | 0x0F;
            int green = bg_color | 0x02;
            int red = bg_color | 0x04;
            say("\n", color);

            if (strcmp(cmd, "help") == 0) {
                say("Commands: help, clear, about, ping, mode, cat, win, reboot, echo, fetch, say, edit, notes, 8ball, testdisk, save, load\n", color);
            }
            e if (strcmp(cmd, "clear") == 0) {
                clear_screen();
            }
            e if (strcmp(cmd, "about") == 0) {
                say("BitOS - 32-bit OS written in C and ASM\n", 0x72);
                say("Features: text mode, colors, keyboard, ATA, FAT16\n", 0x72);
            }
            e if (strcmp(cmd, "ping") == 0) {
                say("pong\n", green);
            }
            e if (strcmp(cmd, "disks") == 0) {
                char model[41];
                say("==== Detected ATA devices ====\n", color);

                if (ata_detect(0x1F0)) {
                    ata_get_model(0x1F0, model);
                    say("Primary Master: ", color);
                    say(model, color);
                    say("\n", color);
                } e {
                    say("Primary Master: not detected\n", red);
                }
                if (ata_detect(0x170)) {
                    ata_get_model(0x170, model);
                    say("Secondary Master: ", color);
                    say(model, color);
                    say("\n", color);
                } e {
                    say("Secondary Master: not detected\n", red);
                }
            }
            e if (strcmp(cmd, "testdisk") == 0) {
                uint8_t sector[512];
                ata_read_sector(0, sector);
                say("First byte: ", color);
                print_int(sector[0], color);
                say(" (should be 235 for FAT16)\n", color);
            }
            e if (strcmp(cmd, "pp2") == 0) {
                say("╔══════════╗\n", color);
                say("║        ║\n", color);
                say("╚══════════╝\n", color);
            }
            e if (strcmp(cmd, "mode") == 0) {
                say("Protected mode (32-bit)\n", color);
            }
            e if (strcmp(cmd, "w1") == 0) {
                wi();
                read_keyboard();
                wi_close();
            }
            e if (strcmp(cmd, "install") == 0) {
                install_os();
            }
            e if (strcmp(cmd, "par") == 0) {
                cmd_par(NULL);
            }
            e if (strncmp(cmd, "par ", 4) == 0) {
                cmd_par(cmd + 4);
            }
            e if (strcmp(cmd, "switch") == 0) {

                cyc (int i = 0; i < 80 * 25; i++) {
                    video[i * 2] = ' ';
                    video[i * 2 + 1] = 0x00;
                }

                cyc (int i = 0; i < 25; i++) print("\n", 0x00);
                say("\r", 0x00);

                say("+======================================================+\n", 0x72);
                say("|                   BITOS DESKTOP                      |\n", 0x71);
                say("+------------------------------------------------------+\n", 0x72);
                say("|     [1] Exit  [2] Reboot  [3] Notes                  |\n", 0x71);
                say("|     [4] Edit  [5] Save to disk [6] load from disk    |\n", 0x71);
                say("+======================================================+\n", 0x72);

                loop (1) {
                    char ch2 = read_keyboard();
                    if (ch2 == '1') {
                        stop;
                    }
                    e if (ch2 == '2') {
                        outb(0x64, 0xFE);
                    }
                    e if (ch2 == '5') {
                        save_note_to_disk();
                    }
                    e if (ch2 == '6') {
                        load_note_from_disk();
                    }
                }
                cyc (int i = 0; i < 80 * 25; i++) {
                    video[i * 2] = ' ';
                    video[i * 2 + 1] = 0x00;
                }

                say("> ", green);
            }
            e if (strcmp(cmd, "bitos-c") == 0) {
                say("bitos-c is a programming language embedded\n", color);
                say("inside the BitOS-32 operating system.\n", color);
            }
            e if (strcmp(cmd, "cat") == 0) {
                say(" /\\_/\\\n", green);
                say("( o.o )\n", green);
                say(" > ^ <\n", green);
            }
            e if (strcmp(cmd, "save") == 0) {
                save_note_to_disk();
            }
            e if (strcmp(cmd, "load") == 0) {
                load_note_from_disk();
            }
            e if (strcmp(cmd, "8ball") == 0) {
                char* answers[] = {"Yes", "No", "Maybe"};
                int count = 3;
                static int seed = 0;
                seed++;
                int r = (seed * 12345) % count;
                print(answers[r], color);
                print("\n", color);
            }
            e if (strcmp(cmd, "banner") == 0) {
                banner();
            }
            e if (strcmp(cmd, "datetime") == 0) {
                int sec = 0, min = 0, hour = 0;
                int day = 7, month = 6, year = 2026;

                loop (1) {
                    say("\r", color);
                    print_int(day, color); say(".", color);
                    print_int(month, color); say(".", color);
                    print_int(year, color); say(" ", color);
                    print_int(hour, color); say(":", color);
                    print_int(min, color); say(":", color);
                    print_int(sec, color); say("   ", color);

                    sec++;
                    if (sec >= 60) { sec = 0; min++; }
                    if (min >= 60) { min = 0; hour++; }
                    if (hour >= 24) { hour = 0; day++; }

                    if (day > 30) { day = 1; month++; }
                    if (month > 12) { month = 1; year++; }

                    cyc (int i = 0; i < 1000000; i++);

                    char ch2 = read_keyboard();
                    if (ch2 != 0) stop;
                }
                say("\n", color);
            }
            e if (strcmp(cmd, "win") == 0) {
                say("+--------------[x]-+\n", 0x70);
                say("|     BITOS-32     |\n", 0x71);
                say("|   Hello, User!   |\n", 0x70);
                say("+------------------+\n", 0x70);
            }
            e if (strcmp(cmd, "say") == 0) {
                say("BITOS is awesome!\n", color);
            }
            e if (strcmp(cmd, "cube") == 0) {
                say("            $++++++\n", color);
                say("           $$$$+++++++++++++\n", color);
                say("          $$$$$$++++++++++++++++++++\n", color);
                say("         $$$$$$$$$++++++++++++++++++++++\n", color);
                say("        $$$$$$$$$$$$++++++++++++++++++++++\n", color);
                say("       $$$$$$$$$$$$$$$++++++++++++++++++++++\n", color);
                say("      $$$$$$$$$$$$$$$$$$++++++++++++++++++++++\n", color);
                say("     $$$$$$$$$$$$$$$$$$$$$++++++++++++++++++++++\n", color);
                say("    $$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", color);
                say("   $$$$$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", color);
                say("  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", color);
                say(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", color);
                say("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$......................\n", color);
                say(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$......................\n", color);
                say("  $$$$$$$$$$$$$$$$$$$$$$$$$$$......................\n", color);
                say("   $$$$$$$$$$$$$$$$$$$$$$$$......................\n", color);
                say("    $$$$$$$$$$$$$$$$$$$$$.......................\n", color);
                say("     $$$$$$$$$$$$$$$$$$.......................\n", color);
                say("      $$$$$$$$$$$$$$$$.......................\n", color);
                say("       $$$$$$$$$$$$$.......................\n", color);
                say("        $$$$$$$$$$.......................\n", color);
                say("         $$$$$$$........................\n", color);
                say("          $$$$.......................\n", color);
                say("           $................\n", color);
            }
            e if (strcmp(cmd, "req") == 0) {
                say("=== BitOS-32 System Requirements ===\n", 0x71);
                say("CPU: i386 or higher                 \n", 0x71);
                say("RAM: 4 MB minimum                   \n", 0x71);
                say("Disk: ATA compatible                \n", 0x71);
                say("Video: VGA text mode 80x25          \n", 0x71);
                say("Input: Keyboard                     \n", 0x71);
                say("====================================\n", 0x71);
            }
            e if (strcmp(cmd, "window") == 0) {
                say("+------------------------------------------------------------------------------+\n", color);
                say("|                               W I N D O W                                    |\n", color);
                say("|------------------------------------------------------------------------------|\n", color);
                say("|                               FULL SCREEN                                    |\n", color);
                say("|------------------------------------------------------------------------------|\n", color);
                say("|   Terminal       : text mode 80x25 symbols                                   |\n", color);
                say("|   Character size : 8x16 pixels                                               |\n", color);
                say("+------------------------------------------------------------------------------+\n", color);
            }
            e if (strcmp(cmd, "new") == 0) {
                say("hello\n", color);
            }
            e if (strcmp(cmd, "cpu") == 0) {
                sysinf();
            }
            e if (strcmp(cmd, "history") == 0) {
                show_history();
            }
            e if (strcmp(cmd, "line") == 0) {
                say("________________________________________________________________________________", 0x0F);
            }
            e if (strcmp(cmd, "edit") == 0) {
                char line[256];
                char full_text[4096] = "";

                say("========== BITOS EDITOR ==========\n", color);
                say("Commands: /wq - save, /q. - exit, /ls - show\n", 0x70);
                say("==================================\n", color);

                loop (1) {
                    say("> ", color);

                    int line_pos = 0;
                    loop (1) {
                        char c = read_keyboard();
                        if (c == 0) continue;
                        if (c == '\n') {
                            line[line_pos] = '\0';
                            say("\n", 0x0F);
                            stop;
                        }
                        if (line_pos < 255) {
                            line[line_pos++] = c;
                            putchar(c, 0x0F);
                        }
                    }

                    if (line[0] == '/') {
                        if (line[1] == 'w' && line[2] == 'q') {
                            int i = 0;
                            loop (full_text[i] && i < 4095) {
                                saved_note[i] = full_text[i];
                                i++;
                            }
                            saved_note[i] = '\0';
                            say("\n[OK] Note saved\n", green);
                            stop;
                        }
                        e if (line[1] == 'q' && line[2] == '.') {
                            say("\n[OK] Exited\n", green);
                            stop;
                        }
                        e if (line[1] == 'l' && line[2] == 's') {
                            say("\n--- TEXT ---\n", 0x0F);
                            if (full_text[0] == '\0') print("(empty)\n", 0x0F);
                            e say(full_text, 0x0F);
                            say("-------------\n", 0x0F);
                        }
                    } e {
                        int i = 0;
                        loop (full_text[i]) i++;
                        int j = 0;
                        loop (line[j] && i < 4095) full_text[i++] = line[j++];
                        if (i < 4095) full_text[i++] = '\n';
                        full_text[i] = '\0';
                    }
                }
            }
            e if (strcmp(cmd, "notes") == 0) {
                if (saved_note[0] == '\0') {
                    say("No saved note\n", 0x74);
                } else {
                    say("=== SAVED NOTE ===\n", 0x72);
                    say(saved_note, 0x70);
                    say("==================\n", 0x72);
                }
            }

            e if (strcmp(cmd, "kernel") == 0)  {
                say("\n", color);
                say("Version kernel: sysb9\n", color);
                say("Version OS: 2.0\n", color);
            }
            e if (strcmp(cmd, "files") == 0) {
                loop (1) {
                    clear_screen();
                    say("\n========== File Manager ============\n", 0x72);
                    say("[1] list files [2] Show memory (mem)  \n", 0x70);
                    say("[3] peek video memory [4] exit [5] cat\n", 0x70);
                    say("[6] Delet file                        \n", 0x70);
                    say("======================================\n", 0x72);
                    say("Choice: \n", color);
                    char ch2 = read_keyboard();

                    if (ch2 == '1') {
                        uint8_t sector[512];
                        ata_read_sector(19, sector);
                        say("====== FILES ======\n", color);
                        cyc (int i = 0; i < 512; i += 32) {
                            if (sector[i] == 0x00) stop;
                            if (sector[i] == 0xE5) continue;
                            for (int j = 0; j < 8; j++) putchar(sector[i + j], color);
                            say("\n", color);
                        }
                        say("===================\n", color);
                    }
                    e if (ch2 == '2') {
                        say("Video RAM: 0xB8000\n", color);
                        say("Heap: 1 MB\n", color);
                    }
                    e if (ch2 == '3') {
                        char* addr = (char*)0xB8000;
                        say("First byte: ", color);
                        print_int(addr[0], color);
                        say("\n", color);
                    }
                    e if (ch2 == '6') {
                        uint8_t empty[512] = {0};
                        ata_write_sector(100, empty);
                        say("Note erased.\n", green);
                    }
                    e if (ch2 == '8') {
                        say("File to delete: ", color);
                        char name[16];
                        int i = 0;
                        loop (1) {
                            char c = read_keyboard();
                            if (c == '\n') stop;
                            if (i < 15) name[i++] = c;
                            putchar(c, color);
                        }
                    }
                    e if (ch2 == '5') {
                        say(saved_note, color);
                        say("\n", color);
                    }
                    e if (ch2 == '4') {
                        stop;
                    }
                }
            }
            e if (strcmp(cmd, "fetch") == 0) {
                say("BITOS-32 | 32-bit | ATA | FAT16\n", color);
            }
            e if (strcmp(cmd, "shutdown") == 0) {
                say("Shutting down...\n", color);
                outw(0x604, 0x2000);
                loop (1);
            }
            e if (strcmp(cmd, "music") == 0) {
                loop (1) {
                    clear_screen();
                    say("======= MUSIC PLAYER =======\n", 0x72);
                    say("[1] Beep (short)            \n", 0x70);
                    say("[2] Melody (do-re-mi-fa-sol)\n", 0x70);
                    say("[3] Alarm (5 beeps)         \n", 0x70);
                    say("[4] Exit                    \n", 0x70);
                    say("============================\n", 0x72);
                    say("Choice: ", color);

                    char ch2 = read_keyboard();

                    if (ch2 == '1') {
                        outb(0x60, inb(0x61) | 3);
                        cyc (int i = 0; i < 100000; i++);
                        outb(0x61, inb(0x61) & 0xFC);
                    }
                    e if (ch2 == '2') {
                        int freqs[] = {262, 294, 330, 349, 392, 440, 494, 523};
                        cyc (int n = 0; n < 8; n++) {
                            int div = 1193180 / freqs[n];
                            outb(0x43, 0xB6);
                            outb(0x42, div & 0xFF);
                            outb(0x42, (div >> 8) & 0xFF);
                            outb(0x61, inb(0x61) | 3);
                            cyc (int i = 0; i < 30000; i++);
                            outb(0x61, inb(0x61) & 0xFC);
                            cyc (int i = 0; i < 5000; i++);
                        }
                    }
                    e if (ch2 == '3') {
                        cyc (int i = 0; i < 5; i++) {
                            outb(0x61, inb(0x61) | 3);
                            cyc (int j = 0; j < 150000; j++);
                            outb(0x61, inb(0x61) & 0xFC);
                            cyc (int j = 0; j < 100000; j++);
                        }
                    }
                    e if (ch2 == '4') {
                        stop;
                    }
                }
                clear_screen();
            }
            e if (strcmp(cmd, "reboot") == 0) {
                say("Rebooting...\n", red);
                outb(0x64, 0xFE);
            }
            e if (cmd[0] == 'e' && cmd[1] == 'c' && cmd[2] == 'h' && cmd[3] == 'o' && cmd[4] == ' ') {
                say(cmd + 5, color);
                say("\n", color);
            }
            e if (cmd[0] != '\0') {
                say("Unknown command\n", red);
            }
            say("> ", green);
            idx = 0;
        } e if (ch == '\b') {
            if (idx > 0) {
                idx--;
                say("\b \b", bg_color | 0x0F);
            }
        } e if (idx < 255) {
            cmd[idx++] = ch;
            putchar(ch, bg_color | 0x0F);
        }
    }
}
