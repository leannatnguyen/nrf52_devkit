#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/sys/util.h>

#define MY_SENSOR_NODE DT_ALIAS(my_sensor)

#define MY_THREAD_STACK_SIZE 1024
#define MY_THREAD_PRIORITY 5

#define SW0_NODE	DT_ALIAS(sw0)
#define LED0_NODE	DT_ALIAS(led0)
struct sensor_message {
        float temp;
        float humidity;
};

LOG_MODULE_REGISTER(main);

K_THREAD_STACK_DEFINE(my_sensor_thread, MY_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(my_uart_thread, MY_THREAD_STACK_SIZE);
static struct k_thread my_sensor_thread_data;
static struct k_thread my_uart_thread_data;

static bool logging_enabled = true;
static struct k_mutex logging_mutex;

K_MSGQ_DEFINE(sensor_queue, sizeof(struct sensor_message), 5, 4);

static const struct gpio_dt_spec button = GPIO_DT_SPEC_GET(SW0_NODE, gpios);
static struct gpio_callback button_cb_data;

static const struct gpio_dt_spec my_led0 = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

void my_sensor_thread_function(void *device_ptr, void *led_ptr, void *p3) {
	// Cast device's pointer back to the correct type
	const struct device *sensor_device = (const struct device *)device_ptr;
	const struct gpio_dt_spec *led0 = (const struct gpio_dt_spec *)led_ptr;
	
	if (!device_is_ready(sensor_device)){
		// handle error
		LOG_ERR("Sensor is not ready");
		return;
	}
	LOG_INF("Sensor is ready.");

	struct sensor_message msg;
	bool is_logging;
	// bool led_state;

	while (1) {
		k_mutex_lock(&logging_mutex, K_FOREVER);
		is_logging = logging_enabled;
		k_mutex_unlock(&logging_mutex);
		
		if (is_logging) {
			gpio_pin_toggle_dt(led0);
			struct sensor_value temp, humidity;
			int ret = sensor_sample_fetch(sensor_device);

			if (ret < 0) {
				LOG_ERR("Failed to fetch temperature and humidity.");
			} else {
				LOG_INF("Temperature and humidity Fetched.");

			}

			// Read humidity and temperature + add to message queue
			int ret_humidity = sensor_channel_get(sensor_device, SENSOR_CHAN_HUMIDITY, &humidity);
			int ret_temp= sensor_channel_get(sensor_device, SENSOR_CHAN_AMBIENT_TEMP, &temp);
			if (ret_humidity < 0) {
				LOG_ERR("Failed to get humidity. Error: %d", ret_humidity);
				break;
			} else if(ret_temp < 0) {
				LOG_ERR("Failed to get temperature. Error: %d", ret_temp);
				break;
			}
			else {
				msg.temp = sensor_value_to_double(&temp);
				msg.humidity = sensor_value_to_double(&humidity);
				// LOG_INF(......);
				k_msgq_put(&sensor_queue, &msg, K_NO_WAIT);
			}
		} else {
            // When logging is disabled, turn LED off
            gpio_pin_set_dt(led0, 0);
        }

		k_sleep(K_MSEC(2000)); // sleep for 2 seconds
	}
}

void my_uart_thread_function(void *led_ptr, void *p2, void *p3) {
	const struct gpio_dt_spec *led0 = (const struct gpio_dt_spec *)led_ptr;
	struct sensor_message msg;

	while (1) {
		int ret = k_msgq_get(&sensor_queue, &msg, K_FOREVER);
		if (ret == 0) {
			LOG_INF("Temperature = %.2f C, Humidity = %.2f%%", (double)msg.temp, (double)msg.humidity);
		}
	}
}

void button_pressed(const struct device *dev, struct gpio_callback *cb, gpio_port_pins_t pins)
{
	k_mutex_lock(&logging_mutex, K_FOREVER);

	// toggle logging enabled since button was pressed
	logging_enabled = !logging_enabled;
	LOG_INF("Button pressed!");
	if (logging_enabled) {
		LOG_INF("Sensor logging enabled!");
		gpio_pin_set_dt(&my_led0, 1);
	} else {
		LOG_INF("Sensor logging disabled!");
		gpio_pin_set_dt(&my_led0, 0);
	}
	
	k_mutex_unlock(&logging_mutex);
}

int main(void)
{
	LOG_INF("Starting up!");
	
	// Init mutex
	k_mutex_init(&logging_mutex);
	LOG_INF("Logging mutex initialized");

	// Get device
	const struct device *sensor_device = DEVICE_DT_GET(MY_SENSOR_NODE);

	// Check if button is ready
	if (!gpio_is_ready_dt(&button)) {
		LOG_ERR("Button is not ready.");
		return 0;
	}
	LOG_INF("Button is ready.");
	// configure the button as an input
	int ret_button = gpio_pin_configure_dt(&button, GPIO_INPUT);
	ret_button = gpio_pin_interrupt_configure_dt(&button, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret_button != 0) {
		LOG_ERR("Failed to configure button.");
		return 0;
	}
	LOG_INF("Button configured as input.");

	// Check if LED is ready
	if (!gpio_is_ready_dt(&my_led0)){
		LOG_ERR("LED is not ready.");
		return 0;
	}
	LOG_INF("LED is ready");	
	// Configure button as output
	int ret_led = gpio_pin_configure_dt(&my_led0, GPIO_OUTPUT_ACTIVE);
	if (ret_led != 0) {
		LOG_ERR("Failed to configure LED1");
		return 0;
	}
	LOG_INF("LED configured as output.");


	// init callback
	gpio_init_callback(&button_cb_data, button_pressed, BIT(button.pin));
	gpio_add_callback(button.port, &button_cb_data);

	k_thread_create(&my_sensor_thread_data, my_sensor_thread, MY_THREAD_STACK_SIZE, 
						my_sensor_thread_function,(void *)sensor_device,(void *)&my_led0, 
						NULL,MY_THREAD_PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&my_uart_thread_data, my_uart_thread, MY_THREAD_STACK_SIZE, 
                        my_uart_thread_function, (void *)&my_led0, NULL, 
                        NULL, MY_THREAD_PRIORITY, 0, K_NO_WAIT);

	while (1) 
	{
		k_sleep(K_MSEC(5000));
	}
	

	return 0;
}

