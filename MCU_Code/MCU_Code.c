#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/clocks.h"
#include "sensors.h"
#include "sdCard.h"
#include "sd_card.h"
#include "buttons.h"
#include "pico/util/queue.h"

#include <time.h>

int64_t alarm_callback(alarm_id_t id, void *user_data) {
    // Put your timeout handler code in here
    return 0;
}

//Sensor Data Buffer
static struct sensorData sensorBuffer[800];
static struct tm date[800];
uint16_t BufferCounter = 0;

int main()
{

    stdio_init_all();


    // // Timer example code - This example fires off the callback after 2000ms
    // add_alarm_in_ms(2000, alarm_callback, NULL, false);
    // // For more examples of timer use see https://github.com/raspberrypi/pico-examples/tree/master/timer

    printf("System Clock Frequency is %d Hz\n", clock_get_hz(clk_sys));
    printf("USB Clock Frequency is %d Hz\n", clock_get_hz(clk_usb));
    // For more examples of clocks use see https://github.com/raspberrypi/pico-examples/tree/master/clocks
    

    /*Start of non example code*/
    //Init both I2C0 and I2C1
    configI2C();

    //Temp Sensor ADC Setup
    TMP_ADC_setup();

    //Init SD Card Setup (hw_config.c sets the SPI pins)
    // sd_init_driver();

    //Setup Buttons
    buttonsInit();

    // //Setup External ADC (ADS1115)
    configExtADC((((((((CONFIG_DEFAULT & ~CONFIG_MUX_MASK) | CONFIG_MUX_AIN0_GND) & ~CONFIG_PGA_MASK) | CONFIG_PGA_4p096V) & ~CONFIG_MODE_MASK) | CONFIG_MODE_CONT) & ~CONFIG_DR_MASK) | CONFIG_DR_475SPS);
    
    // //Init Queue (NEED TO TEST)
    // queue_t shareQueue;
    // queue_init(&shareQueue, 32, 10);
    // aon_timer_start_with_timeofday();
    // struct tm time;
    
    while (true) {
        // PM_printManID(PM1);
        // PM_printManID(PM2);
        // PM_printManID(PM3);

        /* Sensor Loop*/

        //Power Monitors
        sensorBuffer[BufferCounter].PM1voltage = PM_readVoltage(PM1);
        sensorBuffer[BufferCounter].PM1current = PM_readCurrent(PM1);

        sensorBuffer[BufferCounter].PM2voltage = PM_readVoltage(PM2);
        sensorBuffer[BufferCounter].PM2current = PM_readCurrent(PM2);

        sensorBuffer[BufferCounter].PM3voltage = PM_readVoltage(PM3);
        sensorBuffer[BufferCounter].PM3current = PM_readCurrent(PM3);

        //Temperature
        sensorBuffer[BufferCounter].temperature = readTempature(2, 5);
        
        //Irradiance
        sensorBuffer[BufferCounter].irradiance = readExtADC();
        // Returns irradance converted voltage value


        // //Write inital header data below
        printf("\nTimestamp, PM1 (V), PM1(I), PM1(W), PM2 (V), PM2(I), PM2(W), PM3 (V), PM3(I), PM3(W), Temp (C), Light (W/m^2)");
        
        //Write row data
        printf("\nTIME, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f", 
        sensorBuffer[BufferCounter].PM1voltage, sensorBuffer[BufferCounter].PM1current, sensorBuffer[BufferCounter].PM1power, 
        sensorBuffer[BufferCounter].PM2voltage, sensorBuffer[BufferCounter].PM2current, sensorBuffer[BufferCounter].PM2power, sensorBuffer[BufferCounter].PM3voltage, 
        sensorBuffer[BufferCounter].PM3current, sensorBuffer[BufferCounter].PM3power, sensorBuffer[BufferCounter].temperature, sensorBuffer[BufferCounter].irradiance);

        // //Sprintf to format the data
        // char formatString[32];
        // aon_timer_get_time(&time);
        // sprintf(&formatString, "%02d:%02d:%02d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f", 
        // time.tm_hour, time.tm_min, time.tm_sec,
        // sensorBuffer[BufferCounter].PM1voltage, sensorBuffer[BufferCounter].PM1current, sensorBuffer[BufferCounter].PM1power, 
        // sensorBuffer[BufferCounter].PM2voltage, sensorBuffer[BufferCounter].PM2current, sensorBuffer[BufferCounter].PM2power, sensorBuffer[BufferCounter].PM3voltage, 
        // sensorBuffer[BufferCounter].PM3current, sensorBuffer[BufferCounter].PM3power, sensorBuffer[BufferCounter].temperature, sensorBuffer[BufferCounter].irradiance);

        // queue_try_add(&shareQueue, &formatString);

        if(BufferCounter++ > 800){
            BufferCounter = 0;
        }
        /*End sensor loop*/

        //Notes
        /*Core 1 read current count value and display that value on the oled screen
        SD card: need to decide how often to write to sd card, maybe reading 400 behind??
        Then make a copy into a new array so that everything is formatted correctly, then save to SD card*/
    }
}
