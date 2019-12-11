//
// Created by liuzikai on 12/7/19.
//

#include "gui_objs.h"

#include "gui_font_data.h"

#include "../file_system.h"
#include "upng.h"

static unsigned char _png_buf[MAX_PNG_SIZE];  // use as png channel buffer
static vga_argb _png_data[VGA_WIDTH * VGA_HEIGHT];  // use as png file buffer and final png buffer

static unsigned char desktop_canvas[VGA_SCREEN_BYTES];
gui_object_t gui_obj_desktop;

void ca_clear(unsigned char *canvas, unsigned int size) {
    memset(canvas, 0, size);
}

void ca_draw_pixel(unsigned char *canvas, int x, int y, vga_argb argb) {
    unsigned long offset = y * vga_info.xbytes + x * VGA_BYTES_PER_PIXEL;
    *(unsigned short *) (canvas + offset) =
            color_convert(rgb_blend(color_revert(*(unsigned short *) (canvas + offset)),
                                    argb,
                                    alpha(argb)));
}

int load_png(const char *fname, int expected_width, int expected_height, vga_argb *png_data) {
    dentry_t test_png;
    int32_t readin_size;

    // Open the png file and read it into buffer
    int32_t read_png = read_dentry_by_name((const uint8_t *) fname, &test_png);
    if (0 == read_png) {
        readin_size = read_data(test_png.inode_num, 0, (unsigned char *) _png_data, sizeof(_png_data));
        if (readin_size == sizeof(_png_data)) {
            DEBUG_ERR("draw_png(): PNG BUFFER NOT ENOUGH!");
            return -1;
        }
    } else {
        DEBUG_ERR("draw_png: png file %f does not exist!", fname);
        return -1;
    }

    upng_t upng;
    unsigned width;
    unsigned height;
    unsigned px_size;
    const unsigned char *buffer;
    unsigned int idx;

    upng = upng_new_from_file((unsigned char *) _png_data, (long) readin_size);
    upng.buffer = (unsigned char *) &_png_buf;
    upng_decode(&upng);
    if (upng_get_error(&upng) != UPNG_EOK) {
        DEBUG_ERR("draw_png: fail to decode png %s", fname);
        return -1;
    }

    width = upng_get_width(&upng);
    if (width != expected_width) {
        DEBUG_WARN("draw_png(): %s does not meet expected width", fname);
    }
    height = upng_get_height(&upng);
    if (height != expected_height) {
        DEBUG_WARN("draw_png(): %s does not meet expected height", fname);
    }

    px_size = upng_get_pixelsize(&upng) / 8;
    buffer = upng_get_buffer(&upng);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            idx = (j * width + i) * px_size;
            if (px_size == 3) {
                png_data[j * width + i] = 0xFF000000 | buffer[idx + 0] << 16 | buffer[idx + 1] << 8 | buffer[idx + 2];
            } else if (px_size == 4) {
                png_data[j * width + i] =
                        buffer[idx + 3] << 24 | buffer[idx + 0] << 16 | buffer[idx + 1] << 8 | buffer[idx + 2];
            } else {
                DEBUG_ERR("draw_png(): invalid pixel size %d", px_size);
                return -1;
            }
        }
    }

    return 0;
}

int render_png_to_obj(vga_argb *png_data, unsigned width, unsigned height,
                      unsigned char *canvas, int x_offset, int y_offset, gui_object_t *gui_object) {

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            if (canvas == NULL) {
                // Draw to video memory
                vga_set_color_argb(png_data[j * width + i]);
                vga_draw_pixel(i + x_offset, j + y_offset);
            } else {
                // Draw to canvas
                ca_draw_pixel(canvas, i + x_offset, j + y_offset, png_data[j * width + i]);
            }
        }
    }

    gui_object->canvas = canvas;
    gui_object->x = x_offset;
    gui_object->y = y_offset;
    gui_object->width = width;
    gui_object->height = height;

    return 0;
}

static void init_font_obj(vga_rgb fg, vga_rgb bg) {
    int ch;
    int i, j;
    int x, y;

    for (ch = 0; ch < 256; ch++) {

        x = (ch & 0x7F) * FONT_WIDTH;
        y = VGA_HEIGHT * 2 + (ch / 128) * FONT_HEIGHT;

        for (i = 0; i < FONT_WIDTH; i++) {
            for (j = 0; j < FONT_HEIGHT; j++) {
                vga_set_color_argb((font_data[ch][j] & (1 << (7 - i)) ? fg : bg));
                vga_draw_pixel(x + i, y + j);
            }
        }
    }
}

gui_object_t gui_get_obj_font(char ch) {
    return (gui_object_t) {NULL,
                           (ch & 0x7F) * FONT_WIDTH, VGA_HEIGHT * 2 + (ch / 128) * FONT_HEIGHT,
                           FONT_WIDTH, FONT_HEIGHT};
}

static void init_desktop_obj() {
    ca_clear(desktop_canvas, sizeof(desktop_canvas));
    load_png("background_a.png", VGA_WIDTH, VGA_HEIGHT, _png_data);
    render_png_to_obj(_png_data, VGA_WIDTH, VGA_HEIGHT, desktop_canvas, 0, 0, &gui_obj_desktop);
}

vga_argb gui_win_up[WIN_UP_WIDTH * WIN_UP_HEIGHT];
vga_argb gui_win_down[WIN_DOWN_WIDTH * WIN_DOWN_HEIGHT];
vga_argb gui_win_left[WIN_LEFT_WIDTH * WIN_LEFT_HEIGHT];
vga_argb gui_win_right[WIN_RIGHT_WIDTH * WIN_RIGHT_HEIGHT];
vga_argb gui_win_red[2][WIN_RED_B_WIDTH * WIN_RED_B_HEIGHT];
vga_argb gui_win_yellow[2][WIN_YELLOW_B_WIDTH * WIN_YELLOW_B_HEIGHT];
vga_argb gui_win_green[2][WIN_GREEN_B_WIDTH * WIN_GREEN_B_HEIGHT];


static void init_window_png() {
    load_png("up_window.png", WIN_UP_WIDTH, WIN_UP_HEIGHT, gui_win_up);
    load_png("down_window.png", WIN_DOWN_WIDTH, WIN_DOWN_HEIGHT, gui_win_down);
    load_png("left_window.png", WIN_LEFT_WIDTH, WIN_LEFT_HEIGHT, gui_win_left);
    load_png("right_window.png", WIN_LEFT_WIDTH, WIN_LEFT_HEIGHT, gui_win_right);

    load_png("red_b.png", WIN_RED_B_WIDTH, WIN_RED_B_HEIGHT, gui_win_red[0]);
    load_png("yellow_b.png", WIN_YELLOW_B_WIDTH, WIN_YELLOW_B_HEIGHT, gui_win_yellow[0]);
    load_png("green_b.png", WIN_GREEN_B_WIDTH, WIN_GREEN_B_HEIGHT, gui_win_green[0]);

    // FIXME: normal red button?
    load_png("red_b.png", WIN_RED_B_WIDTH, WIN_RED_B_HEIGHT, gui_win_red[1]);
    load_png("yellow_b_c.png", WIN_YELLOW_B_WIDTH, WIN_YELLOW_B_HEIGHT, gui_win_yellow[1]);
    load_png("green_b_c.png", WIN_GREEN_B_WIDTH, WIN_GREEN_B_HEIGHT, gui_win_green[1]);
}

void gui_obj_load() {
    init_desktop_obj();
    init_font_obj(GUI_FONT_FORECOLOR_ARGB, GUI_FONT_BACKCOLOR_ARGB);
    init_window_png();
}