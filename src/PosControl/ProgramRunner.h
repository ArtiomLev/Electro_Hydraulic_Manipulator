#pragma once
#include <Arduino.h>
#include "Program.h"
#include "PositionController.h"

class ProgramRunner {
public:
    enum State {
        STOPPED,
        RUNNING,
        PAUSED,
        BRAKING
    };
    
    ProgramRunner(Program* program, PositionController* controller);
    
    // Запустить исполнение программы
    void start();

    // Приостановить исполнение программы
    void pause();

    // Возобновить исполнение программы
    void resume();

    // Остановить исполнение программы
    void stop();

    // Прервать исполнение программы
    void brake();
    
    // Задать скорость движения
    void setSpeed(float _speed);

    // Задать задержку после исполнения программы
    void setPointDelay(uint32_t delay);
    
    // Получить состояние
    State getState() const;

    // Получить текущую точку
    uint16_t getCurrentPoint() const;

    // Получить количество точек программы
    uint16_t getTotalPoints() const;
    
    // Тикер
    void update();
    
private:
    // Указатель на программы
    Program* program;

    // Указатель на контроллер движения
    PositionController* controller;
    
    State state = STOPPED; // Переменная состояния
    uint16_t currentPoint = 0; // Текущая точка
    uint32_t lastUpdateTime = 0; // Таймер обновления
    uint32_t pointDelay = 500; // Задержка между точками
    float speed = 1000; // Скорость

    // Двигаться к следующей точке
    bool moveToNextPoint() const;

    void applyBrake() const;
};