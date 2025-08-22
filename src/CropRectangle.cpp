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
	, theBoundingBorders(QRectF(0, 0, 0, 0))
{
	setRect(0, 0, 0, 0);
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
	
	setVisible(enabled);
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

void CropRectangle::setBoundingBorders(QRectF brect)
{
	if (brect.isValid())
	{
		theBoundingBorders = brect;
		//setPos(theBoundingBorders.topLeft());
		//setRect(theBoundingBorders);
	}
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

bool CropRectangle::isInsideVideo(const QPointF& point) const
{
	return (point.x() >= theBoundingBorders.left() && point.x() <= theBoundingBorders.right() &&
		point.y() >= theBoundingBorders.top() && point.y() <= theBoundingBorders.bottom());
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
	QPointF tr = rect().topRight();
	QPointF bl = rect().bottomLeft();
	QPointF br = rect().bottomRight();
	QRectF realrect = rect();
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
		if (isInsideVideo(realrect.topLeft() + QPointF(dx, dy)))
		{
			tl.setX(tl.x() + dx);
			tl.setY(tl.y() + dy);
			setRect(QRectF(tl, br));
		}
		break;
	case CropState_ResizingTR:
		if (isInsideVideo(realrect.topRight() + QPointF(dx, dy)))
		{
			tl.setY(tl.y() + dy);
			br.setX(br.x() + dx);
			setRect(QRectF(tl, br));
		}
		break;
	case CropState_ResizingBL:
		if (isInsideVideo(realrect.bottomLeft() + QPointF(dx, dy)))
		{
			tl.setX(tl.x() + dx);
			br.setY(br.y() + dy);
			setRect(QRectF(tl, br));
		}
		break;
	case CropState_ResizingBR:
		if (isInsideVideo(realrect.bottomRight() + QPointF(dx, dy)))
		{
			br.setX(br.x() + dx);
			br.setY(br.y() + dy);
			setRect(QRectF(tl, br));
		}
		break;
	case CropState_Translating:
		if (isInsideVideo(realrect.topLeft() + QPointF(dx, dy)) &&
			isInsideVideo(realrect.topRight() + QPointF(dx, dy)) &&
			isInsideVideo(realrect.bottomLeft() + QPointF(dx, dy)) &&
			isInsideVideo(realrect.bottomRight() + QPointF(dx, dy))
			)
		{
			tl += QPointF(dx, dy);
			tr += QPointF(dx, dy);
			bl += QPointF(dx, dy);
			br += QPointF(dx, dy);
			setRect(QRectF(tl.x(), tl.y(), tr.x() - tl.x(), bl.y() - tl.y()));
		}
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
