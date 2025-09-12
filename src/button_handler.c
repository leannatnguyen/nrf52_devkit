#include "button_handler.h"
#include "common.h"
#include <zephyr/drivers/gpio.h>
#include <zephyr/logging/log.h>
#include "led_handler.h"

LOG_MODULE_REGISTER(button_handler);

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static struct gpio_callback button_cb_data;

void button_press_handler(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins)
{
	k_mutex_lock(&logging_mutex, K_FOREVER);

	// toggle logging enabled since button was pressed
	logging_enabled = !logging_enabled;

	if (logging_enabled) {
		LOG_INF("Sensor logging enabled!");
		led_on();
	} else {
		LOG_INF("Sensor logging disabled!");
		led_off();
	}
	
	k_mutex_unlock(&logging_mutex);
}

void init_button(void) {
    // Check if button is ready
	if (!gpio_is_ready_dt(&button)) {
		LOG_ERR("Button is not ready.");
		return;
	}

	// configure the button as an input
	int ret_button = gpio_pin_configure_dt(&button, GPIO_INPUT);
	ret_button = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret_button != 0) {
		LOG_ERR("Failed to configure button.");
		return;
	}

    // init callback
	gpio_init_callback(&button_cb_data, button_press_handler, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);
}

