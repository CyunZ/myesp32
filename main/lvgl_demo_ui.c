/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

// This demo UI is adapted from LVGL official example: https://docs.lvgl.io/master/examples.html#loader-with-arc

#include "lvgl.h"

lv_obj_t * label;
void set_mylabel(char *text)
{
    lv_label_set_text(label, text);
}


void example_lvgl_demo_ui(lv_display_t *disp)
{
    label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}
