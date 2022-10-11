#include "DigitalOut.h"
#include "InterruptIn.h"
#include "PinNameAliases.h"
#include "Ticker.h"
#include "mbed.h"
#include "PinNames.h"
#include "BME280/BME280.h"


DigitalOut oled1(LED1);
InterruptIn ibutton(BUTTON1);
BME280 sensor(I2C_SDA, I2C_SCL); // SDCL/SDA

bool mode = true;

Ticker toggle_led_ticker;

void led_ticker_led1(){
    oled1 = !oled1;
}

void pressed(void){
    mode = !mode;
}


// main() runs in its own thread in the OS
int main()
{
    int old_temp = (int)sensor.getTemperature();
    int old_humidity = (int)sensor.getHumidity();

    oled1 = 1;
        
    ibutton.rise(&pressed);

    bool old_mode = true;

    while (true) {
        int temperature = (int)sensor.getTemperature();
        int humidity = (int)sensor.getHumidity();

        if (temperature - old_temp == 0 && humidity - old_humidity == 0 && mode == old_mode){
            continue;
        }

        printf("Temperature: %d, humidity: %d, mode: %d\n", temperature, humidity, mode);

        if (mode){
            if (temperature < 27 && humidity < 45){
                toggle_led_ticker.detach();
                toggle_led_ticker.attach(&led_ticker_led1, 500ms);
            }
            else if (temperature < 27 && humidity >= 45){
                toggle_led_ticker.detach();
                oled1 = 1;
            }
            else{
                toggle_led_ticker.detach();
                oled1 = 0;
            }
        }
        else{
            if (temperature < 27 && humidity < 45){
                toggle_led_ticker.detach();
                oled1 = 0;
            }
            else if (temperature < 27 && humidity >= 45){
                toggle_led_ticker.detach();
                oled1 = 1;
            }
            else{
                toggle_led_ticker.detach();
                toggle_led_ticker.attach(&led_ticker_led1, 200ms);
            }
        }
        old_temp = temperature;
        old_humidity = humidity;

        old_mode = mode;
    }
}

