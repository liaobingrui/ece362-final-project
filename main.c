#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "queue.h"
#include "support.h"
#include "audio_data.h"

//////////////////////////////////////////////////////////////////////////////

const char* username = "lbatlank";

//////////////////////////////////////////////////////////////////////////////
#define SONG
static uint32_t song_pos = 0;
static uint8_t  song_phase = 0;

void pwm_song_handler() {
    pwm_clear_irq(10);

    song_phase = song_phase + 1;
    if (song_phase == 2) {
        song_phase = 0;
        song_pos = song_pos + 1;
        if (song_pos >= AUDIO_LEN) song_pos = 0;
        
        pwm_set_chan_level(10, 0, ((uint32_t)audio[song_pos] * 3749) / 1024);
    }
}
void init_pwm_song() {
    gpio_set_function(36, GPIO_FUNC_PWM);
    pwm_set_clkdiv(10, 1.0f);
    pwm_set_wrap(10, 3749);
    pwm_set_chan_level(10, 0, 0);

    pwm_set_irq_enabled(10, true);
    irq_set_exclusive_handler(PWM_IRQ_WRAP_0, pwm_song_handler);
    irq_set_enabled(PWM_DEFAULT_IRQ_NUM(), true);

    pwm_set_enabled(10, true);
}
//////////////////////////////////////////////////////////////////////////////

int main()
{
    stdio_init_all();
    #ifdef SONG
    init_pwm_song();
    for (;;) tight_loop_contents();
    #endif
}
