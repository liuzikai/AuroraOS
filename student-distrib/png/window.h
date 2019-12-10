//
// Created by Zhenyu Zong on 2019/12/9.
//

#ifndef _WINDOW_H
#define _WINDOW_H

#include "png.h"
#include "../vga/vga.h"
#include "../file_system.h"

#define TERMINAL_X 50
#define TERMINAL_Y 60

void draw_png(const uint8_t *fname, int x_offset, int y_offset);
void full_screen_png();
void draw_terminal_status_bar();
void draw_pressed_yellow_button();
void draw_pressed_green_button();


#endif //_WINDOW_H
