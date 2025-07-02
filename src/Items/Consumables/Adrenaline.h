//
// Created by Gemini on 2025/06/27.
//

#ifndef QT_PROGRAMMING_2024_ADRENALINE_H
#define QT_PROGRAMMING_2024_ADRENALINE_H

#include "Consumable.h"

class Adrenaline : public Consumable {
public:
    explicit Adrenaline(QGraphicsItem *parent = nullptr);
    void applyEffect(Character *character) override;

private:
    qreal speedBoostMultiplier;
    int healPerSecond;
    qint64 durationMs;
};


#endif //QT_PROGRAMMING_2024_ADRENALINE_H
