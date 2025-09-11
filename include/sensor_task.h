#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include <zephyr/kernel.h>

#define MY_SENSOR_NODE DT_ALIAS(my_sensor)

void init_sensor(void);
void my_sensor_thread_function(void *p1, void *p2, void *p3);

#endif

