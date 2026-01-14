#pragma once
#define SETT_NO_DB
#include <SettingsGyver.h>

namespace WebPanel {
    extern SettingsGyver panel;

    void WiFiBegin();

    void SetupPanel();

    void PanelTick();
}