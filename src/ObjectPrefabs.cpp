#include "ObjectPrefabs.h"

ObjectPrefabs::ObjectPrefabs(QWidget* parent, Level* level, QImage* texture) : QListWidget(parent)
{
	m_level = level;
	m_texture = texture;

	connect(this, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(itemSelectionChanged(QListWidgetItem*, QListWidgetItem*)));

	setItemDelegate(new ObjectPrefabDelegate(this));
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	setStyleSheet("background-color: #323232;");

	setContextMenuPolicy(Qt::CustomContextMenu);

	m_remove_action = new QAction(tr("Remove Object"), this);
	this->addAction(m_remove_action);

	connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
	connect(m_remove_action, SIGNAL(triggered()), this, SLOT(removeObject()));

}

ObjectPrefabs::~ObjectPrefabs()
{
}

void ObjectPrefabs::itemSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (current)
	{
		int id = current->data(Qt::UserRole + 2).toInt();
		emit selectionChanged(id);
	}
}

void ObjectPrefabs::reset()
{
	QList<QListWidgetItem*> list = findItems("*", Qt::MatchWildcard);
	QListIterator<QListWidgetItem*> i(list);

	while (i.hasNext())
	{
		QListWidgetItem* obj = i.next();

		QImage* image = obj->data(Qt::UserRole + 1).value<QImage*>();
		if (image != NULL)
			delete image;

		delete obj;
	};
}

void ObjectPrefabs::add(int object_id)
{
	const Level::Prefab* prefab = m_level->getPrefabById(object_id);
	if (prefab != NULL)
	{
		QImage* image = new QImage(256, 256, QImage::Format_ARGB32);
	
		Thumbnail::fromPoly(image, m_texture, prefab->points, prefab->num_points);

		QString object_name = QString(prefab->name.c_str());	

		QListWidgetItem* item = new QListWidgetItem(this);
		item->setData(Qt::DisplayRole, object_name);
		item->setData(Qt::UserRole + 1, QVariant::fromValue(image));
		item->setData(Qt::UserRole + 2, QVariant::fromValue(object_id));
		item->setFlags(item->flags());
	
		addItem(item);
	}
}

void ObjectPrefabs::remove(int object_id)
{
	QList<QListWidgetItem*> list = findItems("*", Qt::MatchWildcard);
	QListIterator<QListWidgetItem*> i(list);

	while (i.hasNext())
	{		
		QListWidgetItem* obj = i.next();

		int id = obj->data(Qt::UserRole + 2).toInt();

		if (object_id == id)
		{
			QListWidgetItem* current = currentItem();
			if (current->data(Qt::UserRole + 2).toInt() == id)
			{
				this->setCurrentItem(NULL);
				emit selectionChanged(-1);
			}

			QImage* image = obj->data(Qt::UserRole + 1).value<QImage*>();
			if (image != NULL)
				delete image;

			if (obj->isSelected())
				setCurrentRow(-1);
			delete obj;

			m_level->removePrefab(object_id);
		}
	};
}

void ObjectPrefabs::select(int object_id)
{
	int numrows = count();
	for (int i=0; i < numrows; i++)
	{
		QListWidgetItem* obj = item(i);
		int id = obj->data(Qt::UserRole + 1).toInt();

		if (object_id == id)
		{
			setCurrentRow(i);
			emit selectionChanged(id);
			return;
		}
	}
	emit selectionChanged(-1);
}

void ObjectPrefabs::deselect()
{
	setCurrentRow(-1);
}

void ObjectPrefabs::setTexture(QImage* texture)
{
	m_texture = texture;

	// update thumbnails
	QList<QListWidgetItem*> list = findItems("*", Qt::MatchWildcard);
	QListIterator<QListWidgetItem*> i(list);

	while (i.hasNext())
	{
		QListWidgetItem* obj = i.next();

		QImage* image = obj->data(Qt::UserRole + 1).value<QImage*>();
		int id = obj->data(Qt::UserRole + 2).toInt();
		
		const Level::Prefab* prefab = m_level->getPrefabById(id);
		if (prefab != NULL)
		{
			Thumbnail::fromPoly(image, texture, prefab->points, prefab->num_points);
		}
	};

	update();
}

void ObjectPrefabs::removeObject()
{
	emit remove(m_context_item_id);

	update();
}

void ObjectPrefabs::showContextMenu(const QPoint& pos)
{
	QListWidgetItem* item = itemAt(pos);

	m_context_item_id = -1;

	if (item != NULL)
	{
		m_context_item_id = item->data(Qt::UserRole + 2).toInt();

		QMenu menu(tr("Context Menu"), this);
		menu.addAction(m_remove_action);
		menu.exec(mapToGlobal(pos));		
	}
}
