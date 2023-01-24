#pragma once
#include "MD_MAX72xx.h"
    
class RawDisplay : MD_MAX72XX {
    public:
        RawDisplay();
        bool loading();
        void begin();
        void gitStatus(int x, int y, byte status[]);
};