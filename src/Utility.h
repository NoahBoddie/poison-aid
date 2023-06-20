#pragma once

#include "SettingManager.h"

namespace POS
{
    void SendPrompt(const char* prompt, const char* sound = nullptr)
    {
        if (SettingManager::promptType == 1) {
            //May do play sound here
            RE::DebugMessageBox(prompt);
        }
        else {
            RE::DebugNotification(prompt, sound);
        }
    }
}