
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdatomic.h>

#define LED_GPIO GPIO_NUM_32
#define BUTTON_GPIO GPIO_NUM_22
#define BLINK_PERIOD_MS 500
static TaskHandle_t blink_task_handle = NULL;

void blink_led(void *args)
{
    vTaskDelay(10 / portTICK_PERIOD_MS);
    while (1) {
        gpio_set_level(LED_GPIO, 1);
        vTaskDelay(BLINK_PERIOD_MS / portTICK_PERIOD_MS);
        gpio_set_level(LED_GPIO, 0);
        vTaskDelay(BLINK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    gpio_reset_pin(LED_GPIO);
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    static atomic_bool paused = ATOMIC_VAR_INIT(false); ;
    xTaskCreate(blink_led, "blink_led", 2048, NULL, 5, &blink_task_handle);
    while(true){
        if(!gpio_get_level(BUTTON_GPIO)){
            if(paused){
                vTaskResume(blink_task_handle);
            } else {
                vTaskSuspend(blink_task_handle);
                gpio_set_level(LED_GPIO, 0); //comment out this line to play a game of "catch the led" [WOWWWWWWW, intellisense knew i was gonna name it that]
            }
            paused = !paused;
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}
