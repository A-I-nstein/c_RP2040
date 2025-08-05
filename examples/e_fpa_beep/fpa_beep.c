#include <math.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// spi
#define SPI_PORT spi0
#define SPI_FREQ 20 * 1000 * 1000 // 20MHz
#define PIN_MISO 4
#define PIN_CS 5
#define PIN_SCK 6
#define PIN_MOSI 7

// dac
#define LDAC 8
int DAC_output;
uint16_t DAC_data;
#define DAC_config_chan_B 0b1011000000000000

// fixed-point arithmetic
typedef signed int fix15;
#define fix2int15(a) ((int)(a >> 15))
#define int2fix15(a) ((fix15)(a << 15))
#define float2fix15(a) ((fix15)((a) * 32768.0))
#define fpa_div(a, b) (fix15)(((signed long long)(a) << 15) / (b))
#define fpa_mul(a, b) ((fix15)((((signed long long)(a)) * ((signed long long)(b))) >> 15))

// amplitude modulation
fix15 attack_inc;
fix15 decay_inc;
fix15 current_amplitude = 0;
fix15 max_amplitude = int2fix15(1);

// beep parameters
#define DECAY_TIME 250
#define ATTACK_TIME 250
#define BEEP_DURATION 10500
#define BEEP_REPEAT_INTERVAL 50000

// dds
#define DELAY 20
#define Fs 50000
#define two32 4294967296.0
#define sine_table_size 256
fix15 sin_table[sine_table_size];
volatile unsigned int phase_accum_main;
volatile unsigned int phase_incr_main = (400.0 * two32) / Fs;

// alarm
#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0

// state machine
volatile unsigned int STATE = 0;
volatile unsigned int count = 0;

static void alarm_irq(void)
{

    // clear and reset alarm
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);
    timer_hw->alarm[ALARM_NUM] = timer_hw->timerawl + DELAY;

    if (STATE == 0)
    {
        // dds
        phase_accum_main += phase_incr_main;
        DAC_output = fix2int15(fpa_mul(current_amplitude, sin_table[phase_accum_main >> 24])) + 2048;

        // amplitude modulation
        if (count < ATTACK_TIME)
        {
            current_amplitude = (current_amplitude + attack_inc);
        }
        else if (count > BEEP_DURATION - DECAY_TIME)
        {
            current_amplitude = (current_amplitude - decay_inc);
        }

        // spi write dac
        DAC_data = (DAC_config_chan_B | (DAC_output & 0xffff));
        spi_write16_blocking(SPI_PORT, &DAC_data, 1);

        // state transition
        count += 1;
        if (count == BEEP_DURATION)
        {
            STATE = 1;
            count = 0;
        }
    }

    else
    {
        // state transition
        count += 1;
        if (count == BEEP_REPEAT_INTERVAL)
        {
            current_amplitude = 0;
            STATE = 0;
            count = 0;
        }
    }
}

int main()
{
    stdio_init_all();
    printf("Fixed-Point Arithmetic Beep Demo.\n");

    // spi init and config
    spi_init(SPI_PORT, SPI_FREQ);
    spi_set_format(SPI_PORT, 16, 0, 0, 0);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);

    // dac specific config
    gpio_init(LDAC);
    gpio_set_dir(LDAC, GPIO_OUT);
    gpio_put(LDAC, 0);

    // increments for calculating bow envelope
    attack_inc = fpa_div(max_amplitude, int2fix15(ATTACK_TIME));
    decay_inc = fpa_div(max_amplitude, int2fix15(DECAY_TIME));

    // sine lookup table
    int x;
    for (x = 0; x < sine_table_size; x++)
    {
        sin_table[x] = float2fix15(2047 * sin((float)x * 6.283 / (float)sine_table_size));
    }

    // interrupt config and init
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
    irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);
    irq_set_enabled(ALARM_IRQ, true);
    timer_hw->alarm[ALARM_NUM] = timer_hw->timerawl + DELAY;

    while (1)
    {
    }
}
