//
// Created by gerw on 8/20/24.
//

#include <QDateTime>
#include <QDebug> // 确保包含 QDebug
#include "Scene.h"

Scene::Scene(QObject *parent) : QGraphicsScene(parent), timer(new QTimer(this)) {
    connect(timer, &QTimer::timeout, this, &Scene::update);
}

void Scene::update() {
    auto currentTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
    if (lastTime == -1) { // first frame
        deltaTime = 0;
    } else {
        deltaTime = currentTime - lastTime;
    }
    lastTime = currentTime;

    // 【诊断输出】打印 deltaTime
    //qDebug() << "SCENE_DEBUG: DeltaTime =" << deltaTime << "ms";

    processInput();
    processMovement();
    processPicking();
}

void Scene::startLoop() {
    timer->start(1000 / 90); // 90FPS
}

void Scene::processInput() {

}

void Scene::processMovement() {

}

void Scene::processPicking() {

}
