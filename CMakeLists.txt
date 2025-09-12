cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(environmental_logger)

target_include_directories(app PRIVATE include)

target_sources(app PRIVATE 
                src/main.c
                src/sensor_task.c
                src/uart_task.c
                src/button_handler.c
                src/led_handler.c)
