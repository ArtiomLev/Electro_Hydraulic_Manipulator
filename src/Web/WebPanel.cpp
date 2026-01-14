#include "WebPanel.h"

#include "port.h"
#include "config.h"

void build(sets::Builder &b);

SettingsGyver WebPanel::panel("Манипулятор");

void WebPanel::WiFiBegin() {
    static bool wifi_setup = false;
    if (!wifi_setup) {
#if WIFI_MODE == 1
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);
        unsigned long wifi_blink_tmr = millis();
        bool blink_state = false;
        unsigned long progress_tmr = millis();
        Serial.println("Connecting to WiFi:");
        Serial.print("<");
        while (WiFi.status() != WL_CONNECTED) {
            if (millis() - wifi_blink_tmr > 100) {
                wifi_blink_tmr = millis();
                digitalWrite(BUILTIN_LED, blink_state);
                blink_state = !blink_state;
            }
            if (millis() - progress_tmr > 500) {
                progress_tmr = millis();
                Serial.print("-");
            }
        }
        Serial.println(">");
        Serial.print("Connected to WIFI_SSID: ");
        Serial.println(WiFi.localIP());
        wifi_setup = true;
#elif WIFI_MODE == 2
        WiFi.mode(WIFI_AP);
        WiFi.softAP(AP_SSID, AP_PASS);
        {
            for (byte n1 = 0 ; n1 < 2 ; n1++) {
                for (byte n2 = 0 ; n2 < 2 ; n2++) {
                    digitalWrite(BUILTIN_LED, HIGH);
                    delay(100);
                    digitalWrite(BUILTIN_LED, LOW);
                    delay(100);
                }
                delay(500);
            }
        }
        Serial.print("Access Point started. SSID: ");
        Serial.println(AP_SSID);
        Serial.print("IP address: ");
        Serial.println(WiFi.softAPIP());
        wifi_setup = true;
#else
#pragma error "WIFI_MODE invalid! Use 1 (STA) or 2 (AP)"
#endif
    }
}

void WebPanel::SetupPanel() {
    WebPanel::WiFiBegin();
    panel.begin();
    panel.onBuild(build);
}

void WebPanel::PanelTick() {
    panel.tick();
}

void build(sets::Builder &b) {
}
