#pragma once
#include <QGraphicsRectItem>
#include <QPainter>


class CropRectangle : public QGraphicsRectItem
{
	//Q_OBJECT
	//Q_PROPERTY(QColor color READ color WRITE setColor)
	//Q_PROPERTY(int opacity READ opacity WRITE setOpacity)
	//Q_PROPERTY(int borderWidth READ borderWidth WRITE setBorderWidth)

public:
	explicit CropRectangle(QGraphicsItem* parent = nullptr);
	CropRectangle(const QRectF& rect, QGraphicsItem* parent = nullptr);

	void setEnabled(bool enabled);
	QColor color() const;
	void setColor(const QColor& color);
	int opacity() const;
	void setOpacity(int opacity);
	int borderWidth() const;
	void setBorderWidth(int width);

	enum CropHandle
	{
		CropHandle_None,
		CropHandle_TopLeft,
		CropHandle_TopRight,
		CropHandle_BottomLeft,
		CropHandle_BottomRight
	};

	enum CropState
	{
		CropState_Inactive,
		CropState_Active,
		CropState_ResizingTL,
		CropState_ResizingTR,
		CropState_ResizingBL,
		CropState_ResizingBR,
		CropState_Translating
	};

protected:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
	void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
	CropRectangle::CropHandle getInsideCropHandle(QPointF point);

	QColor theColor;
	int theOpacity;
	int theBorderWidth;
	int theCornerHandleSize;
	CropState theCropState;
};
