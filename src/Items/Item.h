//
// Created by gerw on 8/20/24.
//

#ifndef QT_PROGRAMMING_2024_ITEM_H
#define QT_PROGRAMMING_2024_ITEM_H

#include <QGraphicsObject> // 核心修正：使用 QGraphicsObject
#include <QPainter>
#include <QGraphicsPixmapItem>
#include <QPointF> // 新增：包含 QPointF

class Item : public QGraphicsObject { // 核心修正：继承 QGraphicsObject
public:
    explicit Item(QGraphicsItem *parent, const QString &pixmapPath);

    [[nodiscard]] QRectF boundingRect() const override; // Item 提供 boundingRect 实现
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override; // Item 提供 paint 实现

    [[nodiscard]] QGraphicsPixmapItem* getPixmapItem() const { return pixmapItem; }

    // 【新增】为所有 Item 添加速度属性和方法
    [[nodiscard]] const QPointF &getVelocity() const;
    void setVelocity(const QPointF &velocity);

protected:
    QGraphicsPixmapItem *pixmapItem{};
    QPointF velocity{}; // 【新增】所有 Item 都将拥有速度属性
};


#endif //QT_PROGRAMMING_2024_ITEM_H
