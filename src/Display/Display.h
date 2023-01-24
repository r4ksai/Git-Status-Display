#pragma once
#include "MD_MAX72xx.h"
#include "Debug.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4 // Screens
#define CLK_PIN  D5 
#define DATA_PIN D7 
#define CS_PIN   D8 

#define DIMNESS 15

#define CURRENT_ROW 7
#define CURRENT_COL 0

// Loading Animation
#define ANIMATION_DELAY 75	// milliseconds
#define MAX_FRAMES      4   // number of animation frames
#define BLINK_DELAY     200

const uint8_t pacman[MAX_FRAMES][18] =  // pacman pursued by a ghost
{
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0x7e, 0xff, 0xe7, 0xc3, 0x81, 0x00 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xe7, 0xe7, 0x42, 0x00 },
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xe7, 0x66, 0x24 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c },
};
const uint8_t DATA_WIDTH = (sizeof(pacman[0])/sizeof(pacman[0][0]));

    
class RawDisplay : MD_MAX72XX {
    private:
        uint32_t prevTimeAnim = 0;  // remember the millis() value in animations
        int16_t idx;                // display index (column)
        uint8_t frame;              // current animation frame
        uint8_t deltaFrame;         // the animation frame offset for the next frame
        boolean blink;
        boolean isLoading;

        byte reverseByte(byte x);

    public:
        RawDisplay();
        bool loading();
        void begin();
        void gitStatus(byte status[]);
};