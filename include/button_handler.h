#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>

#define SW0_NODE				DT_ALIAS(sw0)


void button_press_handler(const struct device *dev, 
    struct gpio_callback *cb, gpio_port_pins_t pins);
void init_button(void);


#endif