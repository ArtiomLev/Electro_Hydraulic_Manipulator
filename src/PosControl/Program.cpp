#include "Program.h"
#include <LittleFS.h>

Program::Program() {
    table.init(6,
               cell_t::Uint16, // Номер точки
               cell_t::Float, // База
               cell_t::Float, // Звено 1
               cell_t::Float, // Звено 2
               cell_t::Float, // Звено 3
               cell_t::Float // Захват
    );
}

void Program::create(const uint16_t points) {
    table.removeAll();
    for (uint16_t i = 0; i < points; i++) {
        table.append(i + 1, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
    }
}

bool Program::saveToFile(const char *path) {
    File file = LittleFS.open(path, "w");
    if (!file) return false;

    // Сохраняем как бинарную таблицу
    const bool result = table.writeTo(file);
    file.close();
    return result;
}

bool Program::loadFromFile(const char *path) {
    File file = LittleFS.open(path, "r");
    if (!file) return false;

    // Читаем бинарную таблицу
    const bool result = table.readFrom(file, file.size());
    file.close();
    return result;
}

uint16_t Program::getPointCount(){
    return table.rows();
}

bool Program::setPoint(const uint16_t index, const float pos1, const float pos2, const float pos3, const float pos4,
                       const float pos5) {
    if (index >= table.rows()) return false;

    table[index][0] = static_cast<uint16_t>(index + 1);
    table[index][1] = pos1;
    table[index][2] = pos2;
    table[index][3] = pos3;
    table[index][4] = pos4;
    table[index][5] = pos5;

    return true;
}

bool Program::getPoint(const uint16_t index, float &pos1, float &pos2, float &pos3, float &pos4, float &pos5){
    if (index >= table.rows()) return false;

    pos1 = table[index][1];
    pos2 = table[index][2];
    pos3 = table[index][3];
    pos4 = table[index][4];
    pos5 = table[index][5];

    return true;
}

bool Program::insertPoint(uint16_t index, float pos1, float pos2, float pos3, float pos4, float pos5) {
    // Создаем новую таблицу с увеличенным размером
    Table newTable;
    newTable.init(6, cell_t::Uint16, cell_t::Float, cell_t::Float, cell_t::Float, cell_t::Float, cell_t::Float);

    // Копируем строки до индекса
    for (uint16_t i = 0; i < index; i++) {
        float p1, p2, p3, p4, p5;
        getPoint(i, p1, p2, p3, p4, p5);
        newTable.append(i + 1, p1, p2, p3, p4, p5);
    }

    // Вставляем новую точку
    newTable.append(index + 1, pos1, pos2, pos3, pos4, pos5);

    // Копируем оставшиеся строки с обновленными номерами
    for (uint16_t i = index; i < table.rows(); i++) {
        float p1, p2, p3, p4, p5;
        getPoint(i, p1, p2, p3, p4, p5);
        newTable.append(i + 2, p1, p2, p3, p4, p5);
    }

    // Заменяем таблицу
    table = newTable;
    return true;
}

bool Program::removePoint(uint16_t index) {
    if (index >= table.rows()) return false;

    // Создаем новую таблицу с уменьшенным размером
    Table newTable;
    newTable.init(6, cell_t::Uint16, cell_t::Float, cell_t::Float, cell_t::Float, cell_t::Float, cell_t::Float);

    // Копируем все строки кроме удаляемой
    for (uint16_t i = 0; i < table.rows(); i++) {
        if (i == index) continue;

        float p1, p2, p3, p4, p5;
        getPoint(i, p1, p2, p3, p4, p5);

        // Перенумеровываем точки
        uint16_t newIndex = i > index ? i : i + 1;
        newTable.append(newIndex + 1, p1, p2, p3, p4, p5);
    }

    // Заменяем таблицу
    table = newTable;
    return true;
}

bool Program::addPoint(float pos1, float pos2, float pos3, float pos4, float pos5) {
    uint16_t newIndex = table.rows();
    return table.append(newIndex + 1, pos1, pos2, pos3, pos4, pos5);
}

void Program::clear() {
    table.removeAll();
}

String Program::toCSV() {
    return table.toCSV(';', 2);
}
