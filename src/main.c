#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#define MY_SENSOR_NODE DT_ALIAS(my_sensor)

#define MY_THREAD_STACK_SIZE 1024
#define MY_THREAD_PRIORITY 5

LOG_MODULE_REGISTER(main);
K_THREAD_STACK_DEFINE(my_sensor_thread, MY_THREAD_STACK_SIZE);
// K_THREAD_STACK_DEFINE(my_uart_thread, MY_THREAD_STACK_SIZE);
static struct k_thread my_sensor_thread_data;
// static struct k_thread my_uart_thread_data;

void my_sensor_thread_function(void *device_ptr, void *p2, void *p3) {
        // Cast device's pointer back to the correct type
        const struct device *sensor_device = (const struct device *)device_ptr;
        if (!device_is_ready(sensor_device)){
                // handle error
                LOG_ERR("Sensor is not ready");
                return;
        }
        LOG_INF("Sensor is ready.");

        while (1) {
                // struct sensor_value temp, humidity;
                int ret = sensor_sample_fetch(sensor_device);
                // int ret_humidity = sensor_sample_fetch(sensor_device);

                // if (ret_humidity < 0) {
                //         LOG_ERR("Failed to fetch humidity.");
                // } else {
                //         LOG_INF("Humidty Fetched.");
                // }
                if (ret < 0) {
                        LOG_ERR("Failed to fetch temperature and humidity.");
                } else {
                        LOG_INF("Temperature and humidity Fetched.");
                }



                k_sleep(K_MSEC(2000)); // sleep for 2 seconds
        }
}

// void my_uart_thread(void *p1, void *p2, void *p3) {
//         while (1) {

//         }
// }
int main(void)
{
        LOG_INF("Starting up!");
        // struct sensor_value temp, humidity;

        // Get device
        const struct device *sensor_device = DEVICE_DT_GET(MY_SENSOR_NODE);


        k_thread_create(&my_sensor_thread_data, my_sensor_thread, MY_THREAD_STACK_SIZE, 
                                my_sensor_thread_function,(void *)sensor_device, NULL, 
                                NULL,MY_THREAD_PRIORITY, 0, K_NO_WAIT);

        // Read humidity and temperature
        // int ret_humidity = sensor_channel_get(sensor_device, SENSOR_CHAN_HUMIDITY, &humidity);
        // if (ret_humidity < 0) {
        //         LOG_ERR("Failed to get humnidity. Error: %d", ret_humidity);
        // } else {
        //         LOG_INF("Humdity is %d.%06d%%", humidity.val1, humidity.val2);
        // }

        // int ret_temp= sensor_channel_get(sensor_device, SENSOR_CHAN_AMBIENT_TEMP, &temp);
        // if (ret_temp < 0) {
        //         LOG_ERR("Failed to get temperature. Error: %d", ret_temp);
        // } else {
        //         LOG_INF("Temperature is %d.%06d°C", temp.val1, temp.val2);
        // }

        while (1) 
        {
                k_sleep(K_MSEC(5000));
                LOG_INF("MAIN THREAD STILL RUNNING!\n");
        }
        

        return 0;
}
