#include "led_handler.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(led_handler, LOG_LEVEL_INF);

static const struct gpio_dt_spec my_led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void init_led_handler(void) {
    // Check if LED is ready
	if (!gpio_is_ready_dt(&my_led0)){
		LOG_ERR("LED is not ready.");
		return;
	}

    // Configure button as output
	int ret_led = gpio_pin_configure_dt(&my_led0, GPIO_OUTPUT_ACTIVE);
	if (ret_led != 0) {
		LOG_ERR("Failed to configure LED1");
		return;
	}
}

void led_on(void) {
    gpio_pin_set_dt(&my_led0, 1);
}

void led_off(void) {
    gpio_pin_set_dt(&my_led0, 0);
}

void led_toggle(void) {
    gpio_pin_toggle_dt(&my_led0);
}