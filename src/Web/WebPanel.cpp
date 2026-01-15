#include "WebPanel.h"

#include "port.h"
#include "config.h"

#include "Hardware/Motors.h"

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
    static enum PanelTabs: uint8_t {
        MANUAL,
        HOMING,
        AUTO
    } tab;
    if (b.Tabs("Ручное;Нулевая точка;Автоматически"), &tab) {
        b.reload();
        // return;
    }

    switch (tab) {
        case MANUAL:
            if (b.beginRow("Координаты")) {
                b.Label(String(Motors::steppers[0].pos));
                b.Label(String(Motors::steppers[1].pos));
                b.Label(String(Motors::steppers[2].pos));
                b.Label(String(Motors::steppers[3].pos));
                b.Label(String(Motors::steppers[4].pos));
                b.endRow();
            }
            static int32_t motor_speed = STEPPER_DEFAULT_SPEED;
            if (b.beginGroup("Параметры движения")) {
                b.Slider("Скорость", 0, MAX_PLANNER_SPEED, 10, "Шагов" , &motor_speed);
                b.endGroup();
            }
            if (b.beginGroup("Управление")) {
                if (b.beginRow("База")) {
                    if (b.Button("◀")) {
                        Motors::steppers[0].setSpeed(motor_speed);
                    }
                    if (b.Button("⏹")) {
                        Motors::steppers[0].stop();
                    }
                    if (b.Button("▶")) {
                        Motors::steppers[0].setSpeed(motor_speed * -1);
                    }
                    b.endRow();
                }
                if (b.beginRow("Звено 1")) {
                    if (b.Button("вниз")) {

                        Motors::steppers[1].setSpeed(motor_speed);
                    }
                    if (b.Button("⏹")) {
                        Motors::steppers[1].stop();
                    }
                    if (b.Button("вверх")) {
                        Motors::steppers[1].setSpeed(motor_speed * -1);
                    }
                    b.endRow();
                }
                if (b.beginRow("Звено 2")) {
                    if (b.Button("вниз")) {
                        Motors::steppers[2].setSpeed(motor_speed * -1);
                    }
                    if (b.Button("⏹")) {
                        Motors::steppers[2].stop();
                    }
                    if (b.Button("вверх")) {
                        Motors::steppers[2].setSpeed(motor_speed);
                    }
                    b.endRow();
                }
                if (b.beginRow("Звено 3")) {
                    if (b.Button("вниз")) {
                        Motors::steppers[3].setSpeed(motor_speed * -1);
                    }
                    if (b.Button("⏹")) {
                        Motors::steppers[3].stop();
                    }
                    if (b.Button("вверх")) {
                        Motors::steppers[3].setSpeed(motor_speed);
                    }
                    b.endRow();
                }
                if (b.beginRow("Держатель")) {
                    if (b.Button("-")) {
                        Motors::steppers[4].setSpeed(motor_speed);
                    }
                    if (b.Button("⏹")) {
                        Motors::steppers[4].stop();
                    }
                    if (b.Button("+")) {
                        Motors::steppers[4].setSpeed(motor_speed * -1);
                    }
                    b.endRow();
                }
                if (b.Button("Стоп всё!")) {
                    for (auto stepper: Motors::steppers) {
                        stepper.stop();
                    }
                }
                b.endGroup();
            }
            break;
        case HOMING:
            break;
        case AUTO:
            break;
    }
}
