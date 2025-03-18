/*---------------------------------------------------------*\
| SunrexKeyboardControllerDetect.cpp                        |
|                                                           |
|   Detector for Sunrex Acer RGB Keyboards                  |
|                                                           |
|   Yuliyan Lyubenov                            16 Mar 2025 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include <hidapi.h>
#include "Detector.h"
#include "SunrexKeyboardController.h"
#include "RGBController_SunrexKeyboard.h"

/*-----------------------------------------------------*\
| ZET keyboard VID/PID pairs                            |
\*-----------------------------------------------------*/
#define SUNREX_KEYBOARD_VID                     0x05AF
#define SUNREX_KEYBOARD_PID                     0x766C

void DetectSunrexKeyboard(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if (dev)
    {
        SunrexKeyboardController*     controller     = new SunrexKeyboardController(dev, info->path);
        RGBController_SunrexKeyboard* rgb_controller = new RGBController_SunrexKeyboard(controller);
        rgb_controller->name                          = name;

        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR_P("Acer RGB Keyboard (USB)", DetectSunrexKeyboard, SUNREX_KEYBOARD_VID, SUNREX_KEYBOARD_PID, 0xFF02);
