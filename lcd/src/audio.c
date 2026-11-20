#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

#include "audio.h"
#include "audio_data.h"

#define AUDIO_PIN 36
#define AUDIO_SLICE 10
#define AUDIO_WRAP 3749

static volatile uint32_t song_pos = 0;
static uint8_t song_phase = 0;

static void pwm_song_handler(void) {
    pwm_clear_irq(AUDIO_SLICE);

    song_phase++;

    if (song_phase == 2) {
        song_phase = 0;

        song_pos++;

        if (song_pos >= AUDIO_LEN) {
            song_pos = 0;
        }

        pwm_set_chan_level(
            AUDIO_SLICE,
            PWM_CHAN_A,
            ((uint32_t)audio[song_pos] * AUDIO_WRAP) / 1024
        );
    }
}

void init_pwm_song(void) {
    gpio_set_function(AUDIO_PIN, GPIO_FUNC_PWM);

    pwm_set_clkdiv(AUDIO_SLICE, 1.0f);
    pwm_set_wrap(AUDIO_SLICE, AUDIO_WRAP);
    pwm_set_chan_level(AUDIO_SLICE, PWM_CHAN_A, 0);

    pwm_clear_irq(AUDIO_SLICE);

    irq_set_exclusive_handler(
        PWM_IRQ_WRAP_0,
        pwm_song_handler
    );

    pwm_set_irq_enabled(AUDIO_SLICE, true);
    irq_set_enabled(PWM_IRQ_WRAP_0, true);

    pwm_set_enabled(AUDIO_SLICE, true);
}