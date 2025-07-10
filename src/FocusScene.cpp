// Copyright (c) 2023 - Barton Dring
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include <string>
#include "Scene.h"

class FocusScene : public Scene {
private:
    int powerLevel = 1;  // 1-5 percent
    bool laserOn = false;
    const char* stateError = nullptr;

public:
    FocusScene() : Scene("Focus", 4) {}

    void onDialButtonPress() { pop_scene(); }

    void onRedButtonPress() {
        if (state != Idle) {
            stateError = "Machine must be IDLE";
            reDisplay();
            return;
        }
        laserOn = false;
        send_line("M5"); // Laser OFF
        reDisplay();
    }

    void onGreenButtonPress() {
        if (state != Idle) {
            stateError = "Machine must be IDLE";
            reDisplay();
            return;
        }
        laserOn = true;
        int sValue = powerLevel; // 1-5 percent, map to S1-S5 (or scale as needed)
        send_linef("M3 S%d", sValue);
        reDisplay();
    }

    void onStateChange(state_t old_state) { 
        reDisplay(); 
    }

    void onTouchClick() {
        // No specific touch handling needed for this scene
    }

    void onEncoder(int delta) {
        static int encoderAccum = 0;
        if (delta != 0 && !laserOn) {
            encoderAccum += delta;
            // 20 pulses per step (5 steps per rev)
            while (encoderAccum >= 20) {
                powerLevel++;
                encoderAccum -= 20;
            }
            while (encoderAccum <= -20) {
                powerLevel--;
                encoderAccum += 20;
            }
            if (powerLevel < 1) powerLevel = 1;
            if (powerLevel > 5) powerLevel = 5;
            reDisplay();
        }
    }

    void onEntry(void* arg) override {
        if (state != Idle) {
            stateError = "Machine must be IDLE";
        } else {
            stateError = nullptr;
        }
        laserOn = false;
        reDisplay();
    }

    void reDisplay() {
        background();
        drawMenuTitle(current_scene->name());

        // Draw power level indicator
        static char buffer[20];
        sprintf(buffer, "Power: %d%%", powerLevel);
        centered_text(buffer, display.height() / 2, laserOn ? TFT_RED : TFT_WHITE, MEDIUM);

        const char* grnLabel = "Enable";
        const char* redLabel = "Disable";

        drawButtonLegends(redLabel, grnLabel, "Back");
        
        if (stateError) {
            static char errbuf[40];
            snprintf(errbuf, sizeof(errbuf), "%s", stateError);
            centered_text(errbuf, display.height() * 3 / 4, TFT_RED, TINY);
            stateError = nullptr;
        }
        
        refreshDisplay();
    }
};
FocusScene focusScene;
