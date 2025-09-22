#include "uart_task.h"
#include "common.h"
#include <stdio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

#define UART_NODE DT_NODELABEL(uart0)
#define BUFFER_SIZE 50

LOG_MODULE_REGISTER(uart_task);

static const struct device *uart_dev;


void my_uart_thread_function(void *p1, void *p2, void *p3) {
	struct sensor_message msg;
	char buf[BUFFER_SIZE];
	int len;

	uart_dev = DEVICE_DT_GET(UART_NODE);

	if (!device_is_ready(uart_dev)) {
		LOG_ERR("UART device is not ready.");
		return;
	}

	while (1) {
		int ret = k_msgq_get(&sensor_queue, &msg, K_FOREVER);
		if (ret == 0) {
			// LOG_INF("Temperature = %.2f C, Humidity = %.2f%%", (double)msg.temp, (double)msg.humidity);
			len = snprintf(buf, sizeof(buf),
							"Temperature: %.2f C, Humidity %.2f %%\r\n",
						msg.temp, msg.humidity);
			for (int i = 0; i < len; i++) {
				uart_poll_out(uart_dev, buf[i]);
			}
		}
	}
}