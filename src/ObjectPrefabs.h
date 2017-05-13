#pragma once

#include <qwidget.h>
#include <qpainter.h>
#include <qstyleditemdelegate.h>
#include <qlistwidget.h>
#include <qaction.h>
#include <qmenu.h>

#include "Level.h"
#include "Thumbnail.h"

Q_DECLARE_METATYPE(glm::vec2*);


class ObjectPrefabDelegate : public QStyledItemDelegate
{
public:
	ObjectPrefabDelegate(QObject* parent=0) : QStyledItemDelegate(parent) {};
	~ObjectPrefabDelegate() {};

protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		//QRect rect = option.rect;
		QRect rect = QRect(option.rect.x(), option.rect.y(), option.rect.width(), option.rect.height());

		if (option.state & QStyle::State_Selected)
		{
			painter->setPen(QPen(QColor(224, 224, 0), 3, Qt::SolidLine));
			painter->drawRect(QRect(rect.x()+1, rect.y()+1, rect.width()-3, rect.height()-3));
		}

		QPen grey(QColor::fromRgb(192, 192, 192), 1, Qt::SolidLine);

		QRect textrect = QRect(rect.x() + 3, rect.y() + rect.height() - 20 - 3, rect.width() - 1 - 6, 20);

		painter->setPen(grey);
		painter->drawRect(textrect);
		painter->setPen(Qt::white);
		painter->drawText(textrect, Qt::AlignBottom|Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

		int thumbside = rect.height() - 20 - 20;

		QImage* image = index.data(Qt::UserRole + 1).value<QImage*>();

		int mx = rect.x() + (rect.width() / 2);
		int my = rect.y() + ((rect.height() - 20) / 2);
		painter->drawImage(QRect(mx-(thumbside/2), my-(thumbside/2), thumbside, thumbside), *image);
	};

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		return QSize(250, 250);
	}
};


class ObjectPrefabs : public QListWidget
{
	Q_OBJECT

public:
	ObjectPrefabs(QWidget* parent, Level* level, QImage* texture);
	~ObjectPrefabs();

protected:

signals:
	void selectionChanged(int object_id);
	void prefabScaleChanged(double scale);

public slots:
	void reset();
	void add(int object_id);
	void remove(int object_id);
	void select(int object_id);
	void deselect();
	void itemSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
	void setTexture(QImage* texture);
	void showContextMenu(const QPoint& pos);
	void removeObject();

private:
	void fillTriangle(QImage* image, glm::vec4 p1, glm::vec4 p2, glm::vec4 p3);
	void makeThumbnail(QImage* image, const glm::vec4* points, int num_points);

	Level* m_level;
	QImage* m_texture;

	QAction* m_remove_action;
	int m_context_item_id;
};
