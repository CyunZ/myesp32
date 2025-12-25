/*
 * SPDX-FileCopyrightText: 2010-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include <stdio.h>
#include <inttypes.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_chip_info.h"
#include "esp_flash.h"
#include "esp_system.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "iot_button.h"
#include "button_gpio.h"

#define BLINK_GPIO 18


#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (4) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits  2^13=8192
#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (1000) // Frequency in Hertz. Set frequency at 4 kHz

int flag = 0;

static void button_event_cb(void *arg, void *data)
{
    iot_button_print_event((button_handle_t)arg);
    if (iot_button_get_event((button_handle_t)arg) == BUTTON_SINGLE_CLICK){
        flag = !flag;
        gpio_set_level(BLINK_GPIO,flag);
    }
}


void button_init(uint32_t button_num)
{
    button_config_t btn_cfg = {0};
    button_gpio_config_t gpio_cfg = {
        .gpio_num = button_num,
        .active_level = 0,
        .enable_power_save = false,
    };

    button_handle_t btn;
    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &gpio_cfg, &btn);
    assert(ret == ESP_OK);

    ret = iot_button_register_cb(btn, BUTTON_PRESS_DOWN, NULL, button_event_cb, NULL);
    ret |= iot_button_register_cb(btn, BUTTON_PRESS_UP, NULL, button_event_cb, NULL);
    ret |= iot_button_register_cb(btn, BUTTON_PRESS_REPEAT, NULL, button_event_cb, NULL);
    ret |= iot_button_register_cb(btn, BUTTON_PRESS_REPEAT_DONE, NULL, button_event_cb, NULL);
    ret |= iot_button_register_cb(btn, BUTTON_SINGLE_CLICK, NULL, button_event_cb, NULL);
    ret |= iot_button_register_cb(btn, BUTTON_DOUBLE_CLICK, NULL, button_event_cb, NULL);
    ret |= iot_button_register_cb(btn, BUTTON_LONG_PRESS_START, NULL, button_event_cb, NULL);
    ret |= iot_button_register_cb(btn, BUTTON_LONG_PRESS_HOLD, NULL, button_event_cb, NULL);
    ret |= iot_button_register_cb(btn, BUTTON_LONG_PRESS_UP, NULL, button_event_cb, NULL);
    ret |= iot_button_register_cb(btn, BUTTON_PRESS_END, NULL, button_event_cb, NULL);

    ESP_ERROR_CHECK(ret);
}

void buttonLED_task(void *arg){
    int flag = 0;
    while (1) {
        // gpio_set_level(BLINK_GPIO, gpio_get_level(32));
        if (0 == gpio_get_level(32)){
            vTaskDelay(50 / portTICK_PERIOD_MS);
            if (0 == gpio_get_level(32)){
                while(0 == gpio_get_level(32)){
                    vTaskDelay(10 / portTICK_PERIOD_MS);
                }
                flag = !flag;
                gpio_set_level(BLINK_GPIO,flag);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

void pwmLED_task(void *arg){
    while (1){
       for(int i=0;i<8192;i+=40){
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, i);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        for(int j=8192;j>0;j-=40){
            ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, j);
            ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
            vTaskDelay(pdMS_TO_TICKS(20));
        }
    }
}

void app_main(void)
{
    //1.测试
    printf("测试！！！！！！！！！！！！\n");
    fflush(stdout);
    //2.点亮LED
    gpio_reset_pin(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    // gpio_set_level(BLINK_GPIO, 1);

    //3.呼吸灯
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    


    //4.按键输入
    // gpio_config_t io_conf = {};
    // io_conf.pull_down_en = 0;
    // io_conf.intr_type = GPIO_INTR_DISABLE;
    // io_conf.pin_bit_mask = (1ULL<<32);
    // io_conf.mode = GPIO_MODE_INPUT;
    // io_conf.pull_up_en = 1;
    // gpio_config(&io_conf);

    
    button_init(32);

    // xTaskCreate(buttonLED_task,"buttonLED_task",2048,NULL,10,NULL);
    xTaskCreate(pwmLED_task,"pwmLED_task",2048,NULL,10,NULL);
}
