#include "ProgramRunner.h"

ProgramRunner::ProgramRunner(Program* program, PositionController* controller)
    : program(program), controller(controller) {
}

void ProgramRunner::start() {
    if (state != STOPPED || program->getPointCount() == 0) return;
    
    currentPoint = 0;
    state = RUNNING;
    lastUpdateTime = millis();
    
    // Начинаем с первой точки
    moveToNextPoint();
}

void ProgramRunner::pause() {
    if (state == RUNNING) {
        state = PAUSED;
        controller->brake();
    }
}

void ProgramRunner::resume() {
    if (state == PAUSED) {
        state = RUNNING;
        lastUpdateTime = millis();
    }
}

void ProgramRunner::stop() {
    if (state != STOPPED) {
        state = STOPPED;
        controller->brake();
        currentPoint = 0;
    }
}

void ProgramRunner::brake() {
    if (state == RUNNING || state == PAUSED) {
        state = BRAKING;
        controller->brake();
    }
}

void ProgramRunner::setSpeed(const float _speed) {
    this->speed = _speed;
    // Здесь можно обновить скорость моторов если нужно
}

void ProgramRunner::setPointDelay(uint32_t delay) {
    this->pointDelay = delay;
}

ProgramRunner::State ProgramRunner::getState() const {
    return state;
}

uint16_t ProgramRunner::getCurrentPoint() const {
    return currentPoint;
}

uint16_t ProgramRunner::getTotalPoints() const {
    return program->getPointCount();
}

void ProgramRunner::update() {
    if (state != RUNNING) return;
    
    unsigned long currentTime = millis();
    
    // Если система простаивает и прошло достаточно времени с предыдущей точки
    if (controller->systemIdle() && (currentTime - lastUpdateTime >= pointDelay)) {
        currentPoint++;
        
        if (currentPoint >= program->getPointCount()) {
            // Программа завершена
            state = STOPPED;
            currentPoint = 0;
        } else {
            // Переходим к следующей точке
            if (!moveToNextPoint()) {
                // Ошибка при переходе
                state = STOPPED;
                currentPoint = 0;
            }
            lastUpdateTime = currentTime;
        }
    }
}

bool ProgramRunner::moveToNextPoint() const {
    if (currentPoint >= program->getPointCount()) return false;
    
    float pos1, pos2, pos3, pos4, pos5;
    if (!program->getPoint(currentPoint, pos1, pos2, pos3, pos4, pos5)) {
        return false;
    }
    
    // Устанавливаем позиции для всех осей
    controller->axisGoTo(1, pos1);
    controller->axisGoTo(2, pos2);
    controller->axisGoTo(3, pos3);
    controller->axisGoTo(4, pos4);
    controller->axisGoTo(5, pos5);
    
    return true;
}

void ProgramRunner::applyBrake() const {
    controller->brake();
}
