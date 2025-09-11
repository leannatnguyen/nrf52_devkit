#ifndef SENSOR_TASK_H
#define SENSOR_TASK_H

#include <zephyr/kernel.h>

#define SW0_NODE				DT_ALIAS(sw0)

void init_sensor(void);
void my_sensor_thread_function(void *p1, void *p2, void *p3);

#endif

