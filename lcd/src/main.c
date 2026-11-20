#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "lcd.h"
#include <stdio.h>
#include <string.h>
#include <math.h>  
#include "audio.h" 

/****************************************** */
#define PIN_SDI    11
#define PIN_CS     9
#define PIN_SCK    10
#define PIN_DC     7
#define PIN_nRESET 8

typedef struct {
    int lane;
    int y;
    uint16_t color;
} Note;

void init_spi_lcd() {
    gpio_set_function(PIN_CS, GPIO_FUNC_SIO);
    gpio_set_function(PIN_DC, GPIO_FUNC_SIO);
    gpio_set_function(PIN_nRESET, GPIO_FUNC_SIO);

    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_nRESET, GPIO_OUT);

    gpio_put(PIN_CS, 1); // CS high
    gpio_put(PIN_DC, 0); // DC low
    gpio_put(PIN_nRESET, 1); // nRESET high

    // initialize SPI1 with 48 MHz clock
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SDI, GPIO_FUNC_SPI);
    spi_init(spi1, 100 * 1000 * 1000);
    spi_set_format(spi1, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
}

Picture* load_image(const char* image_data);
void free_image(Picture* pic);

int main() {
    stdio_init_all();

    init_spi_lcd();

    LCD_Setup();
    LCD_Clear(0x0000); // Clear the screen to black

    LCD_DrawString(10, 10, WHITE, BLACK, "SCORE: 0", 16, 0);

    for (int x = 60; x <= 180; x += 60) 
    {
        LCD_DrawLine(x, 50, x, 290, GRAY);
    }

    LCD_DrawLine(0, 270, 239, 270, WHITE);

    Note notes[4] = {
        {0, 80, RED},
        {1, 130, GREEN},
        {2, 180, YELLOW},
        {3, 230, BLUE}
    };

    init_pwm_song();

    while (1) 
    {
        // Step 1: Erase each note at its old position.
        for (int i = 0; i < 4; i++) 
        {
            int x = notes[i].lane * 60 + 10;

            LCD_DrawFillRectangle(x, notes[i].y, x + 40, notes[i].y + 18, BLACK);
        }

        // Step 2: Move each note downward.
        for (int i = 0; i < 4; i++) 
        {
            notes[i].y += 3;

            // Restart the note near the top.
            if (notes[i].y > 290) 
            {
                notes[i].y = 80;
            }
        }

        // Step 3: Draw each note at its new position.
        for (int i = 0; i < 4; i++) 
        {
            int x = notes[i].lane * 60 + 10;

            LCD_DrawFillRectangle(x, notes[i].y, x + 40, notes[i].y + 18, notes[i].color);
        }

        // Step 4: Restore the hit line.
        LCD_DrawLine(0, 270, 239, 270, WHITE);

        sleep_ms(30);
    }

    for(;;);
}