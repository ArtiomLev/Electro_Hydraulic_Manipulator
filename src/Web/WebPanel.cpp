#include "WebPanel.h"

#include "Arduino.h"

#include "port.h"
#include "config.h"

#include "Hardware/Motors.h"
#include "PosControl/PositionController.h"
#include "PosControl/Program.h"
#include "PosControl/ProgramRunner.h"

void build(sets::Builder &b);

SettingsGyverWS WebPanel::panel("Манипулятор");

// Добавляем объекты программы и програмраннера
Program program;
ProgramRunner programRunner(&program, &pos_control);

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
        WiFi.softAP(AP_SSID, AP_PASS); {
            for (byte n1 = 0; n1 < 2; n1++) {
                for (byte n2 = 0; n2 < 2; n2++) {
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
    programRunner.update(); // Обновляем состояние програмраннера
}

void build(sets::Builder &b) {
    static enum PanelTabs: uint8_t {
        MANUAL,
        HOMING,
        PROGRAM
    } tab = MANUAL;
    if (b.Tabs("Ручное;Нулевая точка;Программа", &tab)) {
        b.reload();
        return;
    }

    if (b.beginGroup("Координаты")) {
        if (b.beginRow()) {
            b.LabelFloat("База", pos_control.getPosition(1));
            b.LabelFloat("Захват", pos_control.getPosition(5));
            b.endRow();
        }
        if (b.beginRow()) {
            b.LabelFloat("Звено 1", pos_control.getPosition(2));
            b.LabelFloat("Звено 2", pos_control.getPosition(3));
            b.LabelFloat("Звено 3", pos_control.getPosition(4));
            b.endRow();
        }
        b.endGroup();
    }

    switch (tab) {
        case MANUAL:
            if (b.beginGroup("Управление")) {
                static float step = 5;
                b.Slider("Шаг", 0.5, 20, 0.5, "мм", &step);
                if (b.beginRow("База")) {
                    if (b.Button("↺")) {
                        pos_control.axisGoToRel(1, step * -1);
                        b.reload();
                    }
                    if (b.Button("↻")) {
                        pos_control.axisGoToRel(1, step);
                        b.reload();
                    }
                    b.endRow();
                }
                if (b.beginRow("Звено 1")) {
                    if (b.Button("▼")) {
                        pos_control.axisGoToRel(2, step * -1);
                        b.reload();
                    }
                    if (b.Button("▲")) {
                        pos_control.axisGoToRel(2, step);
                        b.reload();
                    }
                    b.endRow();
                }
                if (b.beginRow("Звено 2")) {
                    if (b.Button("▼")) {
                        pos_control.axisGoToRel(3, step * -1);
                        b.reload();
                    }
                    if (b.Button("▲")) {
                        pos_control.axisGoToRel(3, step);
                        b.reload();
                    }
                    b.endRow();
                }
                if (b.beginRow("Звено 3")) {
                    if (b.Button("▼")) {
                        pos_control.axisGoToRel(4, step * -1);
                        b.reload();
                    }
                    if (b.Button("▲")) {
                        pos_control.axisGoToRel(4, step);
                        b.reload();
                    }
                    b.endRow();
                }
                if (b.beginRow("Захват")) {
                    if (b.Button("⊖")) {
                        pos_control.axisGoToRel(5, step * -1);
                        b.reload();
                    }
                    if (b.Button("⊕")) {
                        pos_control.axisGoToRel(5, step);
                        b.reload();
                    }
                    b.endRow();
                }
                if (b.Button("🛑 Стоп всё! 🛑")) {
                    pos_control.brake();
                    b.reload();
                }
                b.endGroup();
            }
            break;
        case HOMING:
            if (b.Button("Сбросить координаты")) {
                pos_control.reset();
                b.reload();
            }
            if (b.Button("Идти в ноль")) {
                for (uint8_t i = 1; i <= 5; i++) {
                    pos_control.axisGoTo(i, 0);
                    b.reload();
                }
            }
            break;
        case PROGRAM:
            // Группа управления файлами программы
            if (b.beginGroup("Файл программы")) {
                // Получаем список файлов с расширением .tbl из LittleFS
                static String fileList = "";
                static String selectedFile = "/program.tbl";

                // Кэшируем список файлов
                static uint32_t file_list_update_tmr = 0;
                if (millis() - file_list_update_tmr > 5000 || fileList.length() == 0) {
                    fileList = WebPanel::panel.fs.flash.listDir();
                    file_list_update_tmr = millis();
                }

                // Фильтруем только .tbl файлы
                String tblFiles = "";
                Text fileListText(fileList);
                size_t count = fileListText.count(';') + 1;
                for (size_t i = 0; i < count; i++) {
                    String file = fileListText.getSub(i, ';').toString();
                    if (file.endsWith(".tbl")) {
                        if (tblFiles.length() > 0) tblFiles += ";";
                        tblFiles += file;
                    }
                }

                // Выбор файла из выпадающего списка
                static uint8_t selectedFileIndex = 0;
                if (b.Select("Выберите файл", tblFiles, &selectedFileIndex)) {
                    selectedFile = Text(tblFiles).getSub(selectedFileIndex, ';').toString();
                    b.reload();
                }

                // Строка с кнопками загрузки и сохранения
                if (b.beginRow()) {
                    if (b.Button("📂 Загрузить")) {
                        if (program.loadFromFile(selectedFile.c_str())) {
                            b.reload();
                        }
                    }
                    if (b.Button("💾 Сохранить")) {
                        if (program.saveToFile(selectedFile.c_str())) {
                            b.reload();
                        }
                    }
                    b.endRow();
                }
                // Кнопка удаления файла
                if (b.Button("🗑️ Удалить файл", sets::Colors::Red)) {
                    // Удаляем файл из файловой системы
                    LittleFS.remove(selectedFile.c_str());
                    // Обновляем список файлов
                    fileList = WebPanel::panel.fs.flash.listDir();
                    // Обновляем список .tbl файлов
                    String updatedTblFiles = "";
                    Text updatedFileListText(fileList);
                    size_t updatedCount = updatedFileListText.count(';') + 1;
                    for (size_t i = 0; i < updatedCount; i++) {
                        String file = updatedFileListText.getSub(i, ';').toString();
                        if (file.endsWith(".tbl")) {
                            if (updatedTblFiles.length() > 0) updatedTblFiles += ";";
                            updatedTblFiles += file;
                        }
                    }
                    // Если есть другие .tbl файлы, выбираем первый, иначе устанавливаем дефолтный
                    if (updatedTblFiles.length() > 0) {
                        selectedFile = Text(updatedTblFiles).getSub(0, ';').toString();
                        selectedFileIndex = 0;
                    } else {
                        selectedFile = "/program.tbl";
                        selectedFileIndex = 0;
                    }
                    b.reload();
                }

                // Создание нового файла
                static char newFileName[32] = "new_program";
                b.Input("Имя файла (без расширения)", AnyPtr(newFileName, 32));
                if (b.Button("➕ Создать файл")) {
                    String fullPath = "/" + String(newFileName) + ".tbl";
                    // Создаем пустую программу и сохраняем
                    program.clear();
                    if (program.saveToFile(fullPath.c_str())) {
                        selectedFile = fullPath;
                        // Обновляем индекс в списке
                        String updatedFileList = WebPanel::panel.fs.flash.listDir();
                        Text updatedListText(updatedFileList);
                        size_t updatedCount = updatedListText.count(';') + 1;
                        for (size_t i = 0; i < updatedCount; i++) {
                            String file = updatedListText.getSub(i, ';').toString();
                            if (file == fullPath) {
                                selectedFileIndex = i;
                                break;
                            }
                        }
                        b.reload();
                    }
                }

                b.endGroup();
            }

            static enum ProgTabs: uint8_t {
                EDIT,
                RUN
            } prog_tabs = RUN;
            if (b.Tabs("Редактирование;Работа", &prog_tabs)) {
                b.reload();
                return;
            }

            switch (prog_tabs) {
                case EDIT: {
                    static uint16_t selectedPoint = 1;
                    static float editPos[5] = {0, 0, 0, 0, 0};

                    // Группа редактирования точки
                    if (b.beginGroup("Редактирование точки")) {
                        // Выбор точки для редактирования
                        if (b.Number("Точка №", &selectedPoint, 1,
                                     program.getPointCount() > 0 ? program.getPointCount() + 1 : 1)) {
                            selectedPoint--;
                        }

                        // Загрузка координат текущей точки или текущего положения
                        if (selectedPoint < program.getPointCount()) {
                            float pos[5];
                            if (program.getPoint(selectedPoint, pos[0], pos[1], pos[2], pos[3], pos[4])) {
                                for (int i = 0; i < 5; i++) editPos[i] = pos[i];
                            }
                        }

                        // Поля редактирования координат
                        b.Number("Позиция 1 (База)", &editPos[0], -200, 200);
                        b.Number("Позиция 2 (Звено 1)", &editPos[1], -200, 200);
                        b.Number("Позиция 3 (Звено 2)", &editPos[2], -200, 200);
                        b.Number("Позиция 4 (Звено 3)", &editPos[3], -200, 200);
                        b.Number("Позиция 5 (Захват)", &editPos[4], -200, 200);

                        // Строка кнопок загрузки
                        if (b.beginRow("Загрузить")) {
                            if (b.Button("Положение")) {
                                // Загружаем текущие координаты манипулятора
                                for (int i = 1; i <= 5; i++) {
                                    editPos[i - 1] = pos_control.getPosition(i);
                                }
                                b.reload();
                            }
                            if (b.Button("Точка")) {
                                // Загружаем координаты выбранной точки
                                if (selectedPoint < program.getPointCount()) {
                                    float pos[5];
                                    if (program.getPoint(selectedPoint, pos[0], pos[1], pos[2], pos[3], pos[4])) {
                                        for (int i = 0; i < 5; i++) editPos[i] = pos[i];
                                    }
                                }
                                b.reload();
                            }
                            b.endRow();
                        }

                        // Кнопка перемещения (для проверки координат)
                        if (b.Button("Переместиться")) {
                            for (int i = 1; i <= 5; i++) {
                                pos_control.axisGoTo(i, editPos[i - 1]);
                            }
                            b.reload();
                        }

                        // Кнопка сохранения изменений
                        if (b.Button("💾 Сохранить точку")) {
                            if (selectedPoint < program.getPointCount()) {
                                program.setPoint(selectedPoint, editPos[0], editPos[1], editPos[2], editPos[3],
                                                 editPos[4]);
                                b.reload();
                            }
                        }

                        b.endGroup();
                    }

                    // Группа управления точками
                    if (b.beginGroup("Управление точками")) {
                        if (b.beginRow()) {
                            if (b.Button("➕ В конец")) {
                                // Добавляем новую точку в конец
                                program.addPoint(editPos[0], editPos[1], editPos[2], editPos[3], editPos[4]);
                                selectedPoint = program.getPointCount() - 1;
                                b.reload();
                            }
                            if (b.Button("➕ После")) {
                                // Вставляем новую точку после выбранной
                                program.insertPoint(selectedPoint + 1, editPos[0], editPos[1], editPos[2], editPos[3],
                                                    editPos[4]);
                                selectedPoint++;
                                b.reload();
                            }
                            b.endRow();
                        }

                        if (b.beginRow()) {
                            if (b.Button("🗑️ Удалить")) {
                                if (selectedPoint < program.getPointCount()) {
                                    program.removePoint(selectedPoint);
                                    if (selectedPoint >= program.getPointCount()) {
                                        selectedPoint = program.getPointCount() > 0 ? program.getPointCount() - 1 : 0;
                                    }
                                    b.reload();
                                }
                            }
                            if (b.Button("🧹 Очистить всё")) {
                                program.clear();
                                selectedPoint = 0;
                                b.reload();
                            }
                            b.endRow();
                        }

                        // Информация о программе
                        b.Label("Всего точек: ", String(program.getPointCount()));

                        b.endGroup();
                    }

                    // Превью программы (только если есть точки)
                    if (program.getPointCount() > 0) {
                        if (b.beginGroup("Предпросмотр программы")) {
                            const String preview = program.toCSV();
                            b.TableCSV(0, preview, "№;База;Звено 1;Звено 2;Звено 3;Захват");
                            b.endGroup();
                        }
                    }
                    break;
                }
                case RUN: {
                    static float runSpeed = 1000;
                    static uint32_t pointDelay = 500;
                    static uint16_t startPoint = 0;

                    // Группа информации о состоянии
                    if (b.beginGroup("Состояние")) {
                        String stateStr;
                        switch (programRunner.getState()) {
                            case ProgramRunner::STOPPED: stateStr = "⏹️ Остановлен";
                                break;
                            case ProgramRunner::RUNNING: stateStr = "▶️ Выполняется";
                                break;
                            case ProgramRunner::PAUSED: stateStr = "⏸️ На паузе";
                                break;
                            case ProgramRunner::BRAKING: stateStr = "🛑 Торможение";
                                break;
                        }

                        b.Label("Статус: ", stateStr);
                        b.Label("Текущая точка: ",
                                String(programRunner.getCurrentPoint() + 1) + " / " +
                                String(programRunner.getTotalPoints()));
                        b.Label("Система простаивает: ", pos_control.systemIdle() ? "✅ Да" : "❌ Нет");
                        b.endGroup();
                    }

                    // Группа настроек выполнения
                    if (b.beginGroup("Настройки выполнения")) {
                        b.Number("Скорость (мм/с)", &runSpeed, 100, 5000);
                        b.Number("Задержка между точками (мс)", &pointDelay, 0, 5000);
                        b.Number("Начать с точки №", &startPoint, 0,
                                 program.getPointCount() > 0 ? program.getPointCount() - 1 : 0);

                        programRunner.setSpeed(runSpeed);
                        programRunner.setPointDelay(pointDelay);
                        b.endGroup();
                    }

                    // Группа управления выполнением
                    if (b.beginGroup("Управление")) {
                        if (b.beginRow()) {
                            if (b.Button("▶️ Старт")) {
                                // Устанавливаем начальную точку
                                // Нужно добавить метод для установки начальной точки в ProgramRunner
                                programRunner.start();
                                b.reload();
                            }
                            if (b.Button("⏸️ Пауза")) {
                                programRunner.pause();
                                b.reload();
                            }
                            b.endRow();
                        }

                        if (b.beginRow()) {
                            if (b.Button("⏹️ Стоп")) {
                                programRunner.stop();
                                b.reload();
                            }
                            b.endRow();
                        }
                        b.endGroup();
                    }

                    // Превью программы (только если есть точки)
                    if (program.getPointCount() > 0) {
                        if (b.beginGroup("Программа")) {
                            const String preview = program.toCSV();
                            b.TableCSV(0, preview, "№;База;Звено 1;Звено 2;Звено 3;Захват");
                            b.endGroup();
                        }
                    }
                    break;
                }
            }
            break;
    }
}
