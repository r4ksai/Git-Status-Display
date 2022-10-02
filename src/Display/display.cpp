#include "MD_MAX72xx.h"
#include "display.h"

#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define CS_PIN    5

// Loading Animation
#define ANIMATION_DELAY 75	// milliseconds
#define MAX_FRAMES      4   // number of animation frames
#define BLINK_DELAY     200

const uint8_t pacman[MAX_FRAMES][18] =  // ghost pursued by a pacman
{
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0x7e, 0xff, 0xe7, 0xc3, 0x81, 0x00 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xfb, 0x73, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xe7, 0xe7, 0x42, 0x00 },
  { 0xfe, 0x73, 0xfb, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xe7, 0x66, 0x24 },
  { 0xfe, 0x7b, 0xf3, 0x7f, 0xf3, 0x7b, 0xfe, 0x00, 0x00, 0x00, 0x3c, 0x7e, 0xff, 0xff, 0xff, 0xff, 0x7e, 0x3c },
};
const uint8_t DATA_WIDTH = (sizeof(pacman[0])/sizeof(pacman[0][0]));

    uint32_t prevTimeAnim = 0;  // remember the millis() value in animations
    int16_t idx;                // display index (column)
    uint8_t frame;              // current animation frame
    uint8_t deltaFrame;         // the animation frame offset for the next frame
    boolean blink;
    boolean isLoading;

    byte reverseByte(byte x) {

        return(
         x << 7        | 
         (x << 5 & 64) | 
         (x << 3 & 32) | 
         (x << 1 & 16) |
         x >> 7        | 
         (x >> 5 & 2)  | 
         (x >> 3 & 4)  | 
         (x >> 1 & 8)  
         );
    }

    RawDisplay::RawDisplay() : MD_MAX72XX(HARDWARE_TYPE, CS_PIN, MAX_DEVICES) {
    };

    void RawDisplay::begin() {
        MD_MAX72XX::begin();
        MD_MAX72XX::control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/15);
        MD_MAX72XX::control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
        MD_MAX72XX::clear();

        for (uint8_t i=0; i<32; i++)
            MD_MAX72XX::setColumn(i, 0b11111111);

        blink = false;
    }

    void RawDisplay::gitStatus(int x, int y, byte status[]) {

        if (millis()-prevTimeAnim < BLINK_DELAY)
            return;

        prevTimeAnim = millis();      // starting point for next time

        MD_MAX72XX::control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

        for (uint8_t i=0; i<32; i++)
        {
            byte dataColumn = status[i];
            if (i % 2 == 0)
                dataColumn = reverseByte(dataColumn);

            MD_MAX72XX::setColumn(i, dataColumn);
        }

        if (!(status[0] & 1))
        {
            MD_MAX72XX::setPoint(x, y, blink);
            blink = !blink;
        }

        MD_MAX72XX::control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
    }

    bool RawDisplay::loading() {
        static boolean bInit = true;  // initialise the animation
        isLoading = false;

        // Is it time to animate?
        if (millis()-prevTimeAnim < ANIMATION_DELAY)
            return isLoading;

        prevTimeAnim = millis();      // starting point for next time

        MD_MAX72XX::control(MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

        // Initialize
        if (bInit)
        {
            MD_MAX72XX::clear();
            idx = 0;
            frame = 0;
            deltaFrame = 1;
            bInit = false;

            // Lay out the dots
            for (uint8_t i=0; i<MAX_DEVICES; i++)
            {
            MD_MAX72XX::setPoint(3, (i*COL_SIZE) + 3, true);
            MD_MAX72XX::setPoint(4, (i*COL_SIZE) + 3, true);
            MD_MAX72XX::setPoint(3, (i*COL_SIZE) + 4, true);
            MD_MAX72XX::setPoint(4, (i*COL_SIZE) + 4, true);
            }
        }

        // clear old graphic
        for (uint8_t i=0; i<DATA_WIDTH; i++)
            MD_MAX72XX::setColumn(idx-DATA_WIDTH+i, 0);
        // move reference column and draw new graphic
        idx++;
        for (uint8_t i=0; i<DATA_WIDTH; i++)
            MD_MAX72XX::setColumn(idx-DATA_WIDTH+i, pacman[frame][i]);

        // advance the animation frame
        frame += deltaFrame;
        if (frame == 0 || frame == MAX_FRAMES-1)
            deltaFrame = -deltaFrame;

        // check if we are completed and set initialise for next time around
        bInit = (idx == MD_MAX72XX::getColumnCount()+DATA_WIDTH);
        isLoading = bInit;

        MD_MAX72XX::control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);

        return isLoading;

    }