/*---------------------------------------------------------*\
| SunrexKeyboardController.cpp                              |
|                                                           |
|   Driver for Acer Sunrex USB Keyboard                     |
|                                                           |
|   Yuliyan Lyubenov                            18 Mar 2025 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include <cstring>
#include "StringUtils.h"
#include "SunrexKeyboardController.h"

using namespace std::chrono_literals;

//0xFFFFFFFF indicates an unused entry in matrix
#define NA  0xFFFFFFFF

/*-----------------------------------------*\
| Skip these indices in the color output    |
\*-----------------------------------------*/
static const unsigned int SKIP_INDICES[] = { 1, 17, 18, 19, 20, 75, 77, 78, 79, 83, 85, 96, 98, 100, 108, 109, 111, 112, 113, 116, 123, 125 };

SunrexKeyboardController::SunrexKeyboardController(hid_device* dev_handle, const char* path)
{
    dev         = dev_handle;
    location    = path;

    effect_mode = SUNREX_KEYBOARD_MODE_STATIC;
}

SunrexKeyboardController::~SunrexKeyboardController()
{
    hid_close(dev);
}

std::string SunrexKeyboardController::GetDeviceLocation()
{
    return("HID " + location);
}

std::string SunrexKeyboardController::GetSerialString()
{
    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        return("");
    }

    return(StringUtils::wstring_to_string(serial_string));
}

void SunrexKeyboardController::PrepareHeader(unsigned char* packet, unsigned char brightness)
{
    PrepareHeader(packet, 0x1C, 0x02, brightness, 0xFF); // Custom 2, 2, -, separator
}

void SunrexKeyboardController::PrepareHeader(unsigned char* packet, unsigned char mode, unsigned char speed, unsigned char brightness, unsigned char color)
{
    /*-----------------------------------------------------*\
    | Prepare packet header                                 |
    \*-----------------------------------------------------*/
    packet[0x00]   = 0x04; // Report ID
    packet[0x01]   = 0xAE; // RGB Control Packet (KB = 0xA0)
    packet[0x02]   = 0x01; // unk
    packet[0x05]   = mode; // Mode
    packet[0x06]   = speed; // Speed, 0-4
    packet[0x07]   = brightness; // Brightness, 0-4
    packet[0x08]   = color; // Separator FF or Color, 0-7 (0-6 in static color mode) (Rainbow,) R, G, B, Y, M, C, W
}

void SunrexKeyboardController::SetLEDDirect(const std::vector<RGBColor>& colors, unsigned char brightness)
{
    unsigned char buf[9];

    /*-----------------------------------------------------*\
    | Zero out buffer and prepare packet header             |
    \*-----------------------------------------------------*/
    unsigned char one[9] = {0,0xb1,0x0,0x0,0x0,0x0,0x0,0x0,0x4e};
    unsigned char two[9] = {0,0x8,0x2,0x0,0x0,0x0,0x0,0x0,0xf5};
    unsigned char three[9] = {0,0x08,0x02,0x01,0x0a,0x32,0x00,0x00,0xb8};
    unsigned char four[9] = {0,0x14,0x00,0x00,0x29,0xc7,0x05,0x00,0xf6};

    memcpy(buf, one, sizeof(buf));
    hid_send_feature_report(dev, buf, sizeof(buf));

    memcpy(buf, two, sizeof(buf));
    hid_send_feature_report(dev, buf, sizeof(buf));

    memcpy(buf, three, sizeof(buf));
    hid_send_feature_report(dev, buf, sizeof(buf));

    memcpy(buf, four, sizeof(buf));
    if (colors.size() > 0) {
        buf[4] = RGBGetRValue(colors[0]);
        buf[5] = RGBGetGValue(colors[0]);
        buf[6] = RGBGetBValue(colors[0]);
        buf[8] = 0xeb - (buf[4]+buf[5]+buf[6]);
    }
    hid_send_feature_report(dev, buf, sizeof(buf));
    
    std::this_thread::sleep_for(SUNREX_KEYBOARD_DELAY);
}

unsigned char SunrexKeyboardController::RGBToPalette(unsigned char red,
                                                      unsigned char grn,
                                                      unsigned char blu
                                                     )
{
    /*------------------------*\
    | 0 0 1 (1) -> (1) Red     |
    | 0 1 0 (2) -> (2) Green   |
    | 0 1 1 (3) -> (4) Yellow  |
    | 1 0 0 (4) -> (3) Blue    |
    | 1 0 1 (5) -> (5) Magenta |
    | 1 1 0 (6) -> (6) Cyan    |
    | 1 1 1 (7) -> (7) White   |
    \*------------------------*/
    unsigned char color_mask = ((blu > 127) << 2 & 4) | ((grn > 127) << 1 & 2) | ((red > 127) & 1);

    switch(color_mask) // (Rainbow/Off,) R, G, B, Y, M, C, W
    {
        case 3:
            return 4;
        case 4:
            return 3;
        default:
            return color_mask;
    }
}

void SunrexKeyboardController::SetEffect(unsigned char mode,
                                     unsigned char speed,
                                     unsigned char brightness,
                                     bool          random,
                                     unsigned char red,
                                     unsigned char grn,
                                     unsigned char blu
                                    )
{
    unsigned char buf[9];

    /*-----------------------------------------------------*\
    | Zero out buffer and prepare packet header             |
    \*-----------------------------------------------------*/
    unsigned char one[9] = {0,0xb1,0x0,0x0,0x0,0x0,0x0,0x0,0x4e};
    unsigned char two[9] = {0,0x8,0x2,0x0,0x0,0x0,0x0,0x0,0xf5};
    unsigned char mstatic[9] = {0,0x08,0x02,0x01,0x0a,0x32,0x00,0x00,0xb8};
    unsigned char mwave[9] = {0,0x08,0x02,0x03,0x05,0x32,0x00,0x06,0xb5};
    unsigned char mripple[9] = {0,0x08,0x02,0x06,0x05,0x32,0x00,0x00,0xb8};
    unsigned char mfireball[9] = {0,0x08,0x02,0x27,0x05,0x32,0x00,0x00,0x97};
    unsigned char mheartbeat[9] = {0,0x08,0x02,0x29,0x05,0x32,0x00,0x00,0x95};
    unsigned char mbreathing[9] = {0,0x08,0x02,0x02,0x05,0x32,0x00,0x00,0xbc};
    unsigned char msnake[9] = {0,0x08,0x02,0x05,0x05,0x32,0x00,0x00,0xb9};
    unsigned char mneon[9] = {0,0x08,0x02,0x08,0x05,0x32,0x00,0x00,0xb6};
    unsigned char mlighting[9] = {0,0x08,0x02,0x12,0x05,0x32,0x00,0x00,0xac};
    unsigned char msnow[9] = {0,0x08,0x02,0x28,0x05,0x32,0x00,0x00,0x96};
    unsigned char four[9] = {0,0x14,0x00,0x00,0x0,0x0,0x0,0x00,0xeb};

    int send_color = 1;

    memcpy(buf, one, sizeof(buf));
    hid_send_feature_report(dev, buf, sizeof(buf));

    memcpy(buf, two, sizeof(buf));
    hid_send_feature_report(dev, buf, sizeof(buf));

    memcpy(buf, mstatic, sizeof(buf));
    switch(mode)
    {
    case SUNREX_KEYBOARD_MODE_CUSTOM: // TODO Custom:
        memcpy(buf, mstatic, sizeof(buf));
        break;
    case SUNREX_KEYBOARD_MODE_STATIC: // Static:
        memcpy(buf, mstatic, sizeof(buf));
        break;
    case SUNREX_KEYBOARD_MODE_WAVES: // Waves:
        memcpy(buf, mwave, sizeof(buf));
        send_color = 0;
        break;
    case SUNREX_KEYBOARD_MODE_RIPPLE: // Ripple:
        memcpy(buf, mripple, sizeof(buf));
        break;
    case SUNREX_KEYBOARD_MODE_FIREBALL: // Fireball:
        memcpy(buf, mfireball, sizeof(buf));
        break;
    case SUNREX_KEYBOARD_MODE_BREATHING: // Breathing:
        memcpy(buf, mbreathing, sizeof(buf));
        break;
    case SUNREX_KEYBOARD_MODE_SNAKE: // Snake:
        memcpy(buf, msnake, sizeof(buf));
        break;
    case SUNREX_KEYBOARD_MODE_NEON: // Neon:
        memcpy(buf, mneon, sizeof(buf));
        send_color = 0;
        break;
    case SUNREX_KEYBOARD_MODE_LIGHTING: // Lighting:
        memcpy(buf, mlighting, sizeof(buf));
        break;
    case SUNREX_KEYBOARD_MODE_SNOW: // Snow:
        memcpy(buf, msnow, sizeof(buf));
        break;
    default:
        break;
    }
    hid_send_feature_report(dev, buf, sizeof(buf));

    memcpy(buf, four, sizeof(buf));
    if (send_color) {
        buf[4] = red;
        buf[5] = grn;
        buf[6] = blu;
        buf[8] = 0xeb - (buf[4]+buf[5]+buf[6]);
    }
    hid_send_feature_report(dev, buf, sizeof(buf));
    
    std::this_thread::sleep_for(SUNREX_KEYBOARD_DELAY);
}

