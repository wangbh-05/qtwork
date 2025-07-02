//
// Created by Gemini on 2025/06/27.
//

#ifndef QT_PROGRAMMING_2024_MEDKIT_H
#define QT_PROGRAMMING_2024_MEDKIT_H

#include "Consumable.h"

class Medkit : public Consumable {
public:
    explicit Medkit(QGraphicsItem *parent = nullptr);
    void applyEffect(Character *character) override;
};


#endif //QT_PROGRAMMING_2024_MEDKIT_H
