/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#loader-with-arc

#include "lvgl.h"

extern void http_test(void);

lv_obj_t * label;
void set_mylabel(char *text)
{
    lv_label_set_text(label, text);
}

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        http_test();
    }
}

void example_lvgl_demo_ui(lv_display_t *disp)
{
    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);


    lv_obj_t * btn = lv_button_create(lv_screen_active());     /*Add a button the current screen*/
    lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "HTTP");                     /*Set the labels text*/
    lv_obj_center(label);
}
