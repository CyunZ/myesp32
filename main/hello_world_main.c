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

#define BLINK_GPIO 18


#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO          (18) // Define the output GPIO
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits  2^13=8192
#define LEDC_DUTY               (4096) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY          (1000) // Frequency in Hertz. Set frequency at 4 kHz

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
    // // Prepare and then apply the LEDC PWM timer configuration
    // ledc_timer_config_t ledc_timer = {
    //     .speed_mode       = LEDC_MODE,
    //     .duty_resolution  = LEDC_DUTY_RES,
    //     .timer_num        = LEDC_TIMER,
    //     .freq_hz          = LEDC_FREQUENCY,  
    //     .clk_cfg          = LEDC_AUTO_CLK
    // };
    // ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // // Prepare and then apply the LEDC PWM channel configuration
    // ledc_channel_config_t ledc_channel = {
    //     .speed_mode     = LEDC_MODE,
    //     .channel        = LEDC_CHANNEL,
    //     .timer_sel      = LEDC_TIMER,
    //     .intr_type      = LEDC_INTR_DISABLE,
    //     .gpio_num       = LEDC_OUTPUT_IO,
    //     .duty           = 0, // Set duty to 0%
    //     .hpoint         = 0
    // };
    // ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    
    // while (1){
    //    for(int i=0;i<8192;i+=40){
    //         ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, i);
    //         ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    //         vTaskDelay(pdMS_TO_TICKS(20));
    //     }
    //     for(int j=8192;j>0;j-=40){
    //         ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, j);
    //         ledc_update_duty(LEDC_MODE, LEDC_CHANNEL);
    //         vTaskDelay(pdMS_TO_TICKS(20));
    //     }
    // }

    //4.按键输入
    gpio_config_t io_conf = {};
    io_conf.pull_down_en = 0;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1ULL<<32);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    int flag = 0;
    while (1) {
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
