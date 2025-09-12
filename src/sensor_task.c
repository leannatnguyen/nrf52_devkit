#include "sensor_task.h"
#include "common.h"
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/gpio.h>
#include "led_handler.h"

LOG_MODULE_REGISTER(sensor_task);
static const struct device *sensor_device;

void init_sensor(void) {
    sensor_device = DEVICE_DT_GET(MY_SENSOR_NODE);

    if (!device_is_ready(sensor_device)){
		// handle error
		LOG_ERR("Sensor is not ready");
		return;
	}
	LOG_INF("Sensor is ready.");
}
void my_sensor_thread_function(void *p1, void *p2, void *p3) {
	struct sensor_message msg;
	bool is_logging;

	while (1) {
		k_mutex_lock(&logging_mutex, K_FOREVER);
		is_logging = logging_enabled;
		k_mutex_unlock(&logging_mutex);


		
		if (is_logging) {
            
			struct sensor_value temp, humidity;
			int ret = sensor_sample_fetch(sensor_device);

			if (ret < 0) {
				LOG_ERR("Failed to fetch temperature and humidity.");
                led_off();
                k_sleep(K_MSEC(2000));
                continue;
			} else {
				LOG_INF("Temperature and humidity Fetched.");

			}


			// Read humidity and temperature + add to message queue
			int ret_humidity = sensor_channel_get(sensor_device, SENSOR_CHAN_HUMIDITY, &humidity);
			int ret_temp= sensor_channel_get(sensor_device, SENSOR_CHAN_AMBIENT_TEMP, &temp);
			if (ret_humidity < 0) {
				LOG_ERR("Failed to get humidity. Error: %d", ret_humidity);
                led_off();
				break;
			} else if(ret_temp < 0) {
				LOG_ERR("Failed to get temperature. Error: %d", ret_temp);
                led_off();
				break;
			}
			else {
				msg.temp = sensor_value_to_double(&temp);
				msg.humidity = sensor_value_to_double(&humidity);
				k_msgq_put(&sensor_queue, &msg, K_NO_WAIT);
                led_toggle();
			}
		} else {
            // When logging is disabled, turn LED off
            led_off();
        }
        
		k_sleep(K_MSEC(2000)); // sleep for 2 seconds
	}
}