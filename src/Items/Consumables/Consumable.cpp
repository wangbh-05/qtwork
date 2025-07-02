//
// Created by Gemini on 2025/06/27.
//

#include "Consumable.h"

Consumable::Consumable(QGraphicsItem *parent, const QString &pixmapPath) : Item(parent, pixmapPath) {
    // 消耗品默认不应被父项管理，除非被拾取后有特殊挂载需求
    setParentItem(nullptr);
}
