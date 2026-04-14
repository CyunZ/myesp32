/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#loader-with-arc

#include "lvgl.h"
#include "esp_err.h"

extern esp_err_t http_get(char *url,char *response_str);
extern esp_err_t http_post(char *url,char *json_str,char *response_str);

lv_obj_t * label;
void set_mylabel(char *text)
{
    lv_label_set_text(label, text);
}

static void btn_event_get(lv_event_t * e)
{
    char response_str[2048+1];
    esp_err_t err = http_get("http://192.168.31.216:12345/music",response_str);
     if (err == ESP_OK) {
        lv_label_set_text(label, response_str);
    } else {
        lv_label_set_text(label, "HTTP GET FAIL");
    }
}


static void btn_event_post(lv_event_t * e)
{
    char response_str[2048+1];
    esp_err_t err = http_post("http://192.168.31.216:12345/giveme"
        ,"{\"what\":\"Yamato\"}",response_str);
     if (err == ESP_OK) {
        lv_label_set_text(label, response_str);
    } else {
        lv_label_set_text(label, "HTTP POST FAIL");
    }
}

void example_lvgl_demo_ui(lv_display_t *disp)
{
    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_set_width(label,200);
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);


    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_get, LV_EVENT_CLICKED, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label2 = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label2, "GET");                     /*Set the labels text*/
    lv_obj_center(label2);

    lv_obj_t * btn2 = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_align(btn2, LV_ALIGN_BOTTOM_LEFT, 0, 0);                          /*Set its position*/
    lv_obj_set_size(btn2, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn2, btn_event_post, LV_EVENT_CLICKED, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label3 = lv_label_create(btn2);          /*Add a label to the button*/
    lv_label_set_text(label3, "POST");                     /*Set the labels text*/
    lv_obj_center(label3);
}
