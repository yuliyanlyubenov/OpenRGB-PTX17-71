/*---------------------------------------------------------*\
| RGBController_SunrexKeyboard.h                            |
|                                                           |
|   RGBController for Sunrex Acer RGB Keyboards             |
|                                                           |
|   Yuliyan Lyubenov                            16 Mar 2025 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#pragma once

#include <atomic>
#include <chrono>
#include <thread>
#include "RGBController.h"
#include "SunrexKeyboardController.h"

#define SUNREX_KEYBOARD_ROWS           6
#define SUNREX_KEYBOARD_COLUMNS       17

class RGBController_SunrexKeyboard : public RGBController
{
public:
    RGBController_SunrexKeyboard(SunrexKeyboardController* controller_ptr);
    ~RGBController_SunrexKeyboard();

    void        SetupZones();
    void        ResizeZone(int zone, int new_size);

    void        DeviceUpdateLEDs();
    void        UpdateZoneLEDs(int zone);
    void        UpdateSingleLED(int led);

    void        DeviceUpdateMode();

private:
    SunrexKeyboardController*                          controller;
    std::chrono::time_point<std::chrono::steady_clock>  last_update_time;
};
