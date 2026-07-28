#include <stdint.h>
#include <stddef.h>

#define WHITE_ON_BLACK  0x0F
#define GREEN_ON_BLACK  0x02
#define RED_ON_BLACK    0x04
#define MAX_HIST 20
#define loop while
#define e else
#define cyc for
#define stop break

extern void print(const char* str, int color);
extern void print_int(int num, int color);
extern void putchar(char c, int color);
extern void clear_screen(void);
extern void update_prompt(void);
extern void gotoxy(int x, int y);
extern char read_keyboard(void);
extern void outb(unsigned short port, unsigned char value);
extern void outw(uint16_t port, uint16_t value);
extern void ata_read_sector(uint32_t lba, uint8_t* buffer);
extern int ata_detect(int port);
extern void ata_get_model(int port, char *model);
extern int bg_color;
extern char saved_note[4096];
extern char *history[MAX_HIST];
extern int hist_count;
extern int strcmp(const char* a, const char* b);
extern void sysinf(void);
extern void save_note_to_disk(void);
extern void load_note_from_disk(void);
extern void cmd_par(char *args);
extern void parse_table_command(char *cmd);

typedef struct {
    char *name;
    void (*func)(char *args);
    char *desc;
} command_t;

extern command_t commands[];

void cmd_help() {
    print("Commands: help, clear, about, ping, mode, cat, win, reboot, echo, fetch, say, edit, notes, 8ball, testdisk, save, load\n", WHITE_ON_BLACK);
}

void cmd_clear() { clear_screen(); }

void cmd_about() {
    print("BitOS - 32-bit OS written in C and ASM\n", 0x72);
    print("Features: text mode, colors, keyboard, ATA, FAT16\n", 0x72);
}

void cmd_ping() { print("pong\n", GREEN_ON_BLACK); }

void cmd_disks() {
    char model[41];
    print("==== Detected ATA devices ====\n", WHITE_ON_BLACK);
    if (ata_detect(0x1F0)) {
        ata_get_model(0x1F0, model);
        print("Primary Master: ", WHITE_ON_BLACK);
        print(model, WHITE_ON_BLACK);
        print("\n", WHITE_ON_BLACK);
    } else {
        print("Primary Master: not detected\n", RED_ON_BLACK);
    }
    if (ata_detect(0x170)) {
        ata_get_model(0x170, model);
        print("Secondary Master: ", WHITE_ON_BLACK);
        print(model, WHITE_ON_BLACK);
        print("\n", WHITE_ON_BLACK);
    } else {
        print("Secondary Master: not detected\n", RED_ON_BLACK);
    }
}

void cmd_testdisk() {
    uint8_t sector[512];
    ata_read_sector(0, sector);
    print("First byte: ", WHITE_ON_BLACK);
    print_int(sector[0], WHITE_ON_BLACK);
    print(" (should be 235 for FAT16)\n", WHITE_ON_BLACK);
}

void cmd_mode() { print("Protected mode (32-bit)\n", WHITE_ON_BLACK); }

void cmd_win() {
    print("+--------------[x]-+\n", 0x70);
    print("|     BITOS-32     |\n", 0x71);
    print("|   Hello, User!   |\n", 0x70);
    print("+------------------+\n", 0x70);
}

void cmd_say() { print("BITOS is awesome!\n", WHITE_ON_BLACK); }

void cmd_cat() {
    print(" /\\_/\\\n", GREEN_ON_BLACK);
    print("( o.o )\n", GREEN_ON_BLACK);
    print(" > ^ <\n", GREEN_ON_BLACK);
}

void cmd_fetch() { print("BITOS-32 | 32-bit | ATA | FAT16\n", WHITE_ON_BLACK); }

void cmd_cpu() { sysinf(); }

void cmd_version() { print("BitOS-32 v2.0\n", WHITE_ON_BLACK); }

void cmd_kernel() {
    print("\n", WHITE_ON_BLACK);
    print("Version kernel: sysb33\n", WHITE_ON_BLACK);
    print("Version OS: 2.0\n", WHITE_ON_BLACK);
}

void cmd_cube() {
    print("            $++++++\n", WHITE_ON_BLACK);
    print("           $$$$+++++++++++++\n", WHITE_ON_BLACK);
    print("          $$$$$$++++++++++++++++++++\n", WHITE_ON_BLACK);
    print("         $$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    print("        $$$$$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    print("       $$$$$$$$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    print("      $$$$$$$$$$$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    print("     $$$$$$$$$$$$$$$$$$$$$++++++++++++++++++++++\n", WHITE_ON_BLACK);
    print("    $$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", WHITE_ON_BLACK);
    print("   $$$$$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", WHITE_ON_BLACK);
    print("  $$$$$$$$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", WHITE_ON_BLACK);
    print(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$+++++++++++++++++++++++\n", WHITE_ON_BLACK);
    print("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$......................\n", WHITE_ON_BLACK);
    print(" $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$......................\n", WHITE_ON_BLACK);
    print("  $$$$$$$$$$$$$$$$$$$$$$$$$$$......................\n", WHITE_ON_BLACK);
    print("   $$$$$$$$$$$$$$$$$$$$$$$$......................\n", WHITE_ON_BLACK);
    print("    $$$$$$$$$$$$$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    print("     $$$$$$$$$$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    print("      $$$$$$$$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    print("       $$$$$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    print("        $$$$$$$$$$.......................\n", WHITE_ON_BLACK);
    print("         $$$$$$$........................\n", WHITE_ON_BLACK);
    print("          $$$$.......................\n", WHITE_ON_BLACK);
    print("           $................\n", WHITE_ON_BLACK);
}

void cmd_req() {
    print("=== BitOS-32 System Requirements ===\n", 0x71);
    print("CPU: i386 or higher                 \n", 0x71);
    print("RAM: 4 MB minimum                   \n", 0x71);
    print("Disk: ATA compatible                \n", 0x71);
    print("Video: VGA text mode 80x25          \n", 0x71);
    print("Input: Keyboard                     \n", 0x71);
    print("====================================\n", 0x71);
}

void cmd_window() {
    print("+------------------------------------------------------------------------------+\n", WHITE_ON_BLACK);
    print("|                               W I N D O W                                    |\n", WHITE_ON_BLACK);
    print("|------------------------------------------------------------------------------|\n", WHITE_ON_BLACK);
    print("|                               FULL SCREEN                                    |\n", WHITE_ON_BLACK);
    print("|------------------------------------------------------------------------------|\n", WHITE_ON_BLACK);
    print("|   Terminal       : text mode 80x25 symbols                                   |\n", WHITE_ON_BLACK);
    print("|   Character size : 8x16 pixels                                               |\n", WHITE_ON_BLACK);
    print("+------------------------------------------------------------------------------+\n", WHITE_ON_BLACK);
}

void cmd_new() { print("hello\n", WHITE_ON_BLACK); }
void cmd_line() { print("________________________________________________________________________________", 0x0F); }

void cmd_8ball() {
    char* answers[] = {"Yes", "No", "Maybe"};
    static int seed = 0;
    seed++;
    int r = (seed * 12345) % 3;
    print(answers[r], WHITE_ON_BLACK);
    print("\n", WHITE_ON_BLACK);
}

void cmd_notes() {
    if (saved_note[0] == '\0') print("No saved note\n", 0x74);
    else {
        print("=== SAVED NOTE ===\n", 0x72);
        print(saved_note, 0x70);
        print("==================\n", 0x72);
    }
}

void cmd_edit() {
    char line[256], full_text[4096] = "";
    print("========== BITOS EDITOR ==========\n", WHITE_ON_BLACK);
    print("Commands: /wq - save, /q. - exit, /ls - show\n", 0x70);
    print("==================================\n", WHITE_ON_BLACK);
    while (1) {
        print("> ", WHITE_ON_BLACK);
        int line_pos = 0;
        while (1) {
            char c = read_keyboard();
            if (c == 0) continue;
            if (c == '\n') {
                line[line_pos] = '\0';
                print("\n", 0x0F);
                break;
            }
            if (c == '\b' && line_pos > 0) {
                line_pos--;
                print("\b \b", 0x0F);
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
                print("\n[OK] Note saved\n", GREEN_ON_BLACK);
                break;
            }
            if (line[1] == 'q' && line[2] == '.') {
                print("\n[OK] Exited\n", GREEN_ON_BLACK);
                break;
            }
            if (line[1] == 'l' && line[2] == 's') {
                print("\n--- TEXT ---\n", 0x0F);
                if (full_text[0] == '\0') print("(empty)\n", 0x0F);
                else print(full_text, 0x0F);
                print("-------------\n", 0x0F);
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

void cmd_reboot() { print("Rebooting...\n", RED_ON_BLACK); outb(0x64, 0xFE); }
void cmd_shutdown() { print("Shutting down...\n", WHITE_ON_BLACK); outw(0x604, 0x2000); while (1); }

// ============================================
// ТАБЛИЧНЫЕ КОМАНДЫ
// ============================================

void cmd_time(char *args) {
    int sec = 0, min = 0, hour = 0, day = 7, month = 6, year = 2026, color = bg_color | 0x0F;
    while (1) {
        print("\r", color);
        print_int(day, color); print(".", color);
        print_int(month, color); print(".", color);
        print_int(year, color); print(" ", color);
        print_int(hour, color); print(":", color);
        print_int(min, color); print(":", color);
        print_int(sec, color); print("   ", color);
        sec++;
        if (sec >= 60) { sec = 0; min++; }
        if (min >= 60) { min = 0; hour++; }
        if (hour >= 24) { hour = 0; day++; }
        if (day > 30) { day = 1; month++; }
        if (month > 12) { month = 1; year++; }
        for (int i = 0; i < 1000000; i++);
        if (read_keyboard() != 0) break;
    }
    print("\n", color);
}

void cmd_reb(char *args) { print("Rebooting...\n", bg_color | 0x04); outb(0x64, 0xFE); }
void cmd_sh(char *args) { print("Shutting down...\n", bg_color | 0x0F); outw(0x604, 0x2000); while (1); }

void cmd_h(char *args) {
    print("=== Table commands ===\n", 0x0E);
    for (int i = 0; commands[i].name != NULL; i++) {
        print("  ", WHITE_ON_BLACK);
        print(commands[i].name, WHITE_ON_BLACK);
        print(" - ", WHITE_ON_BLACK);
        print(commands[i].desc, WHITE_ON_BLACK);
        print("\n", WHITE_ON_BLACK);
    }
}

void cmd_hello(char *args) { print("Hello, user!\n", WHITE_ON_BLACK); }
void cmd_echo(char *args) { if (args) { print(args, WHITE_ON_BLACK); print("\n", WHITE_ON_BLACK); } }

void cmd_calc(char *args) {
    if (!args) { print("Usage: calc 2+3\n", 0x0F); return; }
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
        default: print("Unknown operator\n", 0x0C); return;
    }
    print_int(a, WHITE_ON_BLACK); print(" ", WHITE_ON_BLACK);
    putchar(op, WHITE_ON_BLACK); print(" ", WHITE_ON_BLACK);
    print_int(b, WHITE_ON_BLACK); print(" = ", WHITE_ON_BLACK);
    print_int(result, GREEN_ON_BLACK); print("\n", GREEN_ON_BLACK);
}

void cmd_sleep(char *args) {
    if (!args) { print("Usage: sleep <seconds>\n", 0x0F); return; }
    int sec = 0;
    while (*args >= '0' && *args <= '9') { sec = sec * 10 + (*args - '0'); args++; }
    if (sec == 0) sec = 1;
    for (int i = 0; i < sec * 500000; i++);
    print("Done\n", 0x0A);
}

void cmd_type(char *args) {
    if (!args) { print("Usage: type <command>\n", 0x0F); return; }
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(commands[i].name, args) == 0) {
            print(args, 0x0A); print(" is a table command\n", 0x0A); return;
        }
    }
    print(args, 0x0C); print(": command not found\n", 0x0C);
}

void cmd_bgcolor(char *args) {
    if (!args) { print("Usage: bgcolor <0-F>\n", 0x0F); return; }
    int color = 0;
    if (args[0] >= '0' && args[0] <= '9') color = args[0] - '0';
    else if (args[0] >= 'A' && args[0] <= 'F') color = args[0] - 'A' + 10;
    else if (args[0] >= 'a' && args[0] <= 'f') color = args[0] - 'a' + 10;
    else { print("Invalid color\n", 0x0C); return; }
    bg_color = color << 4;
    clear_screen();
    print("Background color changed\n", bg_color | 0x0A);
    update_prompt();
}

void cmd_edit_par(char *args) { cmd_edit(); }
void clear_cmd(char *args) {
    char* video = (char*) 0xB8000;
    for (int i = 0; i < 80 * 25; i++) video[i * 2] = ' ', video[i * 2 + 1] = 0x00;
}

void cmd_switch() {
    char* video = (char*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) video[i * 2] = ' ', video[i * 2 + 1] = 0x00;
    for (int i = 0; i < 25; i++) print("\n", 0x00);
    print("\r", 0x00);
    print("+======================================================+\n", 0x72);
    print("|                   BITOS DESKTOP                      |\n", 0x71);
    print("+------------------------------------------------------+\n", 0x72);
    print("|     [1] Exit  [2] Reboot  [3] Notes                  |\n", 0x71);
    print("|     [4] Edit  [5] Save to disk [6] load from disk    |\n", 0x71);
    print("+======================================================+\n", 0x72);
    while (1) {
        char ch = read_keyboard();
        if (ch == '1') break;
        if (ch == '2') outb(0x64, 0xFE);
        if (ch == '5') save_note_to_disk();
        if (ch == '6') load_note_from_disk();
    }
    for (int i = 0; i < 80 * 25; i++) video[i * 2] = ' ', video[i * 2 + 1] = 0x00;
}
