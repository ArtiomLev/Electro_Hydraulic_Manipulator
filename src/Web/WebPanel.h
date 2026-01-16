#pragma once
#define SETT_NO_DB
#include <SettingsGyverWS.h>

namespace WebPanel {
    extern SettingsGyverWS panel;

    void WiFiBegin();

    void SetupPanel();

    void PanelTick();
}