#include "uart_task.h"
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(uart_task);

void my_uart_thread_function(void *p1, void *p2, void *p3) {
	struct sensor_message msg;

	while (1) {
		int ret = k_msgq_get(&sensor_queue, &msg, K_FOREVER);
		if (ret == 0) {
			LOG_INF("Temperature = %.2f C, Humidity = %.2f%%", (double)msg.temp, (double)msg.humidity);
		}
	}
}