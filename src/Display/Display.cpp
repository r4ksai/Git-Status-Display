#include "Display.h"

    RawDisplay::RawDisplay() : MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES) {
    };

    void RawDisplay::begin() {
        DEBUG_PRINTLN("D: Initializing Display");
        MD_MAX72XX::begin();
        DEBUG_PRINT("D: Set Intensity to ");
        DEBUG_PRINT(MAX_INTENSITY/DIMNESS);
        DEBUG_PRINT("/");
        DEBUG_PRINTLN(MAX_INTENSITY);
        control(INTENSITY, MAX_INTENSITY/DIMNESS);
        control(UPDATE, ON);
        clear();

        // Spashscreen will "ALL ON"
        DEBUG_PRINTLN("D: Turn all LEDs ON");
        for (uint8_t i=0; i<32; i++) setColumn(i, 0xFF);

        blink = false;
    }

    void RawDisplay::gitStatus(byte status[]) {

        // Exit if the next frame time has not reached
        if (millis()-prevTimeAnim < BLINK_DELAY)
            return;

        DEBUG_PRINTLN("D: Update Git Status Display");
        prevTimeAnim = millis();      // starting point for next time

        control(INTENSITY, MAX_INTENSITY/DIMNESS);
        control(UPDATE, OFF);
        clear();

        for (uint8_t i=0; i<32; i++)
        {
            byte dataColumn = status[i];
            if (i % 2 == 0)
                dataColumn = reverseByte(dataColumn);

            setColumn(i, dataColumn);
        }

        // Check if the latest git dot is ON/OFF and blink the last dot in the display
        if (!(status[0] & 1))
        {
            setPoint(CURRENT_ROW, CURRENT_COL, blink);
            blink = !blink;
        }

        control(UPDATE, ON);
    }

    // Show pacman animation
    bool RawDisplay::loading() {
        static boolean bInit = true;  // initialise the animation
        isLoading = false;

        // Is it time to animate?
        if (millis()-prevTimeAnim < ANIMATION_DELAY)
            return isLoading;

        DEBUG_PRINTLN("D: Show Loading animation");

        prevTimeAnim = millis();      // starting point for next time

        control(UPDATE, OFF);

        // Initialize
        if (bInit)
        {
            clear();
            idx = 0;
            frame = 0;
            deltaFrame = 1;
            bInit = false;

            // Lay out the dots
            for (uint8_t i=0; i<MAX_DEVICES; i++)
            {
            setPoint(3, (i*COL_SIZE) + 3, true);
            setPoint(4, (i*COL_SIZE) + 3, true);
            setPoint(3, (i*COL_SIZE) + 4, true);
            setPoint(4, (i*COL_SIZE) + 4, true);
            }
        }

        // clear old graphic
        for (uint8_t i=0; i<DATA_WIDTH; i++)
            setColumn(idx-DATA_WIDTH+i, 0);
        // move reference column and draw new graphic
        idx++;
        for (uint8_t i=0; i<DATA_WIDTH; i++)
            setColumn(idx-DATA_WIDTH+i, pacman[frame][i]);

        // advance the animation frame
        frame += deltaFrame;
        if (frame == 0 || frame == MAX_FRAMES-1)
            deltaFrame = -deltaFrame;

        // check if we are completed and set initialise for next time around
        bInit = (idx == getColumnCount()+DATA_WIDTH);
        isLoading = bInit;

        control(UPDATE, ON);

        return isLoading;

    }

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
