#include "MainWindow.h"

#include "QApplication"
#include "QtGui"

MainWindow::MainWindow()
{
	m_texture = new QImage();

	m_level = new Level();
	readConfigFile(tr("config.xml"));

	QWidget *central_widget = new QWidget();

	// main components
	// ---------------------------------------------------------------------------

	m_glwidget = new GLWidget(this, m_level);
	m_objbrowser = new ObjectBrowser(this, m_level);
	
	m_texedit = new TextureEdit(this, m_level);

	m_objedit = new ObjectEdit(this, m_level);
	m_objfilter = new ObjectFilter(this);
	
	m_objprefabs = new ObjectPrefabs(this, m_level, m_texture);
	m_objdesigner = new ObjectDesigner(this, m_level, m_objprefabs);
	

	m_visbox_conf = new VisboxConf(this);

	m_tilemap_widget = new TilemapWidget(this, m_level);
	m_tilemap_widget->setValues(0, 50, 0, 50);
	m_tilemap_widget->setSize(1.0f);

	m_tileset_window = new TilesetWindow(this, m_level, m_texture);

	m_tiledprefabs_window = new TiledPrefabsWindow(this, m_level, m_texture);

	m_level_conf = new LevelConf(this);

	// side widget
	// ---------------------------------------------------------------------------
	QWidget *side_widget = new QWidget();
	
	QBoxLayout *side_layout = new QBoxLayout(QBoxLayout::TopToBottom);
	side_layout->setSpacing(3);
	side_layout->setMargin(0);

	side_widget->setLayout(side_layout);

	m_objbrowser->setMinimumSize(300, 410);
	m_objbrowser->setMaximumWidth(400);


	// object prefabs
	// ---------------------------------------------------------------------------
	m_objprefabs->setMinimumWidth(260);
	m_objprefabs->setMaximumWidth(260);

	
	// main layout
	// ---------------------------------------------------------------------------
	QGridLayout *layout = new QGridLayout();
	layout->setSpacing(3);
	layout->setMargin(3);

	layout->addWidget(m_glwidget, 0, 0);
	layout->addWidget(m_objprefabs, 0, 1);
	layout->addWidget(m_objbrowser, 0, 2);


	central_widget->setLayout(layout);
	setCentralWidget(central_widget);


	// connect signals and slots
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), m_glwidget, SLOT(animate()));
	timer->start(50);


	connect(m_glwidget, SIGNAL(onAddObject(int)), m_objbrowser, SLOT(add(int)));

	connect(m_glwidget, SIGNAL(onRemoveObject(int)), m_objbrowser, SLOT(remove(int)));
	connect(m_glwidget, SIGNAL(onRemoveObject(int)), m_texedit, SLOT(remove(int)));
	connect(m_glwidget, SIGNAL(onRemoveObject(int)), m_objedit, SLOT(remove(int)));
	
	connect(m_glwidget, SIGNAL(onSelectObject(int)), m_objbrowser, SLOT(select(int)));
	connect(m_glwidget, SIGNAL(onSelectObject(int)), m_texedit, SLOT(select(int)));
	connect(m_glwidget, SIGNAL(onSelectObject(int)), m_objedit, SLOT(select(int)));
	
	connect(m_glwidget, SIGNAL(onDeselectObject()), m_objbrowser, SLOT(deselect()));
	connect(m_glwidget, SIGNAL(onDeselectObject()), m_texedit, SLOT(deselect()));
	connect(m_glwidget, SIGNAL(onDeselectObject()), m_objedit, SLOT(deselect()));

	connect(m_glwidget, SIGNAL(onPrefabScaleChange(double)), this, SLOT(setPrefabScale(double)));
	
	connect(m_glwidget, SIGNAL(onSetMode(GLWidget::OperationMode)), m_objedit, SLOT(setMode(GLWidget::OperationMode)));
	
	connect(m_objbrowser, SIGNAL(onSelectObject(int)), m_glwidget, SLOT(select(int)));
	connect(m_objbrowser, SIGNAL(onSelectObject(int)), m_texedit, SLOT(select(int)));
	connect(m_objbrowser, SIGNAL(onSelectObject(int)), m_objedit, SLOT(select(int)));

	connect(m_objbrowser, SIGNAL(onObjectDataChanged(int)), m_objedit, SLOT(objectDataChanged(int)));
	connect(m_objedit, SIGNAL(onObjectDataChanged(int)), m_objbrowser, SLOT(objectDataChanged(int)));
	
	connect(m_texedit, SIGNAL(onClose()), this, SLOT(texEditClosed()));
	connect(m_objedit, SIGNAL(onClose()), this, SLOT(objEditClosed()));
	connect(m_objfilter, SIGNAL(onClose()), this, SLOT(objFilterClosed()));
	connect(m_objdesigner, SIGNAL(onClose()),this, SLOT(objDesignerClosed()));
	connect(m_tileset_window, SIGNAL(onClose()), this, SLOT(tilesetWindowClosed()));
	connect(m_tiledprefabs_window, SIGNAL(onClose()), this, SLOT(tiledprefabsWindowClosed()));

	connect(m_objedit, SIGNAL(onSetCreateType(Level::ObjectType)), m_glwidget, SLOT(setCreateType(Level::ObjectType)));
	connect(m_objedit, SIGNAL(onSetCreateTriggerType(int)), m_glwidget, SLOT(setCreateTriggerType(int)));

	connect(m_objfilter, SIGNAL(onFilterEnable(int)), this, SLOT(enableFilters(int)));
	connect(m_objfilter, SIGNAL(onFilterDisable(int)), this, SLOT(disableFilters(int)));
	connect(m_objfilter, SIGNAL(onDisplayEnable(int)), m_glwidget, SLOT(enableDisplays(int)));
	connect(m_objfilter, SIGNAL(onDisplayDisable(int)), m_glwidget, SLOT(disableDisplays(int)));

	connect(m_objprefabs, SIGNAL(selectionChanged(int)), m_glwidget, SLOT(selectPrefab(int)));

	connect(m_tilemap_widget, SIGNAL(onConfigChanged()), this, SLOT(tilemapConfigChange()));

	connect(m_objdesigner, SIGNAL(onInsertTile(int)), m_tileset_window, SLOT(add(int)));

	connect(m_tileset_window, SIGNAL(onSelectTile(int)), m_glwidget, SLOT(setTileBrush(int)));

	connect(m_tiledprefabs_window, SIGNAL(onSelectPrefab(int)), m_glwidget, SLOT(setTiledPrefab(int)));
	connect(m_tiledprefabs_window, SIGNAL(onChangeDimensions(int,int)), m_glwidget, SLOT(setTiledPrefabSize(int,int)));
	connect(m_tiledprefabs_window, SIGNAL(onChangeTileSize(double)), m_glwidget, SLOT(setTiledPrefabTileSize(double)));

	// zoom shortcuts
	m_zoomin_shortcut = new QShortcut(QKeySequence(Qt::Key_Plus), this);
	m_zoomout_shortcut = new QShortcut(QKeySequence(Qt::Key_Minus), this);
	connect(m_zoomin_shortcut, SIGNAL(activated()), this, SLOT(zoomIn()));
	connect(m_zoomout_shortcut, SIGNAL(activated()), this, SLOT(zoomOut()));



	// create various stuff
	createActions();
	createMenus();
	createToolbars();
	createStatusbar();

	setCurrentFile(tr(""));
	m_texture_file = "";
	
	// go to selection mode by default
	m_select_action->setChecked(true);


	// object editor positioning
	m_objedit->move(QPoint(width() + 120, 100));

	// object filter positioning
	m_objfilter->move(QPoint(width() - 300, 100));

	// object designer positioning
	m_objdesigner->move(QPoint(width() - 600, 230));

	// tileset window positioning
	m_tileset_window->move(QPoint(width() + 850, 600));

	// tiled prefabs window positioning
	m_tiledprefabs_window->move(QPoint(width() + 600, 600));


	// tex edit hidden by default
	m_texedit_open = false;
	m_toggle_texedit->setChecked(false);
	m_texedit->setHidden(true);

	// obj edit shown by default
	m_objedit_open = true;
	m_toggle_objedit->setChecked(true);
	m_objedit->setHidden(false);

	// obj filter shown by default
	m_objfilter_open = true;
	m_toggle_objfilter->setChecked(true);
	m_objfilter->setHidden(false);

	// obj designer hidden by default
	m_objdesigner_open = false;
	m_toggle_objdesigner->setChecked(false);
	m_objdesigner->setHidden(true);

	// tileset shown by default
	m_tileset_window_open = true;
	m_toggle_tileset_window->setChecked(true);
	m_tileset_window->setHidden(false);

	// tiled prefabs shown by default
	m_tiledprefabs_window_open = true;
	m_toggle_tiledprefabs_window->setChecked(true);
	m_tiledprefabs_window->setHidden(false);

	
	// grid settings
	// TODO: load settings
	m_gridSizeCombo->setCurrentIndex(3);
	emit m_glwidget->setGridSize(3);

	m_enable_grid = false;
	m_toggleGridAction->setChecked(false);
	m_snap_grid = false;
	m_snapGridAction->setChecked(false);

	m_glwidget->enableGrid(m_enable_grid);
	m_glwidget->setSnapGrid(m_snap_grid);


	// visbox
	m_toggle_visbox->setChecked(false);
	m_enable_visbox = false;
	m_glwidget->enableVisbox(m_enable_visbox);


	// zoom settings
	m_zoomLevelCombo->setCurrentIndex(3);
	emit m_glwidget->setZoomLevel(3);


	// filter setting
	emit m_objfilter->setFilters(0xff);
	emit m_objfilter->setDisplays(0xff);


	// default type
	m_defTypeCombo->setCurrentIndex(1);
	emit m_objedit->setDefaultType(Level::OBJECT_TYPE_GEOVIS);


	// prefab scale
	m_prefabScaleSpin->setValue(1.0);

	// prefab depth
	m_prefabDepthSpin->setValue(0);
}

MainWindow::~MainWindow(void)
{
	delete m_level;
}

bool MainWindow::handleUnsaved()
{
	QMessageBox box;
	box.setText("The document has been modified.");
	box.setInformativeText("Do you want to save the changes?");
	box.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
	box.setDefaultButton(QMessageBox::Save);
	int ret = box.exec();

	switch (ret)
	{
		case QMessageBox::Save:
		{
			emit saveFile();
			return true;
		}
		case QMessageBox::Discard:
		{
			return true;
		}
		case QMessageBox::Cancel:
		{
			return false;
		}
		default:
			break;
	}

	return false;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (m_level->isModified())
	{
		bool ret = handleUnsaved();

		if (ret)
			event->accept();
		else
			event->ignore();
	}
}


QString MainWindow::getLevelDir()
{
	const QString LEVEL_DIR("level_dir");
	QSettings settings;

	return settings.value(LEVEL_DIR).toString();
}

void MainWindow::setLevelDir(QString path)
{
	const QString LEVEL_DIR("level_dir");
	QSettings settings;

	QDir dir(path);
	QString level_path = dir.absolutePath();
	QString level_dir = level_path.section("/", 0, -2);

	settings.setValue(LEVEL_DIR, level_dir);
}

QString MainWindow::getTextureDir()
{
	const QString TEXTURE_DIR("texture_dir");
	QSettings settings;

	return settings.value(TEXTURE_DIR).toString();
}

void MainWindow::setTextureDir(QString path)
{
	const QString TEXTURE_DIR("texture_dir");
	QSettings settings;

	QDir dir(path);
	QString tex_path = dir.absolutePath();
	QString tex_dir = tex_path.section("/", 0, -2);

	settings.setValue(TEXTURE_DIR, tex_dir);
}

QString MainWindow::getExportDir()
{
	const QString EXPORT_DIR("export_dir");
	QSettings settings;

	return settings.value(EXPORT_DIR).toString();
}

void MainWindow::setExportDir(QString path)
{
	const QString EXPORT_DIR("export_dir");
	QSettings settings;

	QDir dir(path);
	QString exp_path = dir.absolutePath();
	QString exp_dir = exp_path.section("/", 0, -2);

	settings.setValue(EXPORT_DIR, exp_dir);
}


void MainWindow::newFile()
{
	if (m_level->isModified())
	{
		bool ret = handleUnsaved();

		if (!ret)
		{
			return;
		}
	}
	newDocument();
	resetControls();
}

void MainWindow::openFile()
{
	if (m_level->isModified())
	{
		bool ret = handleUnsaved();

		if (!ret)
		{
			return;
		}
	}


	QString filename = QFileDialog::getOpenFileName(this,
													tr("Open Level File"),
													getLevelDir(),
													tr("Level File (*.blpf);;All Files (*.*)"));

	if (!filename.isEmpty())
	{
		if (readBinaryProjectFile(filename))
		{
			setCurrentFile(filename);
			m_level->resetModify();

			// set current directory
			setLevelDir(filename);
		}
	}
}

void MainWindow::openLegacyFile()
{
	if (m_level->isModified())
	{
		bool ret = handleUnsaved();

		if (!ret)
		{
			return;
		}
	}


	QString filename = QFileDialog::getOpenFileName(this,
													tr("Open Legacy Level File"),
													getLevelDir(),
													tr("Level File (*.lvl.xml);;All Files (*.*)"));

	if (!filename.isEmpty())
	{
		if (readLevelFile(filename))
		{
		//	setCurrentFile(QString(""));
			m_level->resetModify();

			// set current directory
			setLevelDir(filename);
		}
	}
}

bool MainWindow::saveFile()
{
	if (m_open_file.length() <= 0)	// save to a new file
	{
		QString filename = QFileDialog::getSaveFileName(this,
														tr("Save Level File"),
														getLevelDir(),
														tr("Level File (*.blpf);;All Files (*.*)"));
		if (!filename.isEmpty())
		{
			//writeLevelFile(filename);
			writeBinaryProjectFile(filename);
			setCurrentFile(filename);
			m_level->resetModify();

			// set current directory
			setLevelDir(filename);
			return true;
		}
		else
		{
			return false;
		}
	}
	else						// save to existing file
	{
		//writeLevelFile(m_open_file);
		writeBinaryProjectFile(m_open_file);
		m_level->resetModify();
		return true;
	}

	return false;
}

bool MainWindow::saveAsFile()
{
	QString filename = QFileDialog::getSaveFileName(this,
													tr("Save Level File"),
													getLevelDir(),
													tr("Level File (*.blpf);;All Files (*.*)"));

	if (!filename.isEmpty())
	{
		//writeLevelFile(filename);
		writeBinaryProjectFile(filename);
		setCurrentFile(filename);
		m_level->resetModify();

		// set current directory
		setLevelDir(filename);
		return true;
	}
	else
	{
		return false;
	}
}

void MainWindow::exportLevel()
{
	QString filename = QFileDialog::getSaveFileName(this,
													tr("Export Level File"),
													getExportDir(),
													tr("Level Binary (*.blb);;All Files (*.*)"));

	if (!filename.isEmpty())
	{
		writeBLBFile(filename);

		// set current directory
		setExportDir(filename);
	}
}

void MainWindow::exitProgram()
{

}


void MainWindow::selectionMode()
{
	emit m_glwidget->setMode(GLWidget::MODE_SELECT);
	m_select_action->setChecked(true);
}

void MainWindow::moveMode()
{
	emit m_glwidget->setMode(GLWidget::MODE_MOVE);
	m_move_action->setChecked(true);
}

void MainWindow::rotateMode()
{
	emit m_glwidget->setMode(GLWidget::MODE_ROTATE);
	m_rotate_action->setChecked(true);
}

void MainWindow::scaleMode()
{
	emit m_glwidget->setMode(GLWidget::MODE_SCALE);
	m_scale_action->setChecked(true);
}

void MainWindow::drawPolyMode()
{
	emit m_glwidget->setMode(GLWidget::MODE_DRAW_POLY);
	m_draw_poly_action->setChecked(true);
}

void MainWindow::drawRectMode()
{
	emit m_glwidget->setMode(GLWidget::MODE_DRAW_RECT);
	m_draw_rect_action->setChecked(true);
}

void MainWindow::prefabMode()
{
	emit m_glwidget->setMode(GLWidget::MODE_PREFAB);
	m_prefab_action->setChecked(true);
}

void MainWindow::tiledprefabMode()
{
	emit m_glwidget->setMode(GLWidget::MODE_TILEDPREFAB);
	m_tiledprefab_action->setChecked(true);
}

void MainWindow::tilemapMode()
{
	emit m_glwidget->setMode(GLWidget::MODE_TILEMAP);
	m_tilemap_action->setChecked(true);
}


void MainWindow::toggleTexEdit()
{
	if (m_texedit_open)
	{
		emit m_texedit->setHidden(true);
		m_texedit_open = false;
		m_toggle_texedit->setChecked(false);
	}
	else
	{
		emit m_texedit->setHidden(false);
		m_texedit_open = true;
		m_toggle_texedit->setChecked(true);
	}
}

void MainWindow::texEditClosed()
{
	m_texedit_open = false;
	m_toggle_texedit->setChecked(false);
}

void MainWindow::toggleObjEdit()
{
	if (m_objedit_open)
	{
		emit m_objedit->setHidden(true);
		m_objedit_open = false;
		m_toggle_objedit->setChecked(false);
	}
	else
	{
		emit m_objedit->setHidden(false);
		m_objedit_open = true;
		m_toggle_objedit->setChecked(true);
	}
}

void MainWindow::objEditClosed()
{
	m_objedit_open = false;
	m_toggle_objedit->setChecked(false);
}

void MainWindow::toggleObjFilter()
{
	if (m_objfilter_open)
	{
		emit m_objfilter->setHidden(true);
		m_objfilter_open = false;
		m_toggle_objfilter->setChecked(false);
	}
	else
	{
		emit m_objfilter->setHidden(false);
		m_objfilter_open = true;
		m_toggle_objfilter->setChecked(true);
	}
}

void MainWindow::objFilterClosed()
{
	m_objfilter_open = false;
	m_toggle_objfilter->setChecked(false);
}

void MainWindow::toggleObjDesigner()
{
	if (m_objdesigner_open)
	{
		emit m_objdesigner->setHidden(true);
		m_objdesigner_open = false;
		m_toggle_objdesigner->setChecked(false);
	}
	else
	{
		emit m_objdesigner->setHidden(false);
		m_objdesigner_open = true;
		m_toggle_objdesigner->setChecked(true);
	}
}

void MainWindow::objDesignerClosed()
{
	m_objdesigner_open = false;
	m_toggle_objdesigner->setChecked(false);
}

void MainWindow::toggleTilesetWindow()
{
	if (m_tileset_window_open)
	{
		emit m_tileset_window->setHidden(true);
		m_tileset_window_open = false;
		m_toggle_tileset_window->setChecked(false);
	}
	else
	{
		emit m_tileset_window->setHidden(false);
		m_tileset_window_open = true;
		m_toggle_tileset_window->setChecked(true);
	}
}

void MainWindow::tilesetWindowClosed()
{
	m_tileset_window_open = false;
	m_toggle_tileset_window->setChecked(false);
}

void MainWindow::toggleTiledprefabsWindow()
{
	if (m_tiledprefabs_window_open)
	{
		emit m_tiledprefabs_window->setHidden(true);
		m_tiledprefabs_window_open = false;
		m_toggle_tiledprefabs_window->setChecked(false);
	}
	else
	{
		emit m_tiledprefabs_window->setHidden(false);
		m_tiledprefabs_window_open = true;
		m_toggle_tiledprefabs_window->setChecked(true);
	}
}

void MainWindow::tiledprefabsWindowClosed()
{
	m_tiledprefabs_window_open = false;
	m_toggle_tiledprefabs_window->setChecked(false);
}

void MainWindow::toggleVisbox()
{
	if (m_enable_visbox)
		m_enable_visbox = false;
	else
		m_enable_visbox = true;
	
	m_glwidget->enableVisbox(m_enable_visbox);
}

void MainWindow::visboxConfig()
{
	if (m_visbox_conf->exec() == QDialog::Accepted)
	{
		float width = m_visbox_conf->getWidth();
		float height = m_visbox_conf->getHeight();

		m_glwidget->configVisbox(width, height);
	}
}

void MainWindow::tilemapConfig()
{
	if (m_tilemap_widget->exec() == QDialog::Accepted)
	{
		int xstart = m_tilemap_widget->getXStart();
		int xend = m_tilemap_widget->getXEnd();
		int ystart = m_tilemap_widget->getYStart();
		int yend = m_tilemap_widget->getYEnd();
	}
}

void MainWindow::levelConfig()
{
	if (m_level_conf->exec() == QDialog::Accepted)
	{
		// TODO: set level stuff
	}
}

void MainWindow::toggleGrid()
{
	if (m_enable_grid)
		m_enable_grid = false;
	else
		m_enable_grid = true;

	m_glwidget->enableGrid(m_enable_grid);
}

void MainWindow::snapGrid()
{
	if (m_snap_grid)
		m_snap_grid = false;
	else
		m_snap_grid = true;

	m_glwidget->setSnapGrid(m_snap_grid);
}

void MainWindow::enableFilters(int filter)
{
	emit m_objbrowser->enableFilter(filter);
	emit m_glwidget->enableFilter(filter);
}

void MainWindow::disableFilters(int filter)
{
	emit m_objbrowser->disableFilter(filter);
	emit m_glwidget->disableFilter(filter);
}

void MainWindow::setDefType(int type)
{
	switch (type)
	{
		case 0:		emit m_objedit->setDefaultType(Level::OBJECT_TYPE_COLLISION); break;
		case 1:		emit m_objedit->setDefaultType(Level::OBJECT_TYPE_GEOVIS); break;
		case 2:		emit m_objedit->setDefaultType(Level::OBJECT_TYPE_TRIGGER); break;
		case 3:		emit m_objedit->setDefaultType(Level::OBJECT_TYPE_SLIDER); break;
		case 4:		emit m_objedit->setDefaultType(Level::OBJECT_TYPE_DESTRUCTIBLE); break;
		case 5:		emit m_objedit->setDefaultType(Level::OBJECT_TYPE_MOVER); break;
	}
}

void MainWindow::setPrefabScale(double scale)
{
	m_prefabScaleSpin->setValue(scale);
}

void MainWindow::prefabScaleChange(double value)
{
	emit m_glwidget->setPrefabScale(value);
}

void MainWindow::prefabDepthChange(int value)
{
	emit m_glwidget->setPrefabDepth(value);
}

void MainWindow::setPrefabAlign(int align)
{
	switch (align)
	{
		case 0:		emit m_glwidget->setPrefabAlign(0); break;
		case 1:		emit m_glwidget->setPrefabAlign(1); break;
		case 2:		emit m_glwidget->setPrefabAlign(2); break;
		case 3:		emit m_glwidget->setPrefabAlign(3); break;
	}
}

void MainWindow::changeTexture(QString path)
{
	m_texture->load(path);
	m_texture_file = path;

	m_glwidget->loadTexture(m_texture);
	m_texedit->setTexture(m_texture);
	m_objdesigner->setTexture(m_texture);
	m_objprefabs->setTexture(m_texture);
	m_tileset_window->setTexture(m_texture);
}


void MainWindow::setBGColor()
{
	QColor result = QColorDialog::getColor(m_bgcolor, this, tr("Select background color"));
	if (result.isValid())
	{
		m_bgcolor = result;
		emit m_glwidget->setBGColor(m_bgcolor);
	}
}


void MainWindow::tilemapConfigChange()
{
	int xstart = m_tilemap_widget->getXStart();
	int xend = m_tilemap_widget->getXEnd();
	int ystart = m_tilemap_widget->getYStart();
	int yend = m_tilemap_widget->getYEnd();

	float size = m_tilemap_widget->getSize();

	emit m_glwidget->setTilemapConfig(xstart, xend, ystart, yend, size);
	m_level->resizeTilemap(xstart, xend, ystart, yend, size);
}


void MainWindow::edgify()
{
	FILE *fout = fopen("edgifylog.txt", "wt");

	vector<Level::Object*> coll;

	int numobjs = m_level->numObjects();
	for (int i=0; i < numobjs; i++)
	{
		Level::Object* o = m_level->getObject(i);
		if (o->getType() == Level::OBJECT_TYPE_COLLISION)
		{
			coll.push_back(o);
		}
	}


	for (int i=0; i < coll.size(); i++)
	{
		Level::Object* current = coll.at(i);
		int current_id = current->getId();

		glm::vec2 current_max = current->getBoundingMax();
		glm::vec2 current_min = current->getBoundingMin();

		fprintf(fout, "Object %d: ", current_id);

		for (int j=0; j < coll.size(); j++)
		{
			Level::Object* other = coll.at(j);
			int other_id = other->getId();

			if (current != other)
			{
				glm::vec2 other_max = other->getBoundingMax();
				glm::vec2 other_min = other->getBoundingMin();

				if (current_min.x == other_max.x && other_max.y >= current_min.y && other_min.y <= current_max.y)
				{
					fprintf(fout, "%d left, ", other_id);
				}
				else if (current_max.x == other_min.x && other_max.y >= current_min.y && other_min.y <= current_max.y)
				{
					fprintf(fout, "%d right, ", other_id);
				}
				else if (current_min.y == other_max.y && other_max.x >= current_min.x && other_min.x <= current_max.x)
				{
					fprintf(fout, "%d top, ", other_id);
				}
				else if (current_max.y == other_min.y && other_max.x >= current_min.x && other_min.x <= current_max.x)
				{
					fprintf(fout, "%d bottom, ", other_id);
				}


			}
		}

		fprintf(fout, "\n");
	}


	fprintf(fout, "collision objects: %d\n", coll.size());

	fclose(fout);
}



void MainWindow::createActions()
{
	// file menu
	m_newAction = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
	m_newAction->setShortcuts(QKeySequence::New);
	m_newAction->setStatusTip(tr("Create a new file"));
	connect(m_newAction, SIGNAL(triggered()), this, SLOT(newFile()));

	m_openAction = new QAction(QIcon(":/images/open.png"), tr("&Open"), this);
	m_openAction->setShortcuts(QKeySequence::Open);
	m_openAction->setStatusTip(tr("Open an existing file"));
	connect(m_openAction, SIGNAL(triggered()), this, SLOT(openFile()));

	m_openLegacyAction = new QAction(QIcon(":/images/open.png"), tr("&Open Legacy"), this);
	m_openLegacyAction->setStatusTip(tr("Open an existing legacy file"));
	connect(m_openLegacyAction, SIGNAL(triggered()), this, SLOT(openLegacyFile()));

	m_saveAction = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
	m_saveAction->setShortcuts(QKeySequence::Save);
	m_saveAction->setStatusTip(tr("Save file"));
	connect(m_saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));

	m_saveAsAction = new QAction(QIcon(":/images/saveas.png"), tr("Save &as"), this);
	m_saveAsAction->setShortcuts(QKeySequence::SaveAs);
	m_saveAsAction->setStatusTip(tr("Save as new file"));
	connect(m_saveAsAction, SIGNAL(triggered()), this, SLOT(saveAsFile()));

	m_exportAction = new QAction(tr("Export Level"), this);
	m_exportAction->setStatusTip(tr("Export Level"));
	connect(m_exportAction, SIGNAL(triggered()), this, SLOT(exportLevel()));

	m_exitAction = new QAction(QIcon(":/images/exit.png"), tr("&Exit"), this);
	m_exitAction->setShortcuts(QKeySequence::Quit);
	m_exitAction->setStatusTip(tr("Exit the application"));
	connect(m_exitAction, SIGNAL(triggered()), this, SLOT(close()));


	// edit menu
	m_copyAction = new QAction(tr("Copy"), this);
	m_copyAction->setShortcut(Qt::Key_C + Qt::CTRL);
	connect(m_copyAction, SIGNAL(triggered()), m_glwidget, SLOT(copy()));

	m_pasteAction = new QAction(tr("Paste"), this);
	m_pasteAction->setShortcut(Qt::Key_V + Qt::CTRL);
	connect(m_pasteAction, SIGNAL(triggered()), m_glwidget, SLOT(paste()));

	m_toggleGridAction = new QAction(QIcon("grid.png"), tr("Enable grid"), this);
	m_toggleGridAction->setCheckable(true);
	m_toggleGridAction->setShortcut(Qt::Key_G);
	connect(m_toggleGridAction, SIGNAL(triggered()), this, SLOT(toggleGrid()));

	m_snapGridAction = new QAction(QIcon("snapgrid.png"), tr("Snap to grid"), this);
	m_snapGridAction->setCheckable(true);
	m_snapGridAction->setShortcut(Qt::Key_H);
	connect(m_snapGridAction, SIGNAL(triggered()), this, SLOT(snapGrid()));

	m_bgColorAction = new QAction(tr("Select background color"), this);
	connect(m_bgColorAction, SIGNAL(triggered()), this, SLOT(setBGColor()));

	m_levelConfAction = new QAction(tr("Level settings"), this);
	connect(m_levelConfAction, SIGNAL(triggered()), this, SLOT(levelConfig()));


	m_gridSizeCombo = new QComboBox();
	for (int i=0; i < GLWidget::NUM_GRID_SIZES; i++)
	{
		m_gridSizeCombo->addItem(tr("%1").arg(GLWidget::GRID_SIZE[i], 0, 'f', 2));
	}
	connect(m_gridSizeCombo, SIGNAL(activated(int)), this, SLOT(setGridSize(int)));
	m_gridSizeLabel = new QLabel("Grid:");
	m_gridSizeLabel->setMinimumWidth(30);
	QBoxLayout* gridsize_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	gridsize_layout->setSpacing(2);
	gridsize_layout->setMargin(1);
	gridsize_layout->addWidget(m_gridSizeLabel);
	gridsize_layout->addWidget(m_gridSizeCombo);
	m_gridSizeWidget = new QWidget;
	m_gridSizeWidget->setMaximumHeight(30);
	m_gridSizeWidget->setLayout(gridsize_layout);


	// texture menu
	m_loadTexAction = new QAction(tr("Load Texture"), this);
	m_loadTexAction->setStatusTip(tr("Load new texture"));
	connect(m_loadTexAction, SIGNAL(triggered()), this, SLOT(loadTexture()));


	// prefab menu
	m_loadPrefabsAction = new QAction(tr("Load Prefabs"), this);
	m_loadPrefabsAction->setStatusTip(tr("Load prefabs"));
	connect(m_loadPrefabsAction, SIGNAL(triggered()), this, SLOT(loadPrefabs()));

	m_savePrefabsAction = new QAction(tr("Save Prefabs"), this);
	m_savePrefabsAction->setStatusTip(tr("Save prefabs"));
	connect(m_savePrefabsAction, SIGNAL(triggered()), this, SLOT(savePrefabs()));



	// operation tools
	m_opgroup = new QActionGroup(this);

	m_select_action = new QAction(QIcon("select.png"), tr("Select and edit object"), this);
	m_select_action->setCheckable(true);
	m_select_action->setShortcut(Qt::Key_S);
	connect(m_select_action, SIGNAL(triggered()), this, SLOT(selectionMode()));

	m_move_action = new QAction(QIcon("move.png"), tr("Move object"), this);
	m_move_action->setCheckable(true);
	m_move_action->setShortcut(Qt::Key_M);
	connect(m_move_action, SIGNAL(triggered()), this, SLOT(moveMode()));

	m_rotate_action = new QAction(QIcon("rotate.png"), tr("Rotate object"), this);
	m_rotate_action->setCheckable(true);
	connect(m_rotate_action, SIGNAL(triggered()), this, SLOT(rotateMode()));

	m_scale_action = new QAction(QIcon("scale.png"), tr("Scale object"), this);
	m_scale_action->setCheckable(true);
	connect(m_scale_action, SIGNAL(triggered()), this, SLOT(scaleMode()));

	m_draw_poly_action = new QAction(QIcon("polygon.png"), tr("Draw new polygon object"), this);
	m_draw_poly_action->setCheckable(true);
	m_draw_poly_action->setShortcut(Qt::Key_T);
	connect(m_draw_poly_action, SIGNAL(triggered()), this, SLOT(drawPolyMode()));

	m_draw_rect_action = new QAction(QIcon("rect.png"), tr("Draw new rectangle object"), this);
	m_draw_rect_action->setCheckable(true);
	m_draw_rect_action->setShortcut(Qt::Key_R);
	connect(m_draw_rect_action, SIGNAL(triggered()), this, SLOT(drawRectMode()));

	m_prefab_action = new QAction(QIcon("prefab.png"), tr("Draw a prefab object"), this);
	m_prefab_action->setCheckable(true);
	m_prefab_action->setShortcut(Qt::Key_P);
	connect(m_prefab_action, SIGNAL(triggered()), this, SLOT(prefabMode()));

	m_tiledprefab_action = new QAction(QIcon("prefab.png"), tr("Draw a tiled prefab"), this);
	m_tiledprefab_action->setCheckable(true);
	connect(m_tiledprefab_action, SIGNAL(triggered()), this, SLOT(tiledprefabMode()));

	m_tilemap_action = new QAction(QIcon("tilemap.png"), tr("Draw on tilemap"), this);
	m_tilemap_action->setCheckable(true);
	connect(m_tilemap_action, SIGNAL(triggered()), this, SLOT(tilemapMode()));

	m_opgroup->addAction(m_select_action);
	m_opgroup->addAction(m_move_action);
	m_opgroup->addAction(m_rotate_action);
	m_opgroup->addAction(m_scale_action);
	m_opgroup->addAction(m_draw_poly_action);
	m_opgroup->addAction(m_draw_rect_action);
	m_opgroup->addAction(m_prefab_action);
	m_opgroup->addAction(m_tiledprefab_action);
	m_opgroup->addAction(m_tilemap_action);


	// editor tools
	m_toggle_texedit = new QAction(QIcon("texture.png"), tr("Toggle Texture Editor"), this);
	m_toggle_texedit->setCheckable(true);
	connect(m_toggle_texedit, SIGNAL(triggered()), this, SLOT(toggleTexEdit()));

	m_toggle_objedit = new QAction(QIcon("objedit.png"), tr("Toggle Object Editor"), this);
	m_toggle_objedit->setCheckable(true);
	connect(m_toggle_objedit, SIGNAL(triggered()), this, SLOT(toggleObjEdit()));

	m_toggle_objfilter = new QAction(QIcon("objfilter.png"), tr("Toggle Object Filter"), this);
	m_toggle_objfilter->setCheckable(true);
	connect(m_toggle_objfilter, SIGNAL(triggered()), this, SLOT(toggleObjFilter()));

	m_toggle_objdesigner = new QAction(QIcon("objfilter.png"), tr("Toggle Object Designer"), this);
	m_toggle_objdesigner->setCheckable(true);
	connect(m_toggle_objdesigner, SIGNAL(triggered()), this, SLOT(toggleObjDesigner()));

	m_toggle_tileset_window = new QAction(QIcon("tilemap.png"), tr("Toggle Tileset"), this);
	m_toggle_tileset_window->setCheckable(true);
	connect(m_toggle_tileset_window, SIGNAL(triggered()), this, SLOT(toggleTilesetWindow()));

	m_toggle_tiledprefabs_window = new QAction(QIcon("texture.png"), tr("Toggle Tiled Prefabs"), this);
	m_toggle_tiledprefabs_window->setCheckable(true);
	connect(m_toggle_tiledprefabs_window, SIGNAL(triggered()), this, SLOT(toggleTiledprefabsWindow()));


	// visbox
	m_toggle_visbox = new QAction(QIcon("visbox.png"), tr("Toggle Visualization Box"), this);
	m_toggle_visbox->setCheckable(true);
	m_toggle_visbox->setShortcut(Qt::Key_Period);
	connect(m_toggle_visbox, SIGNAL(triggered()), this, SLOT(toggleVisbox()));

	m_visbox_conf_action = new QAction(QIcon("visboxconf.png"), tr("Visualization Box Configuration"), this);
	connect(m_visbox_conf_action, SIGNAL(triggered()), this, SLOT(visboxConfig()));


	// tilemap settings
	m_tilemap_widget_action = new QAction(QIcon("texture.png"), tr("Tilemap Settings"), this);
	connect(m_tilemap_widget_action, SIGNAL(triggered()), this, SLOT(tilemapConfig()));


	// edgify
	m_edgify_action = new QAction(tr("Edgify"), this);
	connect(m_edgify_action, SIGNAL(triggered()), this, SLOT(edgify()));


	// zoom
	m_zoomLevelCombo = new QComboBox();
	for (int i=0; i < GLWidget::NUM_ZOOM_LEVELS; i++)
	{
		m_zoomLevelCombo->addItem(tr("%1%").arg((int)(GLWidget::ZOOM_LEVELS[i] * 100.0f)));
	}
	connect(m_zoomLevelCombo, SIGNAL(activated(int)), this, SLOT(setZoomLevel(int)));
	m_zoomLevelLabel = new QLabel("Zoom:");
	m_zoomLevelLabel->setMinimumWidth(30);
	QBoxLayout* zoomlevel_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	zoomlevel_layout->setSpacing(2);
	zoomlevel_layout->setMargin(1);
	zoomlevel_layout->addWidget(m_zoomLevelLabel);
	zoomlevel_layout->addWidget(m_zoomLevelCombo);
	m_zoomLevelWidget = new QWidget;
	m_zoomLevelWidget->setMaximumHeight(30);
	m_zoomLevelWidget->setLayout(zoomlevel_layout);


	// default type
	m_defTypeCombo = new QComboBox();
	m_defTypeCombo->addItem("Collision");
	m_defTypeCombo->addItem("Geometry");
	m_defTypeCombo->addItem("Trigger");
	m_defTypeCombo->addItem("Slider");
	m_defTypeCombo->addItem("Destructible");
	m_defTypeCombo->addItem("Mover");
	connect(m_defTypeCombo, SIGNAL(activated(int)), this, SLOT(setDefType(int)));
	m_defTypeLabel = new QLabel("Default Type:");
	m_defTypeLabel->setMinimumWidth(50);
	QBoxLayout* deftype_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	deftype_layout->setSpacing(2);
	deftype_layout->setMargin(1);
	deftype_layout->addWidget(m_defTypeLabel);
	deftype_layout->addWidget(m_defTypeCombo);
	m_defTypeWidget = new QWidget;
	m_defTypeWidget->setMaximumHeight(30);
	m_defTypeWidget->setLayout(deftype_layout);


	// prefab scale
	m_prefabScaleSpin = new QDoubleSpinBox();
	m_prefabScaleSpin->setDecimals(2);
	m_prefabScaleSpin->setRange(0.5, 1000.0);
	m_prefabScaleSpin->setSingleStep(0.1);
	connect(m_prefabScaleSpin, SIGNAL(valueChanged(double)), this, SLOT(prefabScaleChange(double)));
	m_prefabScaleLabel = new QLabel("Prefab Scale:");
	m_prefabScaleLabel->setMinimumWidth(50);
	QBoxLayout* prefab_scale_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	prefab_scale_layout->setSpacing(2);
	prefab_scale_layout->setMargin(1);
	prefab_scale_layout->addWidget(m_prefabScaleLabel);
	prefab_scale_layout->addWidget(m_prefabScaleSpin);
	m_prefabScaleWidget = new QWidget();
	m_prefabScaleWidget->setMaximumHeight(30);
	m_prefabScaleWidget->setLayout(prefab_scale_layout);

	// prefab depth
	m_prefabDepthSpin = new QSpinBox();
	m_prefabDepthSpin->setRange(0, 100);
	m_prefabDepthSpin->setSingleStep(1);
	connect(m_prefabDepthSpin, SIGNAL(valueChanged(int)), this, SLOT(prefabDepthChange(int)));
	m_prefabDepthLabel = new QLabel("Prefab Depth:");
	m_prefabDepthLabel->setMinimumWidth(50);
	QBoxLayout* prefab_depth_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	prefab_depth_layout->setSpacing(2);
	prefab_depth_layout->setMargin(1);
	prefab_depth_layout->addWidget(m_prefabDepthLabel);
	prefab_depth_layout->addWidget(m_prefabDepthSpin);
	m_prefabDepthWidget = new QWidget();
	m_prefabDepthWidget->setMaximumHeight(30);
	m_prefabDepthWidget->setLayout(prefab_depth_layout);

	// prefab align
	m_prefabAlignCombo = new QComboBox();
	m_prefabAlignCombo->addItem("Top Left");
	m_prefabAlignCombo->addItem("Bottom Left");
	m_prefabAlignCombo->addItem("Top Right");
	m_prefabAlignCombo->addItem("Bottom Right");
	connect(m_prefabAlignCombo, SIGNAL(activated(int)), this, SLOT(setPrefabAlign(int)));
	m_prefabAlignLabel = new QLabel("Prefab Align");
	m_prefabAlignLabel->setMinimumWidth(50);
	QBoxLayout* prefabalign_layout = new QBoxLayout(QBoxLayout::LeftToRight);
	prefabalign_layout->setSpacing(2);
	prefabalign_layout->setMargin(1);
	prefabalign_layout->addWidget(m_prefabAlignLabel);
	prefabalign_layout->addWidget(m_prefabAlignCombo);
	m_prefabAlignWidget = new QWidget;
	m_prefabAlignWidget->setMaximumHeight(30);
	m_prefabAlignWidget->setLayout(prefabalign_layout);

}

void MainWindow::createMenus()
{
	// file menu
	m_fileMenu = menuBar()->addMenu(tr("&File"));
	m_fileMenu->addAction(m_newAction);
	m_fileMenu->addAction(m_openAction);
	m_fileMenu->addAction(m_openLegacyAction);
	m_fileMenu->addAction(m_saveAction);
	m_fileMenu->addAction(m_saveAsAction);
	m_fileMenu->addSeparator();
	m_fileMenu->addAction(m_exportAction);
	m_fileMenu->addSeparator();
	m_fileMenu->addAction(m_exitAction);

	// edit menu
	m_editMenu = menuBar()->addMenu(tr("&Edit"));
	m_editMenu->addAction(m_copyAction);
	m_editMenu->addAction(m_pasteAction);
	m_editMenu->addSeparator();
	m_editMenu->addAction(m_toggleGridAction);
	m_editMenu->addAction(m_snapGridAction);
	m_editMenu->addSeparator();
	m_editMenu->addAction(m_bgColorAction);
	m_editMenu->addAction(m_levelConfAction);
	m_editMenu->addSeparator();
	m_editMenu->addAction(m_edgify_action);

	// texture menu
	m_textureMenu = menuBar()->addMenu(tr("&Texture"));
	m_textureMenu->addAction(m_loadTexAction);

	// prefab menu
	m_prefabMenu = menuBar()->addMenu(tr("Prefab"));
	m_prefabMenu->addAction(m_loadPrefabsAction);
	m_prefabMenu->addAction(m_savePrefabsAction);
}

void MainWindow::createToolbars()
{
	m_file_toolbar = addToolBar("File");
	m_file_toolbar->addAction(m_newAction);
	m_file_toolbar->addAction(m_openAction);
	m_file_toolbar->addAction(m_saveAction);
	m_file_toolbar->addAction(m_saveAsAction);

	m_op_toolbar = addToolBar("Operation");
	m_op_toolbar->addAction(m_select_action);
	m_op_toolbar->addAction(m_move_action);
	m_op_toolbar->addAction(m_rotate_action);
	m_op_toolbar->addAction(m_scale_action);
	m_op_toolbar->addAction(m_draw_poly_action);
	m_op_toolbar->addAction(m_draw_rect_action);
	m_op_toolbar->addAction(m_prefab_action);
	m_op_toolbar->addAction(m_tiledprefab_action);
	m_op_toolbar->addAction(m_tilemap_action);

	m_grid_toolbar = addToolBar("Grid");
	m_grid_toolbar->addAction(m_toggleGridAction);
	m_grid_toolbar->addAction(m_snapGridAction);
	m_grid_toolbar->addWidget(m_gridSizeWidget);

	m_editor_toolbar = addToolBar("Editors");
	m_editor_toolbar->addAction(m_toggle_texedit);
	m_editor_toolbar->addAction(m_toggle_objedit);
	m_editor_toolbar->addAction(m_toggle_objfilter);
	m_editor_toolbar->addAction(m_toggle_objdesigner);
	m_editor_toolbar->addAction(m_toggle_tileset_window);
	m_editor_toolbar->addAction(m_toggle_tiledprefabs_window);
	m_editor_toolbar->addAction(m_tilemap_widget_action);

	m_visbox_toolbar = addToolBar("Visualization Box");
	m_visbox_toolbar->addAction(m_toggle_visbox);
	m_visbox_toolbar->addAction(m_visbox_conf_action);

	m_zoom_toolbar = addToolBar("Zoom");
	m_zoom_toolbar->addWidget(m_zoomLevelWidget);

	m_deftype_toolbar = addToolBar("Default Type");
	m_deftype_toolbar->addWidget(m_defTypeWidget);
	m_deftype_toolbar->addWidget(m_prefabScaleWidget);
	m_deftype_toolbar->addWidget(m_prefabDepthWidget);
	m_deftype_toolbar->addWidget(m_prefabAlignWidget);
}

void MainWindow::createStatusbar()
{

}

void MainWindow::setCurrentFile(QString& filename)
{
	QString filetitle;

	if (filename.length() <= 0)
	{
		filetitle = tr("Untitled");
	}
	else
	{
		filetitle = filename;
	}

	m_open_file = filename;

	setWindowFilePath(tr("%1 - WorldEdit").arg(filetitle));
}

void MainWindow::reset()
{
	m_level->reset();
	m_level->resetModify();

	emit m_objprefabs->reset();
	emit m_objbrowser->reset();
	emit m_glwidget->deselect();
	emit m_objbrowser->deselect();

	emit m_glwidget->reset();

	emit m_tileset_window->reset();
}

void MainWindow::newDocument()
{
	reset();
	selectionMode();

	setCurrentFile(tr(""));

	// load default texture
	changeTexture("blue_checker.png");
}

void MainWindow::resetControls()
{
}

void MainWindow::setGridSize(int size)
{
	emit m_glwidget->setGridSize(size);
}

void MainWindow::setZoomLevel(int zoom)
{
	emit m_glwidget->setZoomLevel(zoom);
}

void MainWindow::zoomIn()
{
	int current = m_zoomLevelCombo->currentIndex();
	current++;
	if (current >= m_zoomLevelCombo->count())
		current = m_zoomLevelCombo->count() - 1;

	m_zoomLevelCombo->setCurrentIndex(current);
	setZoomLevel(current);
}

void MainWindow::zoomOut()
{
	int current = m_zoomLevelCombo->currentIndex();
	current--;
	if (current < 0)
		current = 0;

	m_zoomLevelCombo->setCurrentIndex(current);
	setZoomLevel(current);
}

void MainWindow::loadTexture()
{
	QString filename = QFileDialog::getOpenFileName(this,
													tr("Load Texture"),
													getTextureDir(),
													tr("Texture File (*.png);;All Files (*.*)"));

	if (!filename.isEmpty())
	{
		changeTexture(filename);

		// set current directory
		setTextureDir(filename);
	}
}

void MainWindow::loadPrefabs()
{
	QString filename = QFileDialog::getOpenFileName(this,
													tr("Load Prefabs"),
													getLevelDir(),
													tr("Prefab File (*.prefab.xml);;All Files (*.*)"));

	if (!filename.isEmpty())
	{
		vector<glm::vec4> points;
		glm::vec4 current_point;
		QString texture_name = "";
		QString prefab_name = "";

		points.clear();
		m_objprefabs->reset();

		QFile* file = new QFile(filename);
		file->open(QIODevice::ReadOnly);

		QXmlStreamReader input(file);

		while (!input.atEnd())
		{
			input.readNext();

			if (input.isStartElement())
			{
				QString element = input.name().toString();
				QXmlStreamAttributes attrs = input.attributes();

				if (element == "prefab")
				{
					prefab_name = "";
					
					int num_attrs = attrs.size();
					for (int i=0; i < num_attrs; i++)
					{
						QString attr_name = attrs[i].name().toString();
						if (attr_name == "name")
						{
							prefab_name = attrs[i].value().toString();
						}
					}

					points.clear();
				}			
				else if (element == "point")
				{
					int num_attrs = attrs.size();
					for (int i=0; i < num_attrs; i++)
					{
						QString attr_name = attrs[i].name().toString();
						float value = attrs[i].value().toFloat();
						if (attr_name == "x")
							current_point.x = value;
						else if (attr_name == "y")
							current_point.y = value;
						else if (attr_name == "u")
							current_point.z = value;
						else if (attr_name == "v")
							current_point.w = value;
					}
				}
				else if (element == "texture")
				{
					int num_attrs = attrs.size();
					for (int i=0; i < num_attrs; i++)
					{
						QString attr_name = attrs[i].name().toString();
						if (attr_name == "src")
						{
							texture_name = attrs[i].value().toString();
						}
					}
				}
			}
			else if (input.isEndElement())
			{
				QString element = input.name().toString();
				if (element == "prefab")
				{
					glm::vec4 pps[8];
					std::string name = prefab_name.toStdString();

					int num_points = points.size();
					for (int i=0; i < num_points; i++)
					{
						pps[i] = points[i];
					}

					int id = m_level->insertPrefab(name, pps, num_points);
					emit m_objprefabs->add(id);

					points.clear();
				}
				else if (element == "point")
				{
					points.push_back(current_point);
					current_point = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
				}
			}
		}

		file->close();
		delete [] file;

		// load texture if needed
		if (!texture_name.isEmpty())
		{
			if (texture_name != m_texture_file)
			{
				QMessageBox box;
				box.setText("Different texture.");
				box.setInformativeText("The prefab file uses a different texture. Want to load this texture?");
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setDefaultButton(QMessageBox::Yes);
				int ret = box.exec();

				if (ret == QMessageBox::Yes)
				{
					changeTexture(texture_name);
				}
			}
		}		
	}
}

void MainWindow::savePrefabs()
{
	QString filename = QFileDialog::getSaveFileName(this,
													tr("Save Prefabs"),
													getLevelDir(),
													tr("Prefab File (*.prefab.xml);;All Files (*.*)"));

	if (!filename.isEmpty())
	{
		QFile* file = new QFile(filename);
		file->open(QIODevice::WriteOnly);

		QXmlStreamWriter output(file);
		output.setAutoFormatting(true);

		// start document
		output.writeStartDocument();

		output.writeStartElement("prefabs");			// <prefabs>

		// texture
		output.writeStartElement("texture");			// <texture>
		output.writeAttribute("src", m_texture_file);
		output.writeEndElement();					// </texture>

		// prefabs
		int num_prefabs = m_level->getNumPrefabs();
		for (int i=0; i < num_prefabs; i++)
		{
			const Level::Prefab* prefab = m_level->getPrefab(i);

			output.writeStartElement("prefab");		// <prefab>
			output.writeAttribute("name", tr("%1").arg(prefab->name.c_str()));

			for (int p=0; p < prefab->num_points; p++)
			{
				output.writeStartElement("point");		// <point>
				output.writeAttribute("x", tr("%1").arg(prefab->points[p].x, 0, 'f', 10));
				output.writeAttribute("y", tr("%1").arg(prefab->points[p].y, 0, 'f', 10));
				output.writeAttribute("u", tr("%1").arg(prefab->points[p].z, 0, 'f', 10));
				output.writeAttribute("v", tr("%1").arg(prefab->points[p].w, 0, 'f', 10));
				output.writeEndElement();				// </point>
			}
			
			output.writeEndElement();				// </prefab>
		}

		output.writeEndElement();				// </prefabs>

		output.writeEndDocument();
		
		file->close();
		delete file;
	}
}


void MainWindow::readConfigFile(QString& filename)
{
	QFile* file = new QFile(filename);
	file->open(QIODevice::ReadOnly);

	QXmlStreamReader input(file);

	vector<std::string> trigger_list;
	std::string current_name;

	while (!input.atEnd())
	{
		input.readNext();

		if (input.isStartElement())
		{
			QString element = input.name().toString();
			QXmlStreamAttributes attrs = input.attributes();
			int num_attrs = attrs.size();

			if (element == "triggers")
			{
				trigger_list.clear();
			}
			else if (element == "trigger")
			{
				for (int i=0; i < num_attrs; i++)
				{
					QString attr_name = attrs[i].name().toString();

					if (attr_name == "name")
					{
						current_name = attrs[i].value().toString().toStdString();
					}
				}
			}
		}
		else if (input.isEndElement())
		{
			QString element = input.name().toString();

			if (element == "trigger")
			{
				trigger_list.push_back(current_name);
			}
		}
	};

	m_level->setTriggerList(trigger_list);
}


bool MainWindow::readLevelFile(QString& filename)
{
	struct Point
	{
		glm::vec2 position;
		glm::vec2 uv;
	};

	std::vector<Point> points;
	Point current_point;
	QString object_name;
	int object_id;
	int object_type;
	int object_z;
	QString texture_name = "";
	Level::Object::Param params[Level::Object::NUM_PARAMS];
	int current_param;
	QString prefab_name;


	points.clear();

	reset();


	QFile* file = new QFile(filename);
	file->open(QIODevice::ReadOnly);

	QXmlStreamReader input(file);

	while (!input.atEnd() && !input.hasError())
	{
		input.readNext();

		if (input.isStartElement())
		{
			QString element = input.name().toString();
			QXmlStreamAttributes attrs = input.attributes();

			if (element == "object")
			{
				object_name = "";
				object_id = 0;
				object_type = 0;
				object_z = 0;

				int num_attrs = attrs.size();
				for (int i=0; i < num_attrs; i++)
				{
					QString attr_name = attrs[i].name().toString();
					
					if (attr_name == "name")
					{
						object_name = attrs[i].value().toString();
					}
					else if (attr_name == "id")
					{
						object_id = attrs[i].value().toInt();
					}
					else if (attr_name == "type")
					{
						object_type = attrs[i].value().toInt();
					}
					else if (attr_name == "z")
					{
						object_z = attrs[i].value().toInt();
					}
				}

				points.clear();

				current_param = 0;
			}
			else if (element == "point")
			{
				int num_attrs = attrs.size();
				for (int i=0; i < num_attrs; i++)
				{
					QString attr_name = attrs[i].name().toString();
					float value = attrs[i].value().toFloat();
					if (attr_name == "x")
					{
						current_point.position.x = value;
					}
					else if (attr_name == "y")
					{
						current_point.position.y = value;
					}
					else if (attr_name == "u")
					{
						current_point.uv.x = value;
					}
					else if (attr_name == "v")
					{
						current_point.uv.y = value;
					}
				}
			}
			else if (element == "texture")
			{
				int num_attrs = attrs.size();
				for (int i=0; i < num_attrs; i++)
				{
					QString attr_name = attrs[i].name().toString();
					if (attr_name == "src")
					{
						texture_name = attrs[i].value().toString();
					}
				}
			}
			else if (element == "param")
			{
				int num_attrs = attrs.size();
				for (int i=0; i < num_attrs; i++)
				{
					QString attr_name = attrs[i].name().toString();
					if (attr_name == "value")
					{
						assert(current_param >= 0 && current_param < Level::Object::NUM_PARAMS);
						params[current_param].i = attrs[i].value().toInt();
						current_param++;
					}
				}
			}
			else if (element == "prefab")
			{
				prefab_name = "";

				int num_attrs = attrs.size();
				for (int i=0; i < num_attrs; i++)
				{
					QString attr_name = attrs[i].name().toString();
					if (attr_name == "name")
					{
						prefab_name = attrs[i].value().toString();
					}
				}
			}
		}
		else if (input.isEndElement())
		{
			QString element = input.name().toString();

			if (element == "object")
			{
				glm::vec2 pps[8];
				glm::vec2 uvs[8];

				int num_points = points.size();
				for (int i=0; i < num_points; i++)
				{
					pps[i] = points[i].position;
					uvs[i] = points[i].uv;
				}

				int id = m_level->insertObject(pps, uvs, num_points, (Level::ObjectType)object_type, object_name.toStdString());
				emit m_objbrowser->add(id);

				// set params
				Level::Object* obj = m_level->getObjectById(id);				
				for (int i=0; i < Level::Object::NUM_PARAMS; i++)
				{
					obj->setParam(i, params[i]);
				}
				
				// set z
				obj->setZ(object_z);

				points.clear();

				current_param = 0;
			}
			else if (element == "point")
			{
				points.push_back(current_point);

				current_point.position = glm::vec2(0,0);
				current_point.uv = glm::vec2(0,0);
			}
			else if (element == "prefab")
			{
				glm::vec4 pps[8];

				std::string name;

				if (prefab_name == "")
				{
					name = "Object";
				}
				else
				{
					name = prefab_name.toStdString();
				}

				int num_points = points.size();
				for (int i=0; i < num_points; i++)
				{
					pps[i].x = points[i].position.x;
					pps[i].y = points[i].position.y;
					pps[i].z = points[i].uv.x;
					pps[i].w = points[i].uv.y;
				}

				int id = m_level->insertPrefab(name, pps, num_points);
				emit m_objprefabs->add(id);

				points.clear();
			}
		}
	};

	file->close();
	delete [] file;

	// load texture
	if (!texture_name.isEmpty())
	{
		changeTexture(texture_name);
	}

	return true;
}


bool MainWindow::writeLevelFile(QString& filename)
{
	QFile* file = new QFile(filename);
	file->open(QIODevice::WriteOnly);

	QXmlStreamWriter output(file);
	output.setAutoFormatting(true);

	// start document
	output.writeStartDocument();

	output.writeStartElement("level");			// <level>

	// texture
	output.writeStartElement("texture");		// <texture>	
	output.writeAttribute("src", m_texture_file);
	output.writeEndElement();					// </texture>


	// prefabs
	int num_prefabs = m_level->getNumPrefabs();
	for (int i=0; i < num_prefabs; i++)
	{
		const Level::Prefab* prefab = m_level->getPrefab(i);

		output.writeStartElement("prefab");			// <prefab>
		output.writeAttribute("name", tr("%1").arg(prefab->name.c_str()));
		
		for (int p=0; p < prefab->num_points; p++)
		{
			output.writeStartElement("point");		// <point>
			output.writeAttribute("x", tr("%1").arg(prefab->points[p].x, 0, 'f', 10));
			output.writeAttribute("y", tr("%1").arg(prefab->points[p].y, 0, 'f', 10));
			output.writeAttribute("u", tr("%1").arg(prefab->points[p].z, 0, 'f', 10));
			output.writeAttribute("v", tr("%1").arg(prefab->points[p].w, 0, 'f', 10));
			output.writeEndElement();				// </point>
		}

		output.writeEndElement();					// </prefab>
	}


	// objects
	int num_objs = m_level->numObjects();
	for (int i=0; i < num_objs; i++)
	{
		Level::Object* obj = m_level->getObject(i);

		int id = obj->getId();
		std::string name = obj->getName();
		int type = obj->getType();
		float z = obj->getZ();

		int num_points = obj->getNumPoints();	// <object>

		output.writeStartElement("object");
		
		output.writeAttribute("name", tr("%1").arg(name.c_str()));
		output.writeAttribute("id", tr("%1").arg(id));
		output.writeAttribute("type", tr("%1").arg(type));
		output.writeAttribute("z", tr("%1").arg(z));

		for (int p=0; p < num_points; p++)
		{
			glm::vec2 point = obj->getPoint(p);
			glm::vec2 uv = obj->getUV(p);

			output.writeStartElement("point");	// <point>
			output.writeAttribute("x", tr("%1").arg(point.x, 0, 'f', 10));
			output.writeAttribute("y", tr("%1").arg(point.y, 0, 'f', 10));
			output.writeAttribute("u", tr("%1").arg(uv.x, 0, 'f', 10));
			output.writeAttribute("v", tr("%1").arg(uv.y, 0, 'f', 10));
			output.writeEndElement();			// </point>
		}

		for (int p=0; p < Level::Object::NUM_PARAMS; p++)
		{
			Level::Object::Param param = obj->getParam(p);
			output.writeStartElement("param");		// <param>
			output.writeAttribute("value", tr("%1").arg(param.i));
			output.writeEndElement();				// </param>
		}

		output.writeEndElement();				// </object>
	}

	output.writeEndElement();					// </level>

	output.writeEndDocument();

	file->close();
	delete file;

	return true;
}


bool MainWindow::readBinaryProjectFile(QString& filename)
{
	BinaryFile input;

	const unsigned int blpf_id = 0x424c5046;
	const unsigned int blpf_version = 0x10001;
	const unsigned int affix_id = 0x41465858;
	
	Level::Object::Param params[Level::Object::NUM_PARAMS];
	QString object_name = "";
	QString texture_name = "";
	QString prefab_name = "";
	QString tile_name = "";

	glm::vec4 points[8];

	reset();

	try
	{
		int inb, inp;
		char buf[200];
		input.open(filename.toStdString(), BinaryFile::MODE_READONLY);

		// ID
		unsigned int id = input.read_dword();
		if (id != blpf_id)
			throw "BLPF ID not found";

		// version
		unsigned int version = input.read_dword();
		if (version != blpf_version)
			throw "Wrong BLPF version";

		// texture name
		inb = 0;
		inp = 0;
		do
		{
			inb = input.read_byte();
			buf[inp++] = inb;
		} while (inb != 0);

		texture_name = QString(buf);


		// --------------------

		// num prefabs
		unsigned int num_prefabs = input.read_dword();

		// prefabs
		for (int i=0; i < num_prefabs; i++)
		{
			// prefab name
			inb = 0;
			inp = 0;
			do
			{
				inb = input.read_byte();
				buf[inp++] = inb;
			} while (inb != 0);

			prefab_name = QString(buf);

			int num_points = input.read_dword();
			for (int j=0; j < num_points; j++)
			{
				float x = input.read_float();
				float y = input.read_float();
				float u = input.read_float();
				float v = input.read_float();
				points[j] = glm::vec4(x, y, u, v);
			}

			std::string name = prefab_name.toStdString();

			int id = m_level->insertPrefab(name, points, num_points);
			emit m_objprefabs->add(id);
		}

		// ---------------------

		// num objects
		int num_objects = input.read_dword();

		// objects
		for (int i=0; i < num_objects; i++)
		{
			// object name
			inb = 0;
			inp = 0;
			do
			{
				inb = input.read_byte();
				buf[inp++] = inb;
			} while (inb != 0);

			object_name = QString(buf);

			int objid = input.read_dword();
			int objtype = input.read_dword();
			int objz = input.read_dword();

			glm::vec2 pps[8];
			glm::vec2 uvs[8];

			int num_points = input.read_dword();
			for (int j=0; j < num_points; j++)
			{
				float x = input.read_float();
				float y = input.read_float();
				float u = input.read_float();
				float v = input.read_float();
				
				pps[j] = glm::vec2(x, y);
				uvs[j] = glm::vec2(u, v);
			}
			for (int j=0; j < Level::Object::NUM_PARAMS; j++)
			{
				params[j].i = input.read_dword();
			}

			int id = m_level->insertObject(pps, uvs, num_points, (Level::ObjectType)objtype, object_name.toStdString());
			emit m_objbrowser->add(id);

			// set params
			Level::Object* obj = m_level->getObjectById(id);
			for (int j=0; j < Level::Object::NUM_PARAMS; j++)
			{
				obj->setParam(j, params[j]);
			}

			obj->setZ(objz);	
		}

		// ------------------

		// num of tiles
		int num_tiles = input.read_dword();

		// tiles
		for (int i=0; i < num_tiles; i++)
		{
			// tile name
			inb = 0;
			inp = 0;
			do
			{
				inb = input.read_byte();
				buf[inp++] = inb;
			} while (inb != 0);

			tile_name = QString(buf);

			int tileid = input.read_dword();

			glm::vec2 pps[4];
			for (int j=0; j < 4; j++)
			{
				float x = input.read_float();
				float y = input.read_float();
				
				pps[j] = glm::vec2(x, y);
			}

			int id = m_level->insertTile(tile_name.toStdString(), pps);
			emit m_tileset_window->add(id);
		}

		// ----------------

		int tilemap_xstart = input.read_dword();
		int tilemap_xend = input.read_dword();
		int tilemap_ystart = input.read_dword();
		int tilemap_yend = input.read_dword();
		int tilemap_tilesize = input.read_float();

		emit m_glwidget->setTilemapConfig(tilemap_xstart, tilemap_xend, tilemap_ystart, tilemap_yend, tilemap_tilesize);
		m_level->resizeTilemap(tilemap_xstart, tilemap_xend, tilemap_ystart, tilemap_yend, tilemap_tilesize);

		for (int y=tilemap_ystart; y < tilemap_yend; y++)
		{
			for (int x=tilemap_xstart; x < tilemap_xend; x++)
			{
				int data = input.read_dword();
				m_level->editTilemap(x, y, data);
			}
		}

		// ------------------------
		std::string col1_item;
		std::string col2_item;
		std::string col3_item;
		std::string dcol_item;

		// affix ID
		id = input.read_dword();
		if (id != affix_id)
			throw "AFFX ID not found";

		// time limit
		unsigned int tlim_enable = input.read_dword();
		unsigned int tlim = input.read_dword();

		// collect 1
		unsigned int col1_enable = input.read_dword();
		input.read_string(&col1_item);
		unsigned int col1_num = input.read_dword();

		// collect 2
		unsigned int col2_enable = input.read_dword();
		input.read_string(&col2_item);
		unsigned int col2_num = input.read_dword();

		// collect 3
		unsigned int col3_enable = input.read_dword();
		input.read_string(&col3_item);
		unsigned int col3_num = input.read_dword();

		// don't collect
		unsigned int dcol_enable = input.read_dword();
		input.read_string(&dcol_item);

		// avoid
		unsigned int avoid_enable = input.read_dword();

		// exit
		unsigned int exit_enable = input.read_dword();

		// inverse gravity
		unsigned int grav_enable = input.read_dword();


		m_level_conf->timelimitEnable(tlim_enable ? true : false);
		m_level_conf->setTimelimit(tlim);

		m_level_conf->collect1Enable(col1_enable ? true : false);
		m_level_conf->setCollect1Item(col1_item);
		m_level_conf->setCollect1Num(col1_num);

		m_level_conf->collect2Enable(col2_enable ? true : false);
		m_level_conf->setCollect2Item(col2_item);
		m_level_conf->setCollect2Num(col2_num);

		m_level_conf->collect3Enable(col3_enable ? true : false);
		m_level_conf->setCollect3Item(col3_item);
		m_level_conf->setCollect3Num(col3_num);

		m_level_conf->dontcollectEnable(dcol_enable ? true : false);
		m_level_conf->setDontcollectItem(dcol_item);

		m_level_conf->avoidEnable(avoid_enable ? true : false);
		m_level_conf->exitEnable(exit_enable ? true : false);
		m_level_conf->gravityEnable(grav_enable ? true : false);




		// ------------------------

		// load texture if needed
		if (!texture_name.isEmpty())
		{
			if (texture_name != m_texture_file)
			{
				QMessageBox box;
				box.setText("Different texture.");
				box.setInformativeText("The prefab file uses a different texture. Want to load this texture?");
				box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
				box.setDefaultButton(QMessageBox::Yes);
				int ret = box.exec();

				if (ret == QMessageBox::Yes)
				{
					changeTexture(texture_name);
				}
			}
		}	
	}
	catch (ios_base::failure&)
	{
		input.close();
		return false;
	}
	catch (int e)
	{
		input.close();
		return false;
	}

	return true;
}


bool MainWindow::writeBinaryProjectFile(QString& filename)
{
	BinaryFile output;

	const char blpf_id[4] = { 0x42, 0x4c, 0x50, 0x46 };
	const unsigned int blpf_version = 0x10001;

	const char affix_id[4] = { 0x41, 0x46, 0x58, 0x58 };

	int num_objects = m_level->numObjects();
	int num_prefabs = m_level->getNumPrefabs();

	try
	{
		output.open(filename.toStdString(), BinaryFile::MODE_WRITEONLY);

		// ID
		output.write((char*)blpf_id, 4);

		// version
		output.write_dword(blpf_version);

		// texture name
		QByteArray texname = m_texture_file.toLocal8Bit();
		for (int i=0; i < texname.length(); i++)
		{
			output.write_byte(texname.at(i));
		}
		output.write_byte(0);		// null terminator


		// num of prefabs
		output.write_dword(num_prefabs);

		// prefabs
		for (int i=0; i < num_prefabs; i++)
		{
			const Level::Prefab* prefab = m_level->getPrefab(i);

			// prefab name
			for (int j=0; j < prefab->name.length(); j++)
			{
				output.write_byte(prefab->name.at(j));
			}
			output.write_byte(0);	// null terminator

			// num points
			output.write_dword(prefab->num_points);

			// points
			for (int p=0; p < prefab->num_points; p++)
			{
				output.write_float(prefab->points[p].x);
				output.write_float(prefab->points[p].y);
				output.write_float(prefab->points[p].z);
				output.write_float(prefab->points[p].w);
			}
		}

		// num objects
		output.write_dword(num_objects);

		// objects
		for (int i=0; i < num_objects; i++)
		{
			Level::Object* object = m_level->getObject(i);

			int num_points = object->getNumPoints();

			// object name
			std::string name = object->getName();
			for (int j=0; j < name.length(); j++)
			{
				output.write_byte(name.at(j));
			}
			output.write_byte(0);	// null terminator

			// id
			output.write_dword(object->getId());

			// type
			output.write_dword(object->getType());

			// z
			output.write_dword(object->getZ());

			// num points
			output.write_dword(num_points);

			// points
			for (int p=0; p < num_points; p++)
			{
				glm::vec2 point = object->getPoint(p);
				glm::vec2 uv = object->getUV(p);
				
				output.write_float(point.x);
				output.write_float(point.y);
				output.write_float(uv.x);
				output.write_float(uv.y);
			}

			// params
			for (int p=0; p < Level::Object::NUM_PARAMS; p++)
			{
				Level::Object::Param param = object->getParam(p);
				output.write_dword(param.i);
			}
		}
		

		// tiles
		int num_tiles = m_level->getNumTiles();
		output.write_dword(num_tiles);

		for (int i=0; i < num_tiles; i++)
		{
			const Tilemap::Tile& tile  = m_level->getTile(i);

			// tile name
			std::string name = tile.name;
			for (int j=0; j < name.length(); j++)
			{
				output.write_byte(name.at(j));
			}
			output.write_byte(0);	// null terminator

			output.write_dword(tile.id);

			for (int j=0; j < 4; j++)
			{
				output.write_float(tile.points[j].x);
				output.write_float(tile.points[j].y);
			}
		}

		// tilemap
		const Tilemap::Config& tilemap = m_level->getTilemapConfig();

		// tilemap xstart
		output.write_dword(tilemap.xstart);

		// tilemap xend
		output.write_dword(tilemap.xend);

		// tilemap ystart
		output.write_dword(tilemap.ystart);

		// tilemap yend
		output.write_dword(tilemap.yend);

		// tilemap tile size
		output.write_float(tilemap.tile_size);

		// tilemap data
		for (int j=tilemap.ystart; j < tilemap.yend; j++)
		{
			for (int i=tilemap.xstart; i < tilemap.xend; i++)
			{
				output.write_dword(m_level->readTilemap(i, j));
			}
		}


		// affixes
		bool enable;
		int mins, secs;
		QString item;
		int num;

		// ID
		output.write((char*)affix_id, 4);

		// time limit
		m_level_conf->getTimelimit(&enable, &mins, &secs);
		output.write_dword(enable ? 1 : 0);
		output.write_dword(mins << 8 | secs);

		// collect 1
		m_level_conf->getCollect1(&enable, &item, &num);
		output.write_dword(enable ? 1 : 0);
		output.write_string(item.toStdString());
		output.write_dword(num);

		// collect 2
		m_level_conf->getCollect2(&enable, &item, &num);
		output.write_dword(enable ? 1 : 0);
		output.write_string(item.toStdString());
		output.write_dword(num);

		// collect 3
		m_level_conf->getCollect3(&enable, &item, &num);
		output.write_dword(enable ? 1 : 0);
		output.write_string(item.toStdString());
		output.write_dword(num);

		// don't collect
		m_level_conf->getDontcollect(&enable, &item);
		output.write_dword(enable ? 1 : 0);
		output.write_string(item.toStdString());

		// avoid
		m_level_conf->getAvoid(&enable);
		output.write_dword(enable ? 1 : 0);
		
		// exit
		m_level_conf->getExit(&enable);
		output.write_dword(enable ? 1 : 0);

		// inverse gravity
		m_level_conf->getGravity(&enable);
		output.write_dword(enable ? 1 : 0);
	}
	catch (ios_base::failure&)
	{
		output.close();
		return false;
	}
	catch (int e)
	{
		output.close();
		return false;
	}

	return true;
}


void MainWindow::writeBLBFile(QString& filename)
{
	BinaryFile output;

	const char blbx_id[4] = { 0x42, 0x4c, 0x42, 0x58 };
	const char blox_id[4] = { 0x42, 0x4c, 0x4f, 0x58 };
	const char tlex_id[4] = { 0x54, 0x4c, 0x45, 0x58 };
	const unsigned int blb_version = 0x10000;

	int num_objects = m_level->numObjects();

	if (num_objects <= 0)		// TODO: error message
		return;

	// calculate level bounds
	float level_minx;
	float level_maxx;
	float level_miny;
	float level_maxy;

	level_minx = m_level->getObject(0)->getPoint(0).x;
	level_maxx = level_minx;
	level_miny = m_level->getObject(0)->getPoint(0).y;
	level_maxy = level_miny;

	for (int i=0; i < num_objects; i++)
	{
		Level::Object* obj = m_level->getObject(i);
		int num_points = obj->getNumPoints();

		for (int p=0; p < num_points; p++)
		{
			glm::vec2 point = obj->getPoint(p);
			if (point.x < level_minx)
				level_minx = point.x;
			if (point.x > level_maxx)
				level_maxx = point.x;
			if (point.y < level_miny)
				level_miny = point.y;
			if (point.y > level_maxy)
				level_maxy = point.y;
		}
	}

	int num_tiles = m_level->getNumTiles();




	try
	{
		output.open(filename.toStdString(), BinaryFile::MODE_WRITEONLY);

		// ID
		output.write((char*)blbx_id, 4);

		// BLB version
		output.write_dword(blb_version);

		// Number of objects
		output.write_dword(num_objects);

		// Texture name
		QString texname_stripped = m_texture_file.section('/', -1);
		QByteArray texname = texname_stripped.toLocal8Bit();
		for (int i=0; i < texname.length(); i++)
		{
			output.write_byte(texname.at(i));
		}
		output.write_byte(0);	// null terminator


		// Level bounds
		output.write_float(level_minx);
		output.write_float(level_maxx);
		output.write_float(level_miny);
		output.write_float(level_maxy);

		// Object ID
		output.write((char*)blox_id, 4);

		// Objects
		for (int i=0; i < num_objects; i++)
		{
			Level::Object* obj = m_level->getObject(i);
			int num_points = obj->getNumPoints();
			Level::ObjectType type = obj->getType();
			int z = obj->getZ();

			// Object type
			output.write_dword(type);

			// Num points
			output.write_dword(num_points);

			// Object Z
			output.write_dword(z);

			// points
			for (int p=0; p < num_points; p++)
			{
				glm::vec2 point = obj->getPoint(p);
				glm::vec2 uv = obj->getUV(p);
				
				output.write_float(point.x);
				output.write_float(point.y);
				output.write_float(uv.x);
				output.write_float(uv.y);
			}

			// parameters
			switch (type)
			{
				case Level::OBJECT_TYPE_COLLISION:
				{
					break;
				}
				case Level::OBJECT_TYPE_GEOVIS:
				{
					break;
				}
				case Level::OBJECT_TYPE_DESTRUCTIBLE:
				{
					break;
				}
				case Level::OBJECT_TYPE_TRIGGER:
				{
					// trigger type
					output.write_dword(obj->getParam(0).i);
					output.write_dword(obj->getParam(1).i);
					output.write_dword(obj->getParam(2).i);
					output.write_dword(obj->getParam(3).i);
					break;
				}
				case Level::OBJECT_TYPE_SLIDER:
				{
					output.write_dword(obj->getParam(0).i);
					break;
				}
				case Level::OBJECT_TYPE_MOVER:
				{
					output.write_dword(obj->getParam(0).i);
					output.write_dword(obj->getParam(1).i);
					output.write_dword(obj->getParam(2).i);
					output.write_dword(obj->getParam(3).i);
					output.write_dword(obj->getParam(4).i);
					break;
				}
			}
		}

		// Number of tiles
		output.write_dword(num_tiles);

		// ID
		output.write((char*)tlex_id, 4);

		for (int i=0; i < num_tiles; i++)
		{
			const Tilemap::Tile tile = m_level->getTile(i);			

			// UVs
			for (int j=0; j < 4; j++)
			{
				output.write_float(tile.points[j].x);
				output.write_float(tile.points[j].y);
			}
		}

		// Tilemap variables
		const Tilemap::Config& tilemap = m_level->getTilemapConfig();

		output.write_dword(tilemap.xstart);
		output.write_dword(tilemap.xend);
		output.write_dword(tilemap.ystart);
		output.write_dword(tilemap.yend);
		output.write_float(tilemap.tile_size);

		// Tilemap data
		for (int j=tilemap.ystart; j < tilemap.yend; j++)
		{
			for (int i=tilemap.xstart; i < tilemap.xend; i++)
			{
				int data = m_level->readTilemap(i, j) + 1;

				assert(data >= 0 && data < 32768);

				// encode to var num
				if (data < 128)
				{
					// single byte, bit 0x80 is zero
					output.write_byte(data);
				}
				else
				{
					// first byte has bit 0x80 set, and the low 7 bits of word
					output.write_byte((data & 0x7f) | 0x80);

					// second byte has bits 8-14
					output.write_byte((data >> 7) & 0xff);
				}
			}
		}
	}
	catch (ios_base::failure&)
	{
		output.close();
		return;
	}
	catch (int e)
	{
		output.close();
		return;
	}
}