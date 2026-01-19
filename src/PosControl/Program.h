#pragma once
#include <Arduino.h>
#include <Table.h>

class Program {
private:
    Table table;

    // Вспомогательный метод для перестроения номеров точек
    void renumber();

public:
    Program();

    // Создание программы с заданным количеством точек
    void create(uint16_t points);

    // Сохранение в файл
    bool saveToFile(const char* path);

    // Загрузка из файла
    bool loadFromFile(const char* path);

    // Получение количества точек
    uint16_t getPointCount();

    // Задание точки
    bool setPoint(uint16_t index, float pos1, float pos2, float pos3, float pos4, float pos5);
    // Получение точки
    bool getPoint(uint16_t index, float &pos1, float &pos2, float &pos3, float &pos4, float &pos5);

    // Вставка точки
    bool insertPoint(uint16_t index, float pos1, float pos2, float pos3, float pos4, float pos5);
    // Удаление точки
    bool removePoint(uint16_t index);

    // Добавление точки в конец
    bool addPoint(float pos1, float pos2, float pos3, float pos4, float pos5);

    // Очистка программы
    void clear();

    // Экспорт в CSV строку для отображения
    String toCSV();
};