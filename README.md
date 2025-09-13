# nrf52 DK Environmental Logger
By: Leanna Nguyen

## Summary
I completed this project to get a better understanding of how to interface a sensor with an nRF52 Devkit using the Zephyr RTOS communicating over the I2C onboard bus. 

## Equipment and Software
1. nRF52 DK
2. Computer
3. I2C SHT3X Sensor
4. Breadboard and Wires
5. nRF Connect (VS Code Extension)

## Setup
### Physical Setup
Note: I'll be referring to the devkit in the orientation with the power switch on the top left and USB connection at the top. 
Connect the I2C SHT3X sensor to your nRF52 DK in the following order:
1. Connect the sensor's VIN pin to the devkit's VDD (left side under the boot button).
    - Note: make sure to **NOT** connect to 5V. This will break the sensor.
    - Picture: red wire
2. Connect the sensor's GND pin to any of the GND pins on the left of the devkit. (I used the GND's a few pins down from the VDD above.)
    - Picture: black wire
3. Conenct the sensor's SCL/T pin to the devkit's `P0.27` SCL pin. 
    - Picture: orange wire
4. Connect the sensor's SDA/RH pin to the devkit's `P0.26` SDA pin.
    - Picture: yellow wire.
5. Connect your devkit to your computer and flip the switch to ON. 

### Software setup 
1. Download nRF Connect VS Code Extension
2. `Manage SDKs` -> `Install SDK` -> `nRF Connect SDK`
    - recommend downloading `v2.7.0` and up
3. After that you can clone this repo
    - `git clone https://github.com/leannatnguyen/nrf52_devkit`
4. Open up the repo in VS Code using the nRF Connect extension -> `Open an existing application`
5. On the nRF Connect `Applications` tab click `+ Add build configuration`
6. Select whichever devkit you are using for the board config. I'm using the `nrf52dk_nrf52832`.
7. Scroll down and click `Generate and build`
8. On the nRF Connect `Actions` tab, click `Build`
9. On the same `Actions` tab, click `Flash`
10. To see the output, click on the `Connected devices` tab and click your device.
11. Select which port you want to view and then hit enter. I used `VCOM0`. It should open up a terminal window under your device's name.

## Software Breakdown
- Files:
    - `main.c`
        - This is where thread and OS related functionality lies. Here is where I create threads, their stacks, a mutex and a message queue. Additionally, the sensor, button, and LED are initialized through init functions here.
    - `button_handler`
        - This is where all button 1 (button0 in the devicetree) functions are defined.
        - `init_button` initalizes the button and gets it ready to use. It configures the button as an interrupt and associates the correct callback function to the the interrupt.
        - `button_press_handler` is the callback function that runs whenever the button interrupt is triggered. It uses a mutex to read the `logging_enabled` variable then toggles the variable to either enable or disable logging. Additionally it turns the LED on or off depending on its previous state.
    - `led_handler`
        - This is where LED1 (led0 in the devicetree) related functions are. They include LED init, turning the LED on or off, and toggling the LED.
        - `init_led_handler` is where the LED is checked for readiness and configured as an output.
        - `led_on` turns the LED on by setting the pin high.
        - `led_off` turns the LED off by setting the pin low.
        - `led_toggle` turns the LED on and off in one sequence.
    - `sensor_task`
        - This is where all sensor (SHT3X) related functions live. That includes initializing the sensor and the sensor thread itself.
        - `init_sensor` gets the sensor device and then checks if it's ready.
        - `my_sensor_thread_function` is the sensor thread function that runs every 2s. It uses the mutex to check if logging is enabled. It it is then the sensor reads the temperature and humidity data and then converts that info to a double then adds it to the message queue for the UART thread to consume.
    - `uart_task`
        - This is where the UART thread definition lives.
        - `my_uart_thread_function` gets data from the message queue anytime there's new data and then outputs to the UART console what the temperature and relative humidity are. 
    - `nrf52dk_nrf52832.overlay`
        - This is where the alias for the sensor is defined and where you add the sensor to the i2c node.

# Example Output(s)
## Working Sensor
```
*** Booting nRF Connect SDK v2.7.0-5cb85570ca43 ***
*** Using Zephyr OS v3.6.99-100befc70c74 ***
[00:00:00.459,594] <inf> main: Environmental logger starting up!
[00:00:00.459,594] <inf> sensor_task: Sensor is ready.
[00:00:00.460,754] <inf> sensor_task: Temperature and humidity Fetched.
[00:00:00.460,815] <inf> uart_task: Temperature = 24.99 C, Humidity = 48.03%
[00:00:02.461,975] <inf> sensor_task: Temperature and humidity Fetched.
[00:00:02.462,066] <inf> uart_task: Temperature = 24.96 C, Humidity = 48.08%
[00:00:04.463,195] <inf> sensor_task: Temperature and humidity Fetched.
[00:00:04.463,287] <inf> uart_task: Temperature = 24.92 C, Humidity = 48.05%
[00:00:06.464,416] <inf> sensor_task: Temperature and humidity Fetched.
[00:00:06.464,508] <inf> uart_task: Temperature = 24.87 C, Humidity = 48.09%
[00:00:08.465,637] <inf> sensor_task: Temperature and humidity Fetched.
[00:00:08.465,728] <inf> uart_task: Temperature = 24.81 C, Humidity = 48.09%
[00:00:10.466,857] <inf> sensor_task: Temperature and humidity Fetched.
[00:00:10.466,949] <inf> uart_task: Temperature = 24.78 C, Humidity = 49.08%
[00:00:12.074,768] <inf> button_handler: Button pressed!
[00:00:12.074,768] <inf> button_handler: Sensor logging disabled!
[00:01:09.309,875] <inf> button_handler: Button pressed!
[00:01:09.309,906] <inf> button_handler: Sensor logging enabled!
[00:01:10.469,848] <inf> sensor_task: Temperature and humidity Fetched.
[00:01:10.469,940] <inf> uart_task: Temperature = 24.21 C, Humidity = 49.62%
[00:01:12.471,069] <inf> sensor_task: Temperature and humidity Fetched.
[00:01:12.471,160] <inf> uart_task: Temperature = 24.21 C, Humidity = 49.59%
[00:01:14.472,290] <inf> sensor_task: Temperature and humidity Fetched.
[00:01:14.472,381] <inf> uart_task: Temperature = 24.21 C, Humidity = 49.63%
[00:01:16.473,510] <inf> sensor_task: Temperature and humidity Fetched.
[00:01:16.473,602] <inf> uart_task: Temperature = 24.21 C, Humidity = 49.63%
[00:01:18.474,731] <inf> sensor_task: Temperature and humidity Fetched.
[00:01:18.474,822] <inf> uart_task: Temperature = 24.18 C, Humidity = 49.59%
[00:01:20.475,952] <inf> sensor_task: Temperature and humidity Fetched.
[00:01:20.476,043] <inf> uart_task: Temperature = 24.18 C, Humidity = 49.59%
[00:01:22.477,172] <inf> sensor_task: Temperature and humidity Fetched.
[00:01:22.477,264] <inf> uart_task: Temperature = 24.20 C, Humidity = 49.67%
[00:01:24.478,393] <inf> sensor_task: Temperature and humidity Fetched.
[00:01:24.478,485] <inf> uart_task: Temperature = 24.20 C, Humidity = 49.72%
[00:01:26.479,614] <inf> sensor_task: Temperature and humidity Fetched.
[00:01:26.479,705] <inf> uart_task: Temperature = 24.18 C, Humidity = 49.82%
[00:01:28.218,170] <inf> button_handler: Button pressed!
[00:01:28.218,170] <inf> button_handler: Sensor logging disabled!
```

## Sensor Not Working + Disconencted
```
*** Booting nRF Connect SDK v2.7.0-5cb85570ca43 ***
*** Using Zephyr OS v3.6.99-100befc70c74 ***
[00:00:00.945,251] <inf> main: Environmental logger starting up!
[00:00:00.945,251] <err> sensor_task: Sensor is not ready
[00:00:00.945,281] <inf> led_handler: LED is ready
[00:00:00.945,281] <inf> led_handler: LED configured as output.
[00:00:00.945,526] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:00.945,587] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:02.945,831] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:02.945,892] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:04.946,166] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:04.946,228] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:06.946,472] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:06.946,533] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:08.946,777] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:08.946,838] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:10.947,082] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:10.947,143] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:12.947,387] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:12.947,448] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:14.947,692] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:14.947,753] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:16.947,998] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:16.948,059] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:18.948,303] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:18.948,364] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:20.948,608] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:20.948,669] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:22.948,913] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:22.948,974] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:24.949,218] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:24.949,279] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:26.949,523] <err> sensor_task: Failed to fetch temperature and humidity.
[00:00:26.949,584] <inf> uart_task: Temperature = -45.00 C, Humidity = 0.00%
[00:00:27.243,011] <inf> button_handler: Sensor logging disabled!
[00:00:28.275,695] <inf> button_handler: Sensor logging enabled!
```