//
// Created by Zhenyu Zong on 2019/12/4.
//
// Codes originally from https://github.com/elanthis/upng.
//

/*
uPNG -- derived from LodePNG version 20100808

Copyright (c) 2005-2010 Lode Vandevenne
Copyright (c) 2010 Sean Middleditch

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

		1. The origin of this software must not be misrepresented; you must not
		claim that you wrote the original software. If you use this software
		in a product, an acknowledgment in the product documentation would be
		appreciated but is not required.

		2. Altered source versions must be plainly marked as such, and must not be
		misrepresented as being the original software.

		3. This notice may not be removed or altered from any source
		distribution.
*/

#ifndef _UPNG_H
#define _UPNG_H

#include "../lib.h"

#define INT_MAX      2147483647      /* max value for an int */
#define MAX_PNG_HEIGHT      768      /* max # of pixels of height */
#define MAX_PNG_WIDTH      1024      /* max # of pixels of width */
#define RGBA_BYTES            4
#define MAX_PNG_SIZE    (MAX_PNG_HEIGHT * MAX_PNG_WIDTH * RGBA_BYTES)   /* Max size of the png file */

typedef enum upng_error {
    UPNG_EOK			= 0, /* success (no error) */
    UPNG_ENOMEM			= 1, /* memory allocation failed */
    UPNG_ENOTFOUND		= 2, /* resource not found (file missing) */
    UPNG_ENOTPNG		= 3, /* image data does not have a PNG header */
    UPNG_EMALFORMED		= 4, /* image data is not a valid PNG image */
    UPNG_EUNSUPPORTED	= 5, /* critical PNG chunk type is not supported */
    UPNG_EUNINTERLACED	= 6, /* image interlacing is not supported */
    UPNG_EUNFORMAT		= 7, /* image color format is not supported */
    UPNG_EPARAM			= 8  /* invalid parameter to method call */
} upng_error;

typedef enum upng_format {
    UPNG_BADFORMAT,
    UPNG_RGB8,
    UPNG_RGB16,
    UPNG_RGBA8,
    UPNG_RGBA16,
    UPNG_LUMINANCE1,
    UPNG_LUMINANCE2,
    UPNG_LUMINANCE4,
    UPNG_LUMINANCE8,
    UPNG_LUMINANCE_ALPHA1,
    UPNG_LUMINANCE_ALPHA2,
    UPNG_LUMINANCE_ALPHA4,
    UPNG_LUMINANCE_ALPHA8
} upng_format;

typedef enum upng_state {
    UPNG_ERROR		= -1,
    UPNG_DECODED	= 0,
    UPNG_HEADER		= 1,
    UPNG_NEW		= 2
} upng_state;

typedef enum upng_color {
    UPNG_LUM		= 0,
    UPNG_RGB		= 2,
    UPNG_LUMA		= 4,
    UPNG_RGBA		= 6
} upng_color;

typedef struct upng_source {
    const unsigned char*	buffer;
    unsigned long			size;
    char					owning;
} upng_source;

typedef struct upng_t upng_t;
struct upng_t {
    unsigned		width;
    unsigned		height;

    upng_color		color_type;
    unsigned		color_depth;
    upng_format		format;

    unsigned char*	buffer;
    unsigned long	size;

    upng_error		error;
    unsigned		error_line;

    upng_state		state;
    upng_source		source;
};

upng_t      upng_new_from_file  (uint8_t * file_buffer, long file_size);
void		upng_free			(upng_t* upng);

upng_error	upng_header			(upng_t* upng);
upng_error	upng_decode			(upng_t* upng);

upng_error	upng_get_error		(const upng_t* upng);
unsigned	upng_get_error_line	(const upng_t* upng);

unsigned	upng_get_width		(const upng_t* upng);
unsigned	upng_get_height		(const upng_t* upng);
unsigned	upng_get_bpp		(const upng_t* upng);
unsigned	upng_get_bitdepth	(const upng_t* upng);
unsigned	upng_get_components	(const upng_t* upng);
unsigned	upng_get_pixelsize	(const upng_t* upng);
upng_format	upng_get_format		(const upng_t* upng);

const unsigned char*	upng_get_buffer		(const upng_t* upng);
unsigned				upng_get_size		(const upng_t* upng);

unsigned    sanity_check_good   (unsigned long size);

#endif //_UPNG_H
