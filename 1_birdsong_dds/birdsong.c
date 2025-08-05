#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/spi.h"
#include "hardware/timer.h"

// dds
#define sine_table_size 256
volatile int sine_table[sine_table_size];
#define DELAY 20           // 1/Fs (in microseconds)
#define two32 4294967296.0 // 2^32
#define Fs 50000           // 50 KHz
volatile unsigned int phase_accum_main;
volatile unsigned int phase_incr_main;
volatile unsigned int current_freq;
#define m 0.000523 // pi/6000

// amplitude modulation parameters
float max_amplitude = 1;     // maximum amplitude
float attack_inc;            // rate at which sound ramps up
float decay_inc;             // rate at which sound ramps down
float current_amplitude = 0; // current amplitude

// timing parameters for beeps (units of interrupts)
#define ATTACK_TIME 1000
#define DECAY_TIME 1000
#define SWOOP_DURATION 6500
#define REPEAT_INTERVAL 10000

// State machine variables
volatile unsigned int STATE = 0;
volatile unsigned int count = 0;

// spi defs
#define SPI_PORT spi0
#define SPI_BAUDRATE 20 * 1000 * 1000 // 20MHz
#define PIN_MISO 4
#define PIN_CS 5
#define PIN_SCK 6
#define PIN_MOSI 7

// isr defs
#define ISR_GPIO 25
#define ALARM_NUM 0
#define ALARM_IRQ TIMER_IRQ_0

// dac parameters
int DAC_output;
uint16_t DAC_data;
#define DAC_config_chan_A 0b0011000000000000

static void alarm_irq(void)
{

    // clear interrupt
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);

    // re-enable alarm
    timer_hw->alarm[ALARM_NUM] = timer_hw->timerawl + DELAY;

    if (STATE == 0)
    {
        // dds
        current_freq = -260 * sin(-m * count) + 1740;
        phase_incr_main = (current_freq * two32) / Fs;
        phase_accum_main += phase_incr_main;
        DAC_output = current_amplitude * sine_table[phase_accum_main >> 24] + 2048;

        // increase amplitude
        if (count < ATTACK_TIME)
        {
            current_amplitude += attack_inc;
        }
        // decrease amplitude
        else if (count > SWOOP_DURATION - DECAY_TIME)
        {
            current_amplitude -= decay_inc;
        }

        // mask with dac control bits
        DAC_data = (DAC_config_chan_A | (DAC_output & 0xffff));

        // write dac data to spi
        spi_write16_blocking(SPI_PORT, &DAC_data, 1);

        // increment count
        count += 1;

        // check for state transition
        if (count == SWOOP_DURATION)
        {
            current_amplitude = 0;
            STATE = 1;
            count = 0;
        }
    }
    else
    {
        if (STATE == 1)
        {
            // dds
            current_freq = 0.0001183 * count * count + 1740;
            phase_incr_main = (current_freq * two32) / Fs;
            phase_accum_main += phase_incr_main;
            DAC_output = current_amplitude * sine_table[phase_accum_main >> 24] + 2048;

            // increase amplitude
            if (count < ATTACK_TIME)
            {
                current_amplitude += attack_inc;
            }
            // decrease amplitude
            else if (count > SWOOP_DURATION - DECAY_TIME)
            {
                current_amplitude -= decay_inc;
            }

            // mask with dac control bits
            DAC_data = (DAC_config_chan_A | (DAC_output & 0xffff));

            // write dac data to spi
            spi_write16_blocking(SPI_PORT, &DAC_data, 1);

            // increment count
            count += 1;

            // check for state transition
            if (count == SWOOP_DURATION)
            {
                current_amplitude = 0;
                STATE = 2;
                count = 0;
            }
        }
        else
        {
            count += 1;
            if (count == REPEAT_INTERVAL)
            {
                current_amplitude = 0;
                STATE = 0;
                count = 0;
            }
        }
    }
}

int main()
{

    stdio_init_all();
    printf("DAC Test Program.");

    // build sine lookup table
    int i;
    for (i = 0; i < sine_table_size; i++)
    {
        sine_table[i] = (int)(2047 * sin((float)i * 6.283 / (float)sine_table_size));
    }

    // increments for calculating bow envelope
    attack_inc = max_amplitude / ATTACK_TIME;
    decay_inc = max_amplitude / DECAY_TIME;

    // spi init - spi port 0; 20MHz (dac datasheet)
    spi_init(SPI_PORT, SPI_BAUDRATE);

    // 16 databits per transfer; 0-0 mode (dac datasheet)
    spi_set_format(SPI_PORT, 16, 0, 0, 0);

    // mapping gpio-spi
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS, GPIO_FUNC_SPI);

    // isr timing gpio setup
    gpio_init(ISR_GPIO);
    gpio_set_dir(ISR_GPIO, GPIO_OUT);
    gpio_put(ISR_GPIO, 0);

    // enable timer alarm 0
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);
    irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);
    irq_set_enabled(ALARM_IRQ, true);

    timer_hw->alarm[ALARM_NUM] = timer_hw->timerawl + DELAY;

    while (1)
    {
    }
    return 0;
}
