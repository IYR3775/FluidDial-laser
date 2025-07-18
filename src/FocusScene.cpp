// Copyright (c) 2023 - Barton Dring
// Use of this source code is governed by a GPLv3 license that can be found in the LICENSE file.

#include <string>
#include "Scene.h"

extern Scene fileSelectScene;

class FocusScene : public Scene {
private:
    int         powerLevel = 1;  // 1-5 percent
    bool        laserOn    = false;
    const char* stateError = nullptr;

public:
    FocusScene() : Scene("Focus", 60) {}

    void onDialButtonPress() override { pop_scene(); }
    void onRightFlick() override { activate_scene(&fileSelectScene); }
    void onLeftFlick() override { 
        if (state == Idle) {
            laserOn = false;
            send_line("M5 S0");  // Laser OFF
            pop_scene();
        }
    }

    void onRedButtonPress() override {
        if (state != Idle) {
            stateError = "Machine must be IDLE";
            reDisplay();
            return;
        }
        laserOn = false;
        send_line("M5 S0");  // Laser OFF
        reDisplay();
    }

    void onGreenButtonPress() override {
        if (state != Idle) {
            stateError = "Machine must be IDLE";
            reDisplay();
            return;
        }
        laserOn    = true;
        int sValue = powerLevel * 10;  // 1-5 percent, map to S10-S50
        send_line("G1 F1");
        send_linef("M3 S%d", sValue);
        reDisplay();
    }
    void onEncoder(int delta) {
        if (delta != 0 && !laserOn) {
            powerLevel += delta;
            if (powerLevel < 1)
                powerLevel = 1;
            if (powerLevel > 5)
                powerLevel = 5;
            reDisplay();
        }
    }
    void onStateChange(state_t old_state) { 
        if (state == Idle) {
            laserOn = false;
            send_line("M5 S0");  // Laser OFF
            pop_scene();
        }
    }

    void onTouchClick() {
        // No specific touch handling needed for this scene
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
        static char buffer[32];
        if (laserOn) {
            sprintf(buffer, "Power: %d%%", powerLevel);
            centered_text(buffer, display.height() * 1 / 4, TFT_RED, MEDIUM);
            drawRect(10, 110, 220, 40, 20, TFT_RED);
            sprintf(buffer, "LASER ON");
            centered_text(buffer, (display.height() * 1 / 4) + 52, TFT_WHITE, SMALL);
        } else {
            sprintf(buffer, "Power: %d%%", powerLevel);
            centered_text(buffer, display.height() * 1 / 4, TFT_WHITE, MEDIUM);
        }

        const char* grnLabel = "ON";
        const char* redLabel = "OFF";

        drawButtonLegends(redLabel, grnLabel, "Back");

        if (stateError) {
            static char errbuf[40];
            snprintf(errbuf, sizeof(errbuf), "%s", stateError);
            centered_text(errbuf, (display.height() * 1 / 4) + 110, TFT_RED, TINY);
            stateError = nullptr;
        }

        refreshDisplay();
    }
};
FocusScene focusScene;
