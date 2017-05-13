#include "TiledPrefabs.h"


TiledPrefabsWidget::TiledPrefabsWidget(QWidget* parent, Level* level, QImage* texture) : QListWidget(parent)
{
	m_level = level;
	m_texture = texture;

	setFlow(QListView::LeftToRight);
	setWrapping(true);

	setItemDelegate(new TiledPrefabsDelegate(this));

	defaults();

	setCurrentRow(0);
}

TiledPrefabsWidget::~TiledPrefabsWidget()
{
}

void TiledPrefabsWidget::defaults()
{
	QImage* image = new QImage(TILEDPREFABS_THUMB_W, TILEDPREFABS_THUMB_H, QImage::Format_ARGB32);
	image->fill(QColor(0, 0, 0, 255));

	QListWidgetItem* item = new QListWidgetItem();
	item->setData(Qt::DisplayRole, QString("[Empty]"));
	item->setData(Qt::UserRole + 1, QVariant::fromValue<QImage*>(image));
	item->setData(Qt::UserRole + 2, -1);

	addItem(item);
}

void TiledPrefabsWidget::reset()
{
	QList<QListWidgetItem*> list = findItems("*", Qt::MatchWildcard);
	QListIterator<QListWidgetItem*> i(list);

	while (i.hasNext())
	{		
		QListWidgetItem* obj = i.next();
		delete obj;
	};

	defaults();

	setCurrentRow(0);
}

void TiledPrefabsWidget::add(int tile_id)
{
	const Tilemap::Tile* tile = m_level->getTileById(tile_id);
	assert(tile != NULL);

	if (tile != nullptr)
	{
		QImage* image = new QImage(TILEDPREFABS_THUMB_W, TILEDPREFABS_THUMB_H, QImage::Format_ARGB32);
//		Thumbnail::fromRect(image, m_texture, tile->points);

		QListWidgetItem* item = new QListWidgetItem();
		item->setData(Qt::DisplayRole, QString(tile->name.c_str()));
		item->setData(Qt::UserRole + 1, QVariant::fromValue<QImage*>(image));
		item->setData(Qt::UserRole + 2, tile_id);

		addItem(item);
	}
}

void TiledPrefabsWidget::remove(int tile_id)
{
	QList<QListWidgetItem*> list = findItems("*", Qt::MatchWildcard);
	QListIterator<QListWidgetItem*> i(list);

	while (i.hasNext())
	{		
		QListWidgetItem* obj = i.next();
		int id = obj->data(Qt::UserRole + 2).toInt();

		if (tile_id == id)
		{
			if (obj->isSelected())
				setCurrentRow(0);

			QImage* image = obj->data(Qt::UserRole + 1).value<QImage*>();
			if (image != nullptr)
				delete image;
				
			delete obj;
		}
	};
}

void TiledPrefabsWidget::setTexture(QImage* texture)
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
		
		const Tilemap::Tile* tile = m_level->getTileById(id);
		if (tile != nullptr)
		{
	//		Thumbnail::fromRect(image, m_texture, tile->points);
		}
	};

	update();
}



TiledPrefabsWindow::TiledPrefabsWindow(QWidget* parent, Level* level, QImage* texture) : QDockWidget("Tiled Prefabs", parent, 0)
{
	m_window = new QMainWindow(0);
	m_level = level;

	setFocusPolicy(Qt::ClickFocus);

	m_widget = new TiledPrefabsWidget(this, level, texture);
	m_widget->setMinimumWidth(1000);


	m_window->setParent(this);
	setWidget(m_window);
	m_window->setCentralWidget(m_widget);
	m_window->setContextMenuPolicy(Qt::NoContextMenu);
	
	setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
	setFloating(true);

	this->setMinimumWidth(1000);
	this->setMinimumHeight(700);
	this->setMaximumWidth(1800);
	this->setMaximumHeight(1500);

	// don't allow docking
	this->setAllowedAreas(0);

	this->setHidden(false);

	m_toolbar = m_window->addToolBar("Edit");

	m_widthSpin = new QSpinBox();
	m_widthSpin->setRange(1, 100);
	m_widthSpin->setSingleStep(1);
	connect(m_widthSpin, SIGNAL(valueChanged(int)), this, SLOT(editDimensionsChanged(int)));

	m_widthLabel = new QLabel("Width:");
	m_widthLabel->setMinimumWidth(50);

	m_heightSpin = new QSpinBox();
	m_heightSpin->setRange(1, 100);
	m_heightSpin->setSingleStep(1);
	connect(m_heightSpin, SIGNAL(valueChanged(int)), this, SLOT(editDimensionsChanged(int)));

	m_heightLabel = new QLabel("Height:");
	m_heightLabel->setMinimumWidth(50);

	m_sizeSpin = new QDoubleSpinBox();
	m_sizeSpin->setRange(0.01, 100.0);
	m_sizeSpin->setValue(1.0);
	m_sizeSpin->setSingleStep(1.0);
	connect(m_sizeSpin, SIGNAL(valueChanged(double)), this, SLOT(editTileSizeChanged(double)));

	m_sizeLabel = new QLabel("Tile size:");
	m_sizeLabel->setMinimumWidth(50);

	QBoxLayout* edit_widget_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	edit_widget_layout->setSpacing(2);
	edit_widget_layout->setMargin(1);
	edit_widget_layout->addWidget(m_widthLabel);
	edit_widget_layout->addWidget(m_widthSpin);
	edit_widget_layout->addWidget(m_heightLabel);
	edit_widget_layout->addWidget(m_heightSpin);
	edit_widget_layout->addWidget(m_sizeLabel);
	edit_widget_layout->addWidget(m_sizeSpin);
	
	m_editWidget = new QWidget();
	m_editWidget->setMaximumHeight(30);
	m_editWidget->setLayout(edit_widget_layout);

	m_toolbar->addWidget(m_editWidget);

	connect(m_widget, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)), this, SLOT(itemSelectionChanged(QListWidgetItem*, QListWidgetItem*)));
}

TiledPrefabsWindow::~TiledPrefabsWindow()
{
}

void TiledPrefabsWindow::closeEvent(QCloseEvent* event)
{
	emit onClose();
}

void TiledPrefabsWindow::editDimensionsChanged(int size)
{
	int width = m_widthSpin->value();
	int height = m_heightSpin->value();

	emit onChangeDimensions(width, height);
}

void TiledPrefabsWindow::editTileSizeChanged(double size)
{
	emit onChangeTileSize(size);
}

void TiledPrefabsWindow::reset()
{
	m_widget->reset();
}

void TiledPrefabsWindow::add(int tile_id)
{
	m_widget->add(tile_id);
}

void TiledPrefabsWindow::remove(int tile_id)
{
	m_widget->remove(tile_id);
}

void TiledPrefabsWindow::setTexture(QImage* texture)
{
	m_widget->setTexture(texture);
}

void TiledPrefabsWindow::itemSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
	if (current)
	{
		int id = current->data(Qt::UserRole + 2).toInt();
	}
}