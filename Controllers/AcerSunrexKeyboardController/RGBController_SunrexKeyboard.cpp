/*---------------------------------------------------------*\
| RGBController_SunrexKeyboard.cpp                          |
|                                                           |
|   RGBController for Acer Sunrex USB Keyboard              |
|                                                           |
|   Yuliyan Lyubenov                            18 Mar 2025 |
|                                                           |
|   This file is part of the OpenRGB project                |
|   SPDX-License-Identifier: GPL-2.0-only                   |
\*---------------------------------------------------------*/

#include "RGBControllerKeyNames.h"
#include "RGBController_SunrexKeyboard.h"

//0xFFFFFFFF indicates an unused entry in matrix
#define NA  0xFFFFFFFF

static unsigned int matrix_map[SUNREX_KEYBOARD_ROWS][SUNREX_KEYBOARD_COLUMNS] =
{
  { 0, 6,11,17,23,28,33,39,44,49,55,61,66,69,71,74,80 },
  { 1, 7,12,18,24,29,34,40,45,50,56,62,67,NA,NA,75,81 },
  { 2, 8,13,19,25,30,35,41,46,51,57,63,68,NA,NA,76,82 },
  { 3, 9,14,20,26,31,36,42,47,52,58,64,NA,NA,NA,77,83 },
  { 4,NA,15,21,27,32,37,43,48,53,59,65,NA,NA,72,78,84 },
  { 5,10,16,22,NA,NA,38,NA,NA,54,60,NA,NA,70,73,79,85 }
};

static const char* zone_names[] =
{
    ZONE_EN_KEYBOARD,
};

static zone_type zone_types[] =
{
    ZONE_TYPE_MATRIX,
};

static const unsigned int zone_sizes[] =
{
    86,
};

static const char *led_names[] =
{
    KEY_EN_ESCAPE,
    // Skip index 1
    KEY_EN_BACK_TICK,
    KEY_EN_TAB,
    KEY_EN_CAPS_LOCK,
    KEY_EN_LEFT_SHIFT,
    KEY_EN_LEFT_CONTROL,
    KEY_EN_F1,
    KEY_EN_1,
    KEY_EN_Q,
    KEY_EN_A,
    // Skip index 10
    KEY_EN_LEFT_FUNCTION,
    KEY_EN_F2,
    KEY_EN_2,
    KEY_EN_W,
    KEY_EN_S,
    KEY_EN_Z,
    KEY_EN_LEFT_WINDOWS,
    KEY_EN_F3,
    KEY_EN_3,
    KEY_EN_E,
    KEY_EN_D,
    KEY_EN_X,
    KEY_EN_LEFT_ALT,
    KEY_EN_F4,
    KEY_EN_4,
    KEY_EN_R,
    KEY_EN_F,
    KEY_EN_C,
    KEY_EN_F5,
    KEY_EN_5,
    KEY_EN_T,
    KEY_EN_G,
    KEY_EN_V,
    KEY_EN_F6,
    KEY_EN_6,
    KEY_EN_Y,
    KEY_EN_H,
    KEY_EN_B,
    KEY_EN_SPACE,
    KEY_EN_F7,
    KEY_EN_7,
    KEY_EN_U,
    KEY_EN_J,
    KEY_EN_N,
    KEY_EN_F8,
    KEY_EN_8,
    KEY_EN_I,
    KEY_EN_K,
    KEY_EN_M,
    KEY_EN_F9,
    KEY_EN_9,
    KEY_EN_O,
    KEY_EN_L,
    KEY_EN_COMMA,
    KEY_EN_RIGHT_ALT,
    KEY_EN_F10,
    KEY_EN_0,
    KEY_EN_P,
    KEY_EN_SEMICOLON,
    KEY_EN_PERIOD,
    KEY_EN_MENU,
    KEY_EN_F11,
    KEY_EN_MINUS,
    KEY_EN_LEFT_BRACKET,
    KEY_EN_QUOTE,
    KEY_EN_FORWARD_SLASH,
    KEY_EN_F12,
    KEY_EN_PLUS,
    KEY_EN_RIGHT_BRACKET,
    KEY_EN_PRINT_SCREEN,
    KEY_EN_BACK_SLASH,
    KEY_EN_INSERT,
    KEY_EN_RIGHT_SHIFT,
    KEY_EN_LEFT_ARROW,
    KEY_EN_DELETE,
    KEY_EN_BACKSPACE,
    KEY_EN_BACK_SLASH,
    KEY_EN_ANSI_ENTER,
    KEY_EN_UP_ARROW,
    KEY_EN_DOWN_ARROW,
    KEY_EN_POWER,
    KEY_EN_POUND,
    KEY_EN_MEDIA_PLAY_PAUSE,
    KEY_EN_MEDIA_PREVIOUS,
    KEY_EN_MEDIA_NEXT,
    KEY_EN_RIGHT_ARROW
};

/**------------------------------------------------------------------*\
    @name Sunrex Keyboard
    @category Keyboard
    @type USB
    @save :x:
    @direct :x:
    @effects :white_check_mark:
    @detectors DetectSunrexKeyboard
    @comment
\*-------------------------------------------------------------------*/

RGBController_SunrexKeyboard::RGBController_SunrexKeyboard(SunrexKeyboardController* controller_ptr)
{
    controller  = controller_ptr;

    name        = "Acer RGB Gaming Keyboard";
    vendor      = "Acer";
    type        = DEVICE_TYPE_KEYBOARD;
    description = "Acer RGP Gaming Keyboard Device (Sunrex vendor ID)";
    location    = controller->GetDeviceLocation();
    serial      = controller->GetSerialString();

    mode Custom;
    Custom.name                 = "Custom";
    Custom.value                = SUNREX_KEYBOARD_MODE_CUSTOM;
    Custom.flags                = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Custom.color_mode           = MODE_COLORS_PER_LED;
    Custom.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Custom.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Custom.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    modes.push_back(Custom);

    mode Static;
    Static.name                 = "Static";
    Static.value                = SUNREX_KEYBOARD_MODE_STATIC;
    Static.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Static.color_mode           = MODE_COLORS_MODE_SPECIFIC;
    Static.colors_min           = 1;
    Static.colors_max           = 1;
    Static.colors.resize(Static.colors_max);
    Static.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Static.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Static.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Static.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Static);

    mode Waves;
    Waves.name                 = "Waves";
    Waves.value                = SUNREX_KEYBOARD_MODE_WAVES;
    Waves.flags                = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Waves.color_mode           = MODE_COLORS_NONE;
    Waves.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Waves.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Waves.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Waves.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Waves.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Waves.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Waves);

    mode Ripple;
    Ripple.name                 = "Ripple";
    Ripple.value                = SUNREX_KEYBOARD_MODE_RIPPLE;
    Ripple.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Ripple.color_mode           = MODE_COLORS_MODE_SPECIFIC;
    Ripple.colors_min           = 1;
    Ripple.colors_max           = 1;
    Ripple.colors.resize(Ripple.colors_max);
    Ripple.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Ripple.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Ripple.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Ripple.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Ripple.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Ripple.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Ripple);

    mode Raindrop;
    Raindrop.name                 = "Rain drop";
    Raindrop.value                = SUNREX_KEYBOARD_MODE_RAINDROP;
    Raindrop.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Raindrop.color_mode           = MODE_COLORS_MODE_SPECIFIC;
    Raindrop.colors_min           = 1;
    Raindrop.colors_max           = 1;
    Raindrop.colors.resize(Raindrop.colors_max);
    Raindrop.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Raindrop.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Raindrop.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Raindrop.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Raindrop.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Raindrop.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Raindrop);
    
    mode Fireball;
    Fireball.name                 = "Fireball";
    Fireball.value                = SUNREX_KEYBOARD_MODE_FIREBALL;
    Fireball.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Fireball.color_mode           = MODE_COLORS_MODE_SPECIFIC;
    Fireball.colors_min           = 1;
    Fireball.colors_max           = 1;
    Fireball.colors.resize(Fireball.colors_max);
    Fireball.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Fireball.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Fireball.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Fireball.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Fireball.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Fireball.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Fireball);

    mode Heartbeat;
    Heartbeat.name                 = "Heartbeat";
    Heartbeat.value                = SUNREX_KEYBOARD_MODE_HEARTBEAT;
    Heartbeat.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Heartbeat.color_mode           = MODE_COLORS_MODE_SPECIFIC;
    Heartbeat.colors_min           = 1;
    Heartbeat.colors_max           = 1;
    Heartbeat.colors.resize(Heartbeat.colors_max);
    Heartbeat.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Heartbeat.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Heartbeat.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Heartbeat.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Heartbeat.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Heartbeat.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Heartbeat);
    
    mode Breathing;
    Breathing.name                 = "Breathing";
    Breathing.value                = SUNREX_KEYBOARD_MODE_BREATHING;
    Breathing.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Breathing.color_mode           = MODE_COLORS_MODE_SPECIFIC;
    Breathing.colors_min           = 1;
    Breathing.colors_max           = 1;
    Breathing.colors.resize(Breathing.colors_max);
    Breathing.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Breathing.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Breathing.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Breathing.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Breathing.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Breathing.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Breathing);

    mode Snake;
    Snake.name                 = "Snake";
    Snake.value                = SUNREX_KEYBOARD_MODE_SNAKE;
    Snake.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Snake.color_mode       = MODE_COLORS_MODE_SPECIFIC;
    Snake.colors_min       = 1;
    Snake.colors_max       = 1;
    Snake.colors.resize(Snake.colors_max);
    Snake.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Snake.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Snake.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Snake.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Snake.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Snake.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Snake);

    mode Neon;
    Neon.name                 = "Neon";
    Neon.value                = SUNREX_KEYBOARD_MODE_NEON;
    Neon.flags                = MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Neon.color_mode           = MODE_COLORS_NONE;
    Neon.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Neon.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Neon.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Neon.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Neon.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Neon.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Neon);
    
    mode Lighting;
    Lighting.name                 = "Lighting";
    Lighting.value                = SUNREX_KEYBOARD_MODE_LIGHTING;
    Lighting.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Lighting.color_mode          = MODE_COLORS_MODE_SPECIFIC;
    Lighting.colors_min          = 1;
    Lighting.colors_max          = 1;
    Lighting.colors.resize(Lighting.colors_max);
    Lighting.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Lighting.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Lighting.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Lighting.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Lighting.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Lighting.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Lighting);

    mode Snow;
    Snow.name                 = "Snow";
    Snow.value                = SUNREX_KEYBOARD_MODE_SNOW;
    Snow.flags                = MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE;
    Snow.color_mode           = MODE_COLORS_MODE_SPECIFIC;
    Snow.colors_min           = 1;
    Snow.colors_max           = 1;
    Snow.colors.resize(Snow.colors_max);
    Snow.brightness_min       = SUNREX_KEYBOARD_BRIGHTNESS_MIN;
    Snow.brightness_max       = SUNREX_KEYBOARD_BRIGHTNESS_MAX;
    Snow.brightness           = SUNREX_KEYBOARD_BRIGHTNESS_DEF;
    Snow.speed_min            = SUNREX_KEYBOARD_SPEED_MIN;
    Snow.speed_max            = SUNREX_KEYBOARD_SPEED_MAX;
    Snow.speed                = SUNREX_KEYBOARD_SPEED_DEF;
    modes.push_back(Snow);

    SetupZones();
}

RGBController_SunrexKeyboard::~RGBController_SunrexKeyboard()
{
    /*---------------------------------------------------------*\
    | Delete the matrix map                                     |
    \*---------------------------------------------------------*/
    for (unsigned int zone_index = 0; zone_index < zones.size(); zone_index++)
    {
        if (zones[zone_index].matrix_map != nullptr)
        {
            delete zones[zone_index].matrix_map;
        }
    }

    delete controller;
}

void RGBController_SunrexKeyboard::SetupZones()
{
    /*---------------------------------------------------------*\
    | Set up zones                                              |
    \*---------------------------------------------------------*/
    unsigned int total_led_count = 0;
    for (unsigned int zone_idx = 0; zone_idx < 1; zone_idx++)
    {
        zone new_zone;
        new_zone.name                   = zone_names[zone_idx];
        new_zone.type                   = zone_types[zone_idx];
        new_zone.leds_min               = zone_sizes[zone_idx];
        new_zone.leds_max               = zone_sizes[zone_idx];
        new_zone.leds_count             = zone_sizes[zone_idx];

        if (zone_types[zone_idx] == ZONE_TYPE_MATRIX)
        {
            new_zone.matrix_map         = new matrix_map_type;
            new_zone.matrix_map->height = SUNREX_KEYBOARD_ROWS;
            new_zone.matrix_map->width  = SUNREX_KEYBOARD_COLUMNS;
            new_zone.matrix_map->map    = (unsigned int *)&matrix_map;
        }
        else
        {
            new_zone.matrix_map         = nullptr;
        }

        zones.push_back(new_zone);

        total_led_count += zone_sizes[zone_idx];
    }

    for (unsigned int led_idx = 0; led_idx < total_led_count; led_idx++)
    {
        led new_led;
        new_led.name                    = led_names[led_idx];
        leds.push_back(new_led);
    }

    SetupColors();
}

void RGBController_SunrexKeyboard::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_SunrexKeyboard::DeviceUpdateLEDs()
{
    last_update_time = std::chrono::steady_clock::now();

    if(active_mode == 0)
    {
        controller->SetLEDDirect(colors, modes[active_mode].brightness);
    }
}

void RGBController_SunrexKeyboard::UpdateZoneLEDs(int /*zone*/)
{
    DeviceUpdateLEDs();
}

void RGBController_SunrexKeyboard::UpdateSingleLED(int /*led*/)
{
    DeviceUpdateLEDs();
}

void RGBController_SunrexKeyboard::DeviceUpdateMode()
{
    bool          random            = (modes[active_mode].color_mode == MODE_COLORS_RANDOM || modes[active_mode].color_mode == MODE_COLORS_NONE);
    unsigned char mode_colors[3];

    mode_colors[0] = 0;
    mode_colors[1] = 0;
    mode_colors[2] = 0;

    if(modes[active_mode].color_mode == MODE_COLORS_MODE_SPECIFIC)
    {
        mode_colors[0] = RGBGetRValue(modes[active_mode].colors[0]);
        mode_colors[1] = RGBGetGValue(modes[active_mode].colors[0]);
        mode_colors[2] = RGBGetBValue(modes[active_mode].colors[0]);
    }

    controller->SetEffect(modes[active_mode].value,
                          modes[active_mode].speed,
                          modes[active_mode].brightness,
                          random,
                          mode_colors[0],
                          mode_colors[1],
                          mode_colors[2]);

    std::this_thread::sleep_for(std::chrono::milliseconds(15));
}

