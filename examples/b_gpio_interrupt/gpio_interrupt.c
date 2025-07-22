#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

void gpio_callback()
{
    // toggle GPIO pin 25
    gpio_put(25, !gpio_get(25));
}

int main()
{
    stdio_init_all();
    printf("GPIO Interrupt Demo.");

    // configure GPIO interrupt
    gpio_set_irq_enabled_with_callback(6, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    // configure GPIO pins
    gpio_init(3);
    gpio_init(25);
    gpio_set_dir(3, GPIO_OUT);
    gpio_set_dir(25, GPIO_OUT);

    gpio_put(3, 0);

    while (1)
    {
        gpio_put(3, 1);
        sleep_ms(1000);
        gpio_put(3, 0);
        sleep_ms(1000);
    }
}
