#include "Display.h"

    // Reverse the row order of the byte so that git status dots have a continuous flow
    byte RawDisplay::reverseByte(byte x) {
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

    RawDisplay::RawDisplay() : MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES) {
    };

    void RawDisplay::begin() {
        DEBUG_PRINTLN("D: Initializing Display");
        MD_MAX72XX::begin();
        MD_MAX72XX::control(MD_MAX72XX::INTENSITY, MAX_INTENSITY/15);
        MD_MAX72XX::control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
        MD_MAX72XX::clear();

        // Turn all led's on
        for (uint8_t i=0; i<32; i++) MD_MAX72XX::setColumn(i, 0b11111111);

        blink = false;
    }

    void RawDisplay::gitStatus(int x, int y, byte status[]) {

        // Exit if the next frame time has not reached
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

        // Check if the latest git dot is ON/OFF and blink the last dot in the display
        if (!(status[0] & 1))
        {
            MD_MAX72XX::setPoint(x, y, blink);
            blink = !blink;
        }

        MD_MAX72XX::control(MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
    }

    // Show pacman animation
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