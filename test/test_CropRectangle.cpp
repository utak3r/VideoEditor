#include <gtest/gtest.h>
#include "../src/CropRectangle.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QApplication>

class TestableCropRectangle : public CropRectangle {
public:
    using CropRectangle::CropRectangle;
    using CropRectangle::mousePressEvent;
    using CropRectangle::mouseMoveEvent;
    using CropRectangle::mouseReleaseEvent;
};

class CropRectangleTest : public ::testing::Test {
protected:
    TestableCropRectangle* cropRect;
    QGraphicsScene* scene;

    void SetUp() override {
        scene = new QGraphicsScene();
        cropRect = new TestableCropRectangle();
        scene->addItem(cropRect);
    }

    void TearDown() override {
        delete scene; // This will also delete cropRect
    }

    QGraphicsSceneMouseEvent* createMouseEvent(QEvent::Type type, const QPointF& pos, const QPointF& lastPos) {
        QGraphicsSceneMouseEvent* event = new QGraphicsSceneMouseEvent(type);
        event->setScenePos(pos);
        event->setLastScenePos(lastPos);
        event->setPos(pos);
        event->setButton(Qt::LeftButton);
        event->setButtons(Qt::LeftButton);
        return event;
    }
};

TEST_F(CropRectangleTest, DefaultInitialization) {
    EXPECT_EQ(cropRect->color(), Qt::blue);
    EXPECT_EQ(cropRect->opacity(), 50);
    EXPECT_EQ(cropRect->borderWidth(), 2);
    EXPECT_EQ(cropRect->rect(), QRectF(0, 0, 0, 0));
    EXPECT_TRUE(cropRect->isVisible());
}

TEST_F(CropRectangleTest, InitializationWithRect) {
    TestableCropRectangle rect(QRectF(10, 10, 100, 100));
    EXPECT_EQ(rect.rect(), QRectF(10, 10, 100, 100));
}

TEST_F(CropRectangleTest, SettersAndGetters) {
    cropRect->setColor(Qt::red);
    EXPECT_EQ(cropRect->color(), Qt::red);

    cropRect->setOpacity(100);
    EXPECT_EQ(cropRect->opacity(), 100);

    cropRect->setBorderWidth(5);
    EXPECT_EQ(cropRect->borderWidth(), 5);
}

TEST_F(CropRectangleTest, SetEnabled) {
    cropRect->setEnabled(true);
    EXPECT_TRUE(cropRect->isVisible());
    cropRect->setEnabled(false);
    EXPECT_FALSE(cropRect->isVisible());
}

TEST_F(CropRectangleTest, SetBoundingBordersInvalid) {
    cropRect->setBoundingBorders(QRectF());
    EXPECT_EQ(cropRect->rect(), QRectF(0, 0, 0, 0));
}

TEST_F(CropRectangleTest, TranslateWithinBorders) {
    cropRect->setBoundingBorders(QRectF(0, 0, 200, 200));
    cropRect->setRect(QRectF(50, 50, 50, 50));
    cropRect->setEnabled(true);

    QGraphicsSceneMouseEvent* pressEvent = createMouseEvent(QEvent::GraphicsSceneMousePress, QPointF(75, 75), QPointF(75, 75));
    cropRect->mousePressEvent(pressEvent);

    QGraphicsSceneMouseEvent* moveEvent = createMouseEvent(QEvent::GraphicsSceneMouseMove, QPointF(85, 85), QPointF(75, 75));
    cropRect->mouseMoveEvent(moveEvent);

    EXPECT_EQ(cropRect->rect(), QRectF(60, 60, 50, 50));

    delete pressEvent;
    delete moveEvent;
}

TEST_F(CropRectangleTest, TranslateOutsideBorders) {
    cropRect->setBoundingBorders(QRectF(0, 0, 200, 200));
    cropRect->setRect(QRectF(150, 150, 50, 50));
    cropRect->setEnabled(true);

    QGraphicsSceneMouseEvent* pressEvent = createMouseEvent(QEvent::GraphicsSceneMousePress, QPointF(175, 175), QPointF(175, 175));
    cropRect->mousePressEvent(pressEvent);

    QGraphicsSceneMouseEvent* moveEvent = createMouseEvent(QEvent::GraphicsSceneMouseMove, QPointF(185, 185), QPointF(175, 175));
    cropRect->mouseMoveEvent(moveEvent);

    EXPECT_EQ(cropRect->rect(), QRectF(150, 150, 50, 50));

    delete pressEvent;
    delete moveEvent;
}

TEST_F(CropRectangleTest, ResizeTopLeftWithinBorders) {
    cropRect->setBoundingBorders(QRectF(0, 0, 200, 200));
    cropRect->setRect(QRectF(50, 50, 100, 100));
    cropRect->setEnabled(true);

    QGraphicsSceneMouseEvent* pressEvent = createMouseEvent(QEvent::GraphicsSceneMousePress, QPointF(55, 55), QPointF(55, 55));
    cropRect->mousePressEvent(pressEvent);

    QGraphicsSceneMouseEvent* moveEvent = createMouseEvent(QEvent::GraphicsSceneMouseMove, QPointF(45, 45), QPointF(55, 55));
    cropRect->mouseMoveEvent(moveEvent);

    EXPECT_EQ(cropRect->rect(), QRectF(40, 40, 110, 110));

    delete pressEvent;
    delete moveEvent;
}

TEST_F(CropRectangleTest, ResizeTopRightOutsideBorders) {
    cropRect->setBoundingBorders(QRectF(0, 0, 200, 200));
    cropRect->setRect(QRectF(50, 50, 140, 140));
    cropRect->setEnabled(true);

    QGraphicsSceneMouseEvent* pressEvent = createMouseEvent(QEvent::GraphicsSceneMousePress, QPointF(185, 55), QPointF(185, 55));
    cropRect->mousePressEvent(pressEvent);

    QGraphicsSceneMouseEvent* moveEvent = createMouseEvent(QEvent::GraphicsSceneMouseMove, QPointF(205, 55), QPointF(185, 55));
    cropRect->mouseMoveEvent(moveEvent);

    EXPECT_EQ(cropRect->rect(), QRectF(50, 50, 140, 140));

    delete pressEvent;
    delete moveEvent;
}

TEST_F(CropRectangleTest, ResizeBottomLeftWithinBorders) {
    cropRect->setBoundingBorders(QRectF(0, 0, 200, 200));
    cropRect->setRect(QRectF(50, 50, 100, 100));
    cropRect->setEnabled(true);

    QGraphicsSceneMouseEvent* pressEvent = createMouseEvent(QEvent::GraphicsSceneMousePress, QPointF(55, 145), QPointF(55, 145));
    cropRect->mousePressEvent(pressEvent);

    QGraphicsSceneMouseEvent* moveEvent = createMouseEvent(QEvent::GraphicsSceneMouseMove, QPointF(45, 155), QPointF(55, 145));
    cropRect->mouseMoveEvent(moveEvent);

    EXPECT_EQ(cropRect->rect(), QRectF(40, 50, 110, 110));

    delete pressEvent;
    delete moveEvent;
}

TEST_F(CropRectangleTest, ResizeBottomRightWithinBorders) {
    cropRect->setBoundingBorders(QRectF(0, 0, 200, 200));
    cropRect->setRect(QRectF(50, 50, 100, 100));
    cropRect->setEnabled(true);

    QGraphicsSceneMouseEvent* pressEvent = createMouseEvent(QEvent::GraphicsSceneMousePress, QPointF(145, 145), QPointF(145, 145));
    cropRect->mousePressEvent(pressEvent);

    QGraphicsSceneMouseEvent* moveEvent = createMouseEvent(QEvent::GraphicsSceneMouseMove, QPointF(155, 155), QPointF(145, 145));
    cropRect->mouseMoveEvent(moveEvent);

    EXPECT_EQ(cropRect->rect(), QRectF(50, 50, 110, 110));

    delete pressEvent;
    delete moveEvent;
}

TEST_F(CropRectangleTest, IgnoreEventsWhenInactive) {
    cropRect->setEnabled(false);
    cropRect->setBoundingBorders(QRectF(0, 0, 200, 200));
    cropRect->setRect(QRectF(50, 50, 100, 100));

    QGraphicsSceneMouseEvent* pressEvent = createMouseEvent(QEvent::GraphicsSceneMousePress, QPointF(145, 145), QPointF(145, 145));
    cropRect->mousePressEvent(pressEvent);

    QGraphicsSceneMouseEvent* moveEvent = createMouseEvent(QEvent::GraphicsSceneMouseMove, QPointF(155, 155), QPointF(145, 145));
    cropRect->mouseMoveEvent(moveEvent);

    EXPECT_EQ(cropRect->rect(), QRectF(50, 50, 100, 100));

    delete pressEvent;
    delete moveEvent;
}

TEST_F(CropRectangleTest, IgnoreMouseReleaseWhenInactive) {
    cropRect->setEnabled(false);
    QGraphicsSceneMouseEvent* releaseEvent = createMouseEvent(QEvent::GraphicsSceneMouseRelease, QPointF(0, 0), QPointF(0, 0));
    cropRect->mouseReleaseEvent(releaseEvent);
    EXPECT_EQ(cropRect->rect(), QRectF(0, 0, 0, 0));
    delete releaseEvent;
}
