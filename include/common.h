#ifndef COMMON_H
#define COMMON_H

extern struct k_msgq sensor_queue;
extern struct k_mutex logging_mutex;
extern bool logging_enabled;

struct sensor_message {
    double temp;
    double humidity;
};

#endif