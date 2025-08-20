#include "CropRectangle.h"
#include <QPen>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

CropRectangle::CropRectangle(QGraphicsItem* parent)
	: QGraphicsRectItem(parent)
	, theColor(Qt::blue)
	, theOpacity(50)
	, theBorderWidth(2)
	, theCornerHandleSize(20)
	, theCropState(CropState_Inactive)
{
	setRect(0, 0, 300, 300);
	setPen(QPen(theColor, theBorderWidth));
	setBrush(QColor(theColor.red(), theColor.green(), theColor.blue(), theOpacity));
}

CropRectangle::CropRectangle(const QRectF& rect, QGraphicsItem* parent)
	: CropRectangle(parent)
{
	setRect(rect);
}

void CropRectangle::setEnabled(bool enabled)
{
	if (enabled)
		theCropState = CropState_Active;
	else
		theCropState = CropState_Inactive;
}

QColor CropRectangle::color() const
{
	return theColor;
}

void CropRectangle::setColor(const QColor& color)
{
	theColor = color;
	setPen(QPen(theColor, theBorderWidth));
	setBrush(QColor(theColor.red(), theColor.green(), theColor.blue(), theOpacity));
}

int CropRectangle::opacity() const
{
	return theOpacity;
}

void CropRectangle::setOpacity(int opacity)
{
	theOpacity = opacity;
	setBrush(QColor(theColor.red(), theColor.green(), theColor.blue(), theOpacity));
}

int CropRectangle::borderWidth() const
{
	return theBorderWidth;
}

void CropRectangle::setBorderWidth(int width)
{
	theBorderWidth = width;
	setPen(QPen(theColor, theBorderWidth));
}

void CropRectangle::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	if (theCropState == CropState_Inactive)
		return;

	painter->setPen(QPen(theColor, theBorderWidth));
	painter->setBrush(QColor(theColor.red(), theColor.green(), theColor.blue(), theOpacity));
	painter->drawRect(this->rect());

	painter->setBrush(QColor(theColor.red(), theColor.green(), theColor.blue(), 255));
	QPointF corner = rect().topLeft();
	painter->drawRect(corner.x(), corner.y(), theCornerHandleSize, theCornerHandleSize);
	corner = rect().topRight();
	painter->drawRect(corner.x() - theCornerHandleSize, corner.y(), theCornerHandleSize, theCornerHandleSize);
	corner = rect().bottomLeft();
	painter->drawRect(corner.x(), corner.y() - theCornerHandleSize, theCornerHandleSize, theCornerHandleSize);
	corner = rect().bottomRight();
	painter->drawRect(corner.x() - theCornerHandleSize, corner.y() - theCornerHandleSize, theCornerHandleSize, theCornerHandleSize);
}

CropRectangle::CropHandle CropRectangle::getInsideCropHandle(QPointF point)
{
	CropRectangle::CropHandle handle = CropHandle_None;

	QPointF corner = rect().topLeft();
	QRectF crect = QRectF(corner.x(), corner.y(), theCornerHandleSize, theCornerHandleSize);
	if (crect.contains(point))
		handle = CropHandle_TopLeft;

	corner = rect().topRight();
	crect = QRectF(corner.x() - theCornerHandleSize, corner.y(), theCornerHandleSize, theCornerHandleSize);
	if (crect.contains(point))
		handle = CropHandle_TopRight;

	corner = rect().bottomLeft();
	crect = QRectF(corner.x(), corner.y() - theCornerHandleSize, theCornerHandleSize, theCornerHandleSize);
	if (crect.contains(point))
		handle = CropHandle_BottomLeft;

	corner = rect().bottomRight();
	crect = QRectF(corner.x() - theCornerHandleSize, corner.y() - theCornerHandleSize, theCornerHandleSize, theCornerHandleSize);
	if (crect.contains(point))
		handle = CropHandle_BottomRight;

	return handle;
}

void CropRectangle::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
	switch (theCropState)
	{
	case CropState_Inactive:
		event->ignore();
		break;
	case CropState_Active:
		CropRectangle::CropHandle handle = getInsideCropHandle(event->pos());
		switch (handle)
		{
		case CropHandle_None:
			theCropState = CropState_Translating;
			break;
		case CropHandle_TopLeft:
			theCropState = CropState_ResizingTL;
			break;
		case CropHandle_TopRight:
			theCropState = CropState_ResizingTR;
			break;
		case CropHandle_BottomLeft:
			theCropState = CropState_ResizingBL;
			break;
		case CropHandle_BottomRight:
			theCropState = CropState_ResizingBR;
			break;
		}
		QGraphicsItem::mousePressEvent(event);
		event->accept();
	}
}

void CropRectangle::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
	QPointF tl = rect().topLeft();
	QPointF br = rect().bottomRight();
	qreal dx = event->scenePos().x() - event->lastScenePos().x();
	qreal dy = event->scenePos().y() - event->lastScenePos().y();
	switch (theCropState)
	{
	case CropState_Inactive:
		event->ignore();
		break;
	case CropState_Active:
		break;
	case CropState_ResizingTL:
		tl.setX(tl.x() + dx);
		tl.setY(tl.y() + dy);
		setRect(QRectF(tl, br));
		break;
	case CropState_ResizingTR:
		tl.setY(tl.y() + dy);
		br.setX(br.x() + dx);
		setRect(QRectF(tl, br));
		break;
	case CropState_ResizingBL:
		tl.setX(tl.x() + dx);
		br.setY(br.y() + dy);
		setRect(QRectF(tl, br));
		break;
	case CropState_ResizingBR:
		br.setX(br.x() + dx);
		br.setY(br.y() + dy);
		setRect(QRectF(tl, br));
		break;
	case CropState_Translating:
		moveBy(dx, dy);
		break;
	}
}

void CropRectangle::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
	QGraphicsItem::mouseReleaseEvent(event);
	if (theCropState == CropState_Inactive)
	{
		event->ignore();
	}
	else
	{
		theCropState = CropState_Active; // Reset to active state after resizing or moving
		event->accept();
	}
}
