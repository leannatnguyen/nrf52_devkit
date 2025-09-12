#ifndef LED_HANDLER_H
#define LED_HANDLER_H

#include <zephyr/kernel.h>

#define LED0_NODE	DT_ALIAS(led0)

void init_led_handler(void);
void led_on(void);
void led_off(void);
void led_toggle(void);

#endif