#include <stdio.h>
#include <math.h>

#include "pico/stdlib.h"
#include "hardware/irq.h"
#include "hardware/spi.h"
#include "hardware/timer.h"

// dds
#define sine_table_size 256
volatile int sine_table[sine_table_size];
// delay controls the sampling rate of the DDS
#define DELAY 20           // 1/Fs (in microseconds)
#define two32 4294967296.0 // 2^32
#define Fs (50 * 1000)     // 50 KHz
volatile unsigned int phase_accum_main;
// increment amount = (Fout/Fs)*2^32
// Fout - output frequency (frequncy of the sound to be made),
// Fs - sampling frequncy (here 5KHz or 50_000 times a second),
// 2^32 since we are scaling the angles from 0->2Pi to 0->(2^32)-1 units
// and store that rescaled value in a 32-bit variable that we are calling accumulator.
volatile unsigned int phase_incr_main = (800.0 * two32) / Fs;

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
uint16_t DAC_data;
// from dac datasheet
// 0011: 0 - write to DACa, 0 - don't care, 1 - 1x output gain, 1 - activate DAC
// 000000000000: DAC input data
#define DAC_config_chan_A 0b0011000000000000

static void alarm_irq(void)
{   
    // toggle onboard LED; program heartbeat
    gpio_put(ISR_GPIO, !gpio_get(ISR_GPIO));

    // Once the alarm has fired, the ARMED bit will be set to 0.
    // To clear the latched interrupt, write a 1 to the appropriate bit in INTR
    hw_clear_bits(&timer_hw->intr, 1u << ALARM_NUM);

    // re-enable alarm
    timer_hw->alarm[ALARM_NUM] = timer_hw->timerawl + DELAY;

    // dds
    phase_accum_main += phase_incr_main;
    // get the phasor value, remove 24 least significant bit
    // as the sine table's can only hold 256 values (32-24 = 8 bits, 2^8 = 255).
    // look up that value in sine table and add 2048 to it
    // as to convert the resulting value to be between 0 and 4095 ((2^12)-1)
    // since the dac expects 4 config bits + 12 data bits.
    // convert the resulting number to 16 digit binary using AND op with ffff
    // which is hexadecimal for (2^16)-1
    // merge the DAC config to the data by OR op with DAC_config_chan_A
    DAC_data = (DAC_config_chan_A | ((sine_table[phase_accum_main >> 24] + 2048) & 0xffff));

    // write dac data to spi
    spi_write16_blocking(SPI_PORT, &DAC_data, 1);
}

int main()
{

    stdio_init_all();
    printf("DAC Test Program.");

    // build sine lookup table
    int i;
    for (i = 0; i < sine_table_size; i++)
    {
        // given i, find the repective value in radians scaled to sine_table_size
        // 6.283 = 2pi, 2pi is a full circle in radians
        // because/since sin() takes radians as input and return value between -1 and 1
        // scale the result to -2047 to 2047 range to maintain a 12 bit standard int;
        // 2047 = 2^11 - 1;
        // which will be rescaled to 0-4096 (12 bits) while defining DAC_data
        sine_table[i] = (int)(2047 * sin((float)i * 6.283 / (float)sine_table_size));
    }

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

    // enable the interrupt at the timer with a write
    // to the appropriate alarm bit in INTE:
    // i.e. (1 << 0) for ALARM0
    hw_set_bits(&timer_hw->inte, 1u << ALARM_NUM);

    // enable the appropriate timer interrupt at the processor
    irq_set_exclusive_handler(ALARM_IRQ, alarm_irq);
    irq_set_enabled(ALARM_IRQ, true);

    // write the time you would like the interrupt to fire to ALARM0
    // (i.e. the current value in TIMERAWL plus your desired alarm time in microseconds).
    // Writing the time to the ALARM register sets the ARMED bit as a side effect.
    // Once the alarm has fired, the ARMED bit will be set to 0.
    // To clear the latched interrupt, write a 1 to the appropriate bit in INTR
    timer_hw->alarm[ALARM_NUM] = timer_hw->timerawl + DELAY;

    while (1)
    {
    }
    return 0;
}
