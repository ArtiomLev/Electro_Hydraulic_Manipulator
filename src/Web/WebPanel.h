#pragma once
#include <SettingsGyver.h>

namespace WebPanel {
    extern SettingsGyver panel;

    void WiFiBegin();

    void SetupPanel();

    void PanelTick();
}