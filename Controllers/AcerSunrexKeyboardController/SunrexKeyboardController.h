/*---------------------------------------------------------*\
| SunrexKeyboardController.h                                |
|                                                           |
|   Driver for Acer Sunrex USB Keyboard                     |
|                                                           |
|   Yuliyan Lyubenov                            18 Mar 2025 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#pragma once

#include <string>
#include <hidapi.h>
#include "RGBController.h"

#define SUNREX_KEYBOARD_DELAY       12ms
#define SUNREX_KEYBOARD_HEADER_LEN     9
#define SUNREX_KEYBOARD_COLOR_LEN      4
#define SUNREX_KEYBOARD_KEY_OFFSET  0x80

#define SUNREX_KEYBOARD_SPEED_MIN      0
#define SUNREX_KEYBOARD_SPEED_MAX      4
#define SUNREX_KEYBOARD_SPEED_DEF      2
#define SUNREX_KEYBOARD_BRIGHTNESS_MIN 0
#define SUNREX_KEYBOARD_BRIGHTNESS_MAX 4
#define SUNREX_KEYBOARD_BRIGHTNESS_DEF 4

#define SUNREX_KEYBOARD_MODE_OFF            0x01
#define SUNREX_KEYBOARD_MODE_CUSTOM         0x1C
#define SUNREX_KEYBOARD_MODE_STATIC         0x02
#define SUNREX_KEYBOARD_MODE_WAVES          0x03
#define SUNREX_KEYBOARD_MODE_RIPPLE         0x04
#define SUNREX_KEYBOARD_MODE_RAINDROP       0x05
#define SUNREX_KEYBOARD_MODE_FIREBALL       0x06
#define SUNREX_KEYBOARD_MODE_HEARTBEAT      0x07
#define SUNREX_KEYBOARD_MODE_BREATHING      0x08
#define SUNREX_KEYBOARD_MODE_SNAKE          0x09
#define SUNREX_KEYBOARD_MODE_NEON           0x0A
#define SUNREX_KEYBOARD_MODE_LIGHTING       0x0B
#define SUNREX_KEYBOARD_MODE_SNOW           0x0C

class SunrexKeyboardController
{
public:
    SunrexKeyboardController(hid_device* dev_handle, const char* path);
    ~SunrexKeyboardController();

    std::string                         GetDeviceLocation();
    std::string                         GetSerialString();

    void                                SetLEDDirect(const std::vector<RGBColor>& colors, unsigned char brightness);

    void                                SetEffect(unsigned char mode, unsigned char speed, unsigned char brightness, bool random, unsigned char red1, unsigned char grn1, unsigned char blu1);
private:
    hid_device*                         dev;
    std::string                         location;
    unsigned int                        effect_mode;
    bool                                custom_mode;
    void                                PrepareHeader(unsigned char *packet, unsigned char brightness);
    void                                PrepareHeader(unsigned char *packet, unsigned char mode, unsigned char speed, unsigned char brightness, unsigned char color);
    unsigned char                       RGBToPalette(unsigned char red, unsigned char grn, unsigned char blu);
};
