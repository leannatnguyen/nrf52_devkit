#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include "sensor_task.h"
#include "uart_task.h"
#include "button_handler.h"

// Thread defines
#define MY_THREAD_STACK_SIZE 	1024
#define MY_THREAD_PRIORITY 		5

LOG_MODULE_REGISTER(main);

// Thread stack defines
K_THREAD_STACK_DEFINE(my_sensor_thread, MY_THREAD_STACK_SIZE);
K_THREAD_STACK_DEFINE(my_uart_thread, MY_THREAD_STACK_SIZE);
static struct k_thread my_sensor_thread_data, my_uart_thread_data;

// Mutex + Message queue definitions
bool logging_enabled = true;
K_MUTEX_DEFINE(logging_mutex);
K_MSGQ_DEFINE(sensor_queue, sizeof(struct sensor_message), 5, 4);

int main(void)
{
	LOG_INF("Environmental logger starting up!");
	
	// Init mutex
	k_mutex_init(&logging_mutex);

	// Init sensor and button
    init_sensor();
	init_button();

	// Create threads for sensor + uart
	k_thread_create(&my_sensor_thread_data, my_sensor_thread, MY_THREAD_STACK_SIZE, 
						my_sensor_thread_function, NULL, NULL, 
						NULL,MY_THREAD_PRIORITY, 0, K_NO_WAIT);

	k_thread_create(&my_uart_thread_data, my_uart_thread, MY_THREAD_STACK_SIZE, 
                        my_uart_thread_function, NULL, NULL, 
                        NULL, MY_THREAD_PRIORITY, 0, K_NO_WAIT);


	return 0;
}


