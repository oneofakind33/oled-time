#include <stdio.h>              // Standard I/O
#include <stdlib.h>             // Standard library functions like exit()
#include <fcntl.h>              // File control (open)
#include <unistd.h>             // Unix system calls like read/write
#include <linux/i2c-dev.h>      // I2C device definitions
#include <sys/ioctl.h>          // ioctl system call
#include <string.h>             // For string operations like strlen()
#include <stdint.h>             // For fixed-size integer types like uint8_t
#include <time.h>               // For time functions
#include "font.c"               // Include font definitions and font logic

#define I2C_BUS "/dev/i2c-3"    // The I2C bus to use (depends on your board)
#define OLED_ADDR 0x3C          // I2C address of SSD1306 OLED display

FontType selected_font = FONT_BOLD;  // Default font style
const uint8_t (*current_font)[10];   // Pointer to the selected font array
int i2c_fd;                          // File descriptor for the I2C device

// Sends a command or data byte to OLED
void oled_write(uint8_t val, int is_data) 
{
    uint8_t buffer[2] = { is_data ? 0x40 : 0x00, val };  // 0x40 = data, 0x00 = command
    write(i2c_fd, buffer, 2);                            // Write 2 bytes to OLED
}

// Sends a command byte
void oled_command(uint8_t cmd)
{
    oled_write(cmd, 0); // 0 means it's a command
}

// Sends a data byte
void oled_data(uint8_t data) 
{
    oled_write(data, 1); // 1 means it's data
}

// Initializes the OLED display with SSD1306 commands
void oled_init() 
{
    oled_command(0xAE);             // Display OFF
    oled_command(0xD5); oled_command(0x80); // Set display clock divide
    oled_command(0xA8); oled_command(0x1F); // Set multiplex ratio (32 lines)
    oled_command(0xD3); oled_command(0x00); // Set display offset
    oled_command(0x40);             // Set start line at 0
    oled_command(0x8D); oled_command(0x14); // Enable charge pump
    oled_command(0x20); oled_command(0x00); // Set horizontal addressing mode
    oled_command(0xA1);             // Set segment remap (mirror horizontally)
    oled_command(0xC8);             // COM output scan direction (mirror vertically)
    oled_command(0xDA); oled_command(0x02); // Set COM pins configuration
    oled_command(0x81); oled_command(0x8F); // Set contrast
    oled_command(0xD9); oled_command(0xF1); // Set pre-charge period
    oled_command(0xDB); oled_command(0x40); // Set Vcomh deselect level
    oled_command(0xA4);             // Resume to RAM content display
    oled_command(0xA6);             // Set normal display (not inverted)
    oled_command(0xAF);             // Display ON
}

// Clears the display by writing 0x00 to all pixels
void oled_clear() 
{
    for (int page = 0; page < 4; page++) 
    {       // 4 pages for 32px height (8px/page)
        oled_command(0xB0 + page);               // Set page address
        oled_command(0x00);                      // Set lower column address
        oled_command(0x10);                      // Set higher column address
        for (int i = 0; i < 128; i++) oled_data(0x00);  // Clear 128 columns
    }
}

// Sets cursor position (x: column, page: row of 8-pixel height)
void oled_set_cursor(int x, int page) 
{
    oled_command(0xB0 + page);                   // Set page address
    oled_command(0x00 + (x & 0x0F));             // Lower nibble of column
    oled_command(0x10 + ((x >> 4) & 0x0F));      // Upper nibble of column
}

// Draws a single big character (from selected font)
void oled_print_big_char(char c) 
{
    const uint8_t* font = current_font[0];       // Default to first character
    if (c >= '0' && c <= '9') font = current_font[c - '0']; // Digits 0–9
    else if (c == ':') font = current_font[10];             // Colon
    else if (c == ' ') { static const uint8_t blank[10] = {0}; font = blank; } // Space

    for (int i = 0; i < 10; i++) oled_data(font[i]); // Print 10-column char
}

// Prints a string of big characters starting at (x, y)
void oled_print_big_string(int x, int y, const char* str) 
{
    oled_set_cursor(x, y);              // Set cursor
    while (*str) oled_print_big_char(*str++); // Print each character
}

// Gets the current time string, optionally hiding the colon for blink
void get_time_string(char* buffer, int show_colon) 
{
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    if (show_colon)
        snprintf(buffer, 9, "%02d:%02d:%02d", t->tm_hour, t->tm_min, t->tm_sec);
    else
        snprintf(buffer, 9, "%02d %02d %02d", t->tm_hour, t->tm_min, t->tm_sec); // Hide colon
}

// Main function
int main() 
{
    // Open I2C device
    if ((i2c_fd = open(I2C_BUS, O_RDWR)) < 0) 
    {
        perror("I2C open failed");
        return 1;
    }

    // Set I2C slave address for OLED
    if (ioctl(i2c_fd, I2C_SLAVE, OLED_ADDR) < 0) 
    {
        perror("I2C ioctl failed");
        return 1;
    }

    oled_init();    // Initialize OLED
    oled_clear();   // Clear screen

    char time_str[9];  // "hh:mm:ss\0"

    // Ask user to select font
    printf("Select font: [0] Bold  [1] Thin  [2] Wide  [3] Digital: ");
    int choice;
    scanf("%d", &choice);
    if (choice < 0 || choice > 3) 
    choice = 0;

    selected_font = (FontType)choice;       // Set font choice
    current_font = get_font(selected_font); // Load selected font pointer

    while (1) {
        time_t now = time(NULL);
        struct tm* t = localtime(&now);
        int blink_colon = (t->tm_sec % 1 == 0);  // Blink colon every second

        get_time_string(time_str, blink_colon);  // Get time

        int str_width = strlen(time_str) * 10;   // 10px width per char
        int x_start = (128 - str_width) / 2;     // Center align horizontally

        oled_clear();                            // Clear display
        oled_print_big_string(x_start, 1, time_str); // Print time

        usleep(500000); // Wait 0.5 sec to update again (smooth blink)
    }

    close(i2c_fd);  // Close I2C device
    return 0;
}

