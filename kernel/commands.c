#include <stdint.h>
#include <stddef.h>

#define WHITE_ON_BLACK  0x0F
#define GREEN_ON_BLACK  0x02
#define RED_ON_BLACK    0x04
#define MAX_HIST 20

#define say(s, c) print(s, c)

extern void print(const char* str, int color);
extern void print_int(int num, int color);
extern void putchar(char c, int color);
extern void clear_screen(void);
extern void update_prompt(void);
extern char read_keyboard(void);
extern void outb(unsigned short port, unsigned char value);
extern void outw(uint16_t port, uint16_t value);
extern void ata_read_sector(uint32_t lba, uint8_t* buffer);
extern void ata_write_sector(uint32_t lba, uint8_t* buffer);
extern int ata_detect(int port);
extern void ata_get_model(int port, char *model);
extern void sysinf(void);
extern void show_history(void);
extern int bg_color;
extern char saved_note[4096];
extern int strcmp(const char* a, const char* b);
extern void cmd_par(char *args);
extern void parse_table_command(char *cmd);

typedef struct {
    char *name;
    void (*func)(char *args);
    char *desc;
} command_t;

extern command_t commands[];

void cmd_help() {
    say("Commands: help, clear, about, ping, mode, cat, win, reboot, echo, fetch, say, edit, notes, 8ball, testdisk, save, load\n", WHITE_ON_BLACK);
}

void cmd_clear() { clear_screen(); }

void cmd_about() {
    say("OceroOS - 32-bit OS written in C and ASM\n", 0x72);
    say("Features: text mode, colors, keyboard, ATA, FAT16\n", 0x72);
}

void cmd_ping() { say("pong\n", GREEN_ON_BLACK); }

void cmd_disks() {
    char model[41];
    say("==== Detected ATA devices ====\n", WHITE_ON_BLACK);
    if (ata_detect(0x1F0)) {
        ata_get_model(0x1F0, model);
        say("Primary Master: ", WHITE_ON_BLACK);
        say(model, WHITE_ON_BLACK);
        say("\n", WHITE_ON_BLACK);
    } else {
        say("Primary Master: not detected\n", RED_ON_BLACK);
    }
    if (ata_detect(0x170)) {
        ata_get_model(0x170, model);
        say("Secondary Master: ", WHITE_ON_BLACK);
        say(model, WHITE_ON_BLACK);
        say("\n", WHITE_ON_BLACK);
    } else {
        say("Secondary Master: not detected\n", RED_ON_BLACK);
    }
}

void cmd_testdisk() {
    uint8_t sector[512];
    ata_read_sector(0, sector);
    say("First byte: ", WHITE_ON_BLACK);
    print_int(sector[0], WHITE_ON_BLACK);
    say(" (should be 235 for FAT16)\n", WHITE_ON_BLACK);
}

void cmd_mode() { say("Protected mode (32-bit)\n", WHITE_ON_BLACK); }

void cmd_win() {
    say("+--------------[x]-+\n", 0x70);
    say("|     OceroOS      |\n", 0x71);
    say("|   Hello, User!   |\n", 0x70);
    say("+------------------+\n", 0x70);
}

void cmd_say() { say("BITOS is awesome!\n", WHITE_ON_BLACK); }

void cmd_icat() {
    say(" /\\_/\\\n", GREEN_ON_BLACK);
    say("( o.o )\n", GREEN_ON_BLACK);
    say(" > ^ <\n", GREEN_ON_BLACK);
}

void cmd_fetch() { say("OceroOS | 32-bit | ATA | FAT16\n", WHITE_ON_BLACK); }

void cmd_cpu() { sysinf(); }

void cmd_version() { say("OceroOS-32 v2.1\n", WHITE_ON_BLACK); }

void cmd_kernel() {
    say("\n", WHITE_ON_BLACK);
    say("Version kernel: sysb36\n", WHITE_ON_BLACK);
    say("Version OS: 2.1\n", WHITE_ON_BLACK);
}

void cmd_cube() {
    say("            $++++++\n", WHITE_ON_BLACK);
    say("           $$$$+++++++++++++\n", WHITE_ON_BLACK);
    say("          $$$$$$++++++++++++++++++++\n", WHITE_ON_BLACK);
    say("         $$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    say("        $$$$$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    say("       $$$$$$$$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    say("      $$$$$$$$$$$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    say("     $$$$$$$$$$$$$$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    say("    $$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", WHITE_ON_BLACK);
    say("   $$$$$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", WHITE_ON_BLACK);
    say("  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", WHITE_ON_BLACK);
    say(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", WHITE_ON_BLACK);
    say("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$......................\n", WHITE_ON_BLACK);
    say(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$......................\n", WHITE_ON_BLACK);
    say("  $$$$$$$$$$$$$$$$$$$$$$$$$$$......................\n", WHITE_ON_BLACK);
    say("   $$$$$$$$$$$$$$$$$$$$$$$$......................\n", WHITE_ON_BLACK);
    say("    $$$$$$$$$$$$$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    say("     $$$$$$$$$$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    say("      $$$$$$$$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    say("       $$$$$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    say("        $$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    say("         $$$$$$$........................\n", WHITE_ON_BLACK);
    say("          $$$$.......................\n", WHITE_ON_BLACK);
    say("           $................\n", WHITE_ON_BLACK);
}

void cmd_req() {
    say("=== OceroOS-32 System Requirements ===\n", 0x71);
    say("CPU: i386 or higher                 \n", 0x71);
    say("RAM: 4 MB minimum                   \n", 0x71);
    say("Disk: ATA compatible                \n", 0x71);
    say("Video: VGA text mode 80x25          \n", 0x71);
    say("Input: Keyboard                     \n", 0x71);
    say("====================================\n", 0x71);
}

void cmd_window() {
    say("+------------------------------------------------------------------------------+\n", WHITE_ON_BLACK);
    say("|                               W I N D O W                                    |\n", WHITE_ON_BLACK);
    say("|------------------------------------------------------------------------------|\n", WHITE_ON_BLACK);
    say("|                               FULL SCREEN                                    |\n", WHITE_ON_BLACK);
    say("|------------------------------------------------------------------------------|\n", WHITE_ON_BLACK);
    say("|   Terminal       : text mode 80x25 symbols                                   |\n", WHITE_ON_BLACK);
    say("|   Character size : 8x16 pixels                                               |\n", WHITE_ON_BLACK);
    say("+------------------------------------------------------------------------------+\n", WHITE_ON_BLACK);
}

void cmd_new() { say("hello\n", WHITE_ON_BLACK); }

void cmd_line() { say("________________________________________________________________________________", 0x0F); }

void cmd_8ball() {
    char* answers[] = {"Yes", "No", "Maybe"};
    static int seed = 0;
    seed++;
    int r = (seed * 12345) % 3;
    say(answers[r], WHITE_ON_BLACK);
    say("\n", WHITE_ON_BLACK);
}

void cmd_notes() {
    if (saved_note[0] == '\0') say("No saved note\n", 0x74);
    else {
        say("=== SAVED NOTE ===\n", 0x72);
        say(saved_note, 0x70);
        say("==================\n", 0x72);
    }
}

void cmd_edit() {
    char line[256], full_text[4096] = "";
    say("========== Ocero EDITOR ==========\n", WHITE_ON_BLACK);
    say("Commands: /wq - save, /q. - exit, /ls - show\n", 0x70);
    say("==================================\n", WHITE_ON_BLACK);
    while (1) {
        say("> ", WHITE_ON_BLACK);
        int line_pos = 0;
        while (1) {
            char c = read_keyboard();
            if (c == 0) continue;
            if (c == '\n') {
                line[line_pos] = '\0';
                say("\n", 0x0F);
                break;
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
                while (full_text[i] && i < 4095) {
                    saved_note[i] = full_text[i];
                    i++;
                }
                saved_note[i] = '\0';
                say("\n[OK] Note saved\n", GREEN_ON_BLACK);
                break;
            }
            if (line[1] == 'q' && line[2] == '.') {
                say("\n[OK] Exited\n", GREEN_ON_BLACK);
                break;
            }
            if (line[1] == 'l' && line[2] == 's') {
                say("\n--- TEXT ---\n", 0x0F);
                if (full_text[0] == '\0') say("(empty)\n", 0x0F);
                else say(full_text, 0x0F);
                say("-------------\n", 0x0F);
            }
        } else {
            int i = 0;
            while (full_text[i]) i++;
            int j = 0;
            while (line[j] && i < 4095) full_text[i++] = line[j++];
            if (i < 4095) full_text[i++] = '\n';
            full_text[i] = '\0';
        }
    }
}

void cmd_reboot() { say("Rebooting...\n", RED_ON_BLACK); outb(0x64, 0xFE); }

void cmd_shutdown() { say("Shutting down...\n", WHITE_ON_BLACK); outw(0x604, 0x2000); while (1); }

void cmd_save() { ata_write_sector(100, (uint8_t*)saved_note); say("Note saved to disk (sector 100)\n", GREEN_ON_BLACK); }

void cmd_load() { ata_read_sector(100, (uint8_t*)saved_note); say("Note loaded from disk\n", GREEN_ON_BLACK); }

void cmd_switch() {
    char* video = (char*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) video[i * 2] = ' ', video[i * 2 + 1] = 0x00;
    for (int i = 0; i < 25; i++) say("\n", 0x00);
    say("\r", 0x00);
    say("+======================================================+\n", 0x72);
    say("|                   OceroOS DESKTOP                    |\n", 0x71);
    say("+------------------------------------------------------+\n", 0x72);
    say("|     [1] Exit  [2] Reboot  [3] Notes                  |\n", 0x71);
    say("|     [4] Edit  [5] Save to disk [6] load from disk    |\n", 0x71);
    say("+======================================================+\n", 0x72);
    while (1) {
        char ch = read_keyboard();
        if (ch == '1') break;
        if (ch == '2') outb(0x64, 0xFE);
        if (ch == '5') cmd_save();
        if (ch == '6') cmd_load();
    }
    for (int i = 0; i < 80 * 25; i++) video[i * 2] = ' ', video[i * 2 + 1] = 0x00;
}

void cmd_time(char *args) {
    int sec = 0, min = 0, hour = 0, day = 7, month = 6, year = 2026;
    while (1) {
        say("\r", WHITE_ON_BLACK);
        print_int(day, WHITE_ON_BLACK); say(".", WHITE_ON_BLACK);
        print_int(month, WHITE_ON_BLACK); say(".", WHITE_ON_BLACK);
        print_int(year, WHITE_ON_BLACK); say(" ", WHITE_ON_BLACK);
        print_int(hour, WHITE_ON_BLACK); say(":", WHITE_ON_BLACK);
        print_int(min, WHITE_ON_BLACK); say(":", WHITE_ON_BLACK);
        print_int(sec, WHITE_ON_BLACK); say("   ", WHITE_ON_BLACK);
        sec++;
        if (sec >= 60) { sec = 0; min++; }
        if (min >= 60) { min = 0; hour++; }
        if (hour >= 24) { hour = 0; day++; }
        if (day > 30) { day = 1; month++; }
        if (month > 12) { month = 1; year++; }
        for (int i = 0; i < 1000000; i++);
        if (read_keyboard() != 0) break;
    }
    say("\n", WHITE_ON_BLACK);
}

void cmd_reb(char *args) { say("Rebooting...\n", RED_ON_BLACK); outb(0x64, 0xFE); }
void cmd_sh(char *args) { say("Shutting down...\n", WHITE_ON_BLACK); outw(0x604, 0x2000); while (1); }

void cmd_h(char *args) {
    say("=== Table commands ===\n", 0x0E);
    for (int i = 0; commands[i].name != NULL; i++) {
        say("  ", WHITE_ON_BLACK);
        say(commands[i].name, WHITE_ON_BLACK);
        say(" - ", WHITE_ON_BLACK);
        say(commands[i].desc, WHITE_ON_BLACK);
        say("\n", WHITE_ON_BLACK);
    }
}

void cmd_hello(char *args) { say("Hello, user!\n", WHITE_ON_BLACK); }
void cmd_echo(char *args) { if (args) { say(args, WHITE_ON_BLACK); say("\n", WHITE_ON_BLACK); } }

void cmd_calc(char *args) {
    if (!args) { say("Usage: calc 2+3\n", 0x0F); return; }
    int a = 0, b = 0, i = 0;
    char op = '+';
    while (args[i] >= '0' && args[i] <= '9') { a = a * 10 + (args[i] - '0'); i++; }
    op = args[i++];
    while (args[i] >= '0' && args[i] <= '9') { b = b * 10 + (args[i] - '0'); i++; }
    int result = 0;
    switch(op) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/': result = a / b; break;
        default: say("Unknown operator\n", 0x0C); return;
    }
    print_int(a, WHITE_ON_BLACK); say(" ", WHITE_ON_BLACK);
    putchar(op, WHITE_ON_BLACK); say(" ", WHITE_ON_BLACK);
    print_int(b, WHITE_ON_BLACK); say(" = ", WHITE_ON_BLACK);
    print_int(result, GREEN_ON_BLACK); say("\n", GREEN_ON_BLACK);
}

void cmd_sleep(char *args) {
    if (!args) { say("Usage: sleep <seconds>\n", 0x0F); return; }
    int sec = 0;
    while (*args >= '0' && *args <= '9') { sec = sec * 10 + (*args - '0'); args++; }
    if (sec == 0) sec = 1;
    for (int i = 0; i < sec * 500000; i++);
    say("Done\n", 0x0A);
}

void cmd_type(char *args) {
    if (!args) { say("Usage: type <command>\n", 0x0F); return; }
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(commands[i].name, args) == 0) {
            say(args, 0x0A); say(" is a table command\n", 0x0A); return;
        }
    }
    say(args, 0x0C); say(": command not found\n", 0x0C);
}

void cmd_bgcolor(char *args) {
    if (!args) { say("Usage: bgcolor <0-F>\n", 0x0F); return; }
    int color = 0;
    if (args[0] >= '0' && args[0] <= '9') color = args[0] - '0';
    else if (args[0] >= 'A' && args[0] <= 'F') color = args[0] - 'A' + 10;
    else if (args[0] >= 'a' && args[0] <= 'f') color = args[0] - 'a' + 10;
    else { say("Invalid color\n", 0x0C); return; }
    bg_color = color << 4;
    clear_screen();
    say("Background color changed\n", bg_color | 0x0A);
    update_prompt();
}

void cmd_edit_par(char *args) { cmd_edit(); }

void clear_cmd(char *args) {
    char* video = (char*) 0xB8000;
    for (int i = 0; i < 80 * 25; i++) video[i * 2] = ' ', video[i * 2 + 1] = 0x00;
}

