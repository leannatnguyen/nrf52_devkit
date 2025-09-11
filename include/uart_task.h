#ifndef UART_TASK_H
#define UART_TASK_H

#include <zephyr/kernel.h>

struct sensor_message {
    double temp;
    double humidity;
};

extern struct k_msgq sensor_queue;
extern struct k_mutex logging_mutex;
extern bool logging_enabled;


void my_uart_thread_function(void *p1, void *p2, void *p3);

#endif

