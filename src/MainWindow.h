#pragma once

#include <QMainWindow>
#include <QAction>
#include <QBoxLayout.h>
#include <qfiledialog.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qxmlstream.h>
#include <qmessagebox.h>
#include <qshortcut.h>
#include <qkeysequence.h>
#include <qradiobutton.h>
#include <qcolordialog.h>

#include "assert.h"

#include "GLWidget.h"
#include "Level.h"
#include "ObjectBrowser.h"
#include "TextureEdit.h"
#include "ObjectEdit.h"
#include "BinaryFile.h"
#include "VisboxConf.h"
#include "ObjectFilter.h"
#include "ObjectDesigner.h"
#include "ObjectPrefabs.h"
#include "TilemapWidget.h"
#include "Tileset.h"
#include "LevelConf.h"
#include "TiledPrefabs.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

	QMenu *m_mainWindowMenu;

public:
	MainWindow();
	~MainWindow();
	void newDocument();

protected:
	void closeEvent(QCloseEvent *event);

private:
	void createActions();
	void createMenus();
	void createToolbars();
	void createStatusbar();
	void setCurrentFile(QString& filename);
	void resetControls();
	bool writeLevelFile(QString& filename);
	bool readLevelFile(QString& filename);
	bool handleUnsaved();
	void writeBLBFile(QString& filename);
	QString getLevelDir();
	void setLevelDir(QString path);
	QString getTextureDir();
	void setTextureDir(QString path);
	QString getExportDir();
	void setExportDir(QString path);
	void readConfigFile(QString& filename);
	void changeTexture(QString path);
	bool writeBinaryProjectFile(QString& filename);
	bool readBinaryProjectFile(QString& filename);
	void reset();
	
	QMenu* m_fileMenu;
	QAction* m_newAction;
	QAction* m_openAction;
	QAction* m_openLegacyAction;
	QAction* m_saveAction;
	QAction* m_saveAsAction;
	QAction* m_exportAction;
	QAction* m_exitAction;
	QMenu* m_editMenu;
	QAction* m_copyAction;
	QAction* m_pasteAction;
	QAction* m_toggleGridAction;
	QAction* m_snapGridAction;
	QAction* m_bgColorAction;
	QAction* m_levelConfAction;
	QLabel* m_gridSizeLabel;
	QComboBox* m_gridSizeCombo;
	QWidget* m_gridSizeWidget;
	QMenu* m_textureMenu;
	QAction* m_loadTexAction;
	QMenu* m_prefabMenu;
	QAction* m_loadPrefabsAction;
	QAction* m_savePrefabsAction;
	QLabel* m_zoomLevelLabel;
	QComboBox* m_zoomLevelCombo;
	QWidget* m_zoomLevelWidget;
	QLabel* m_defTypeLabel;
	QComboBox* m_defTypeCombo;
	QWidget* m_defTypeWidget;
	QLabel* m_prefabScaleLabel;
	QDoubleSpinBox* m_prefabScaleSpin;
	QWidget* m_prefabScaleWidget;
	QLabel* m_prefabDepthLabel;
	QSpinBox* m_prefabDepthSpin;
	QWidget* m_prefabDepthWidget;
	QLabel* m_prefabAlignLabel;
	QComboBox* m_prefabAlignCombo;
	QWidget* m_prefabAlignWidget;
	QLabel* m_colorLabel;
	QPushButton* m_colorButton;
	QWidget* m_colorWidget;

	QActionGroup* m_opgroup;
	QAction* m_select_action;
	QAction* m_move_action;
	QAction* m_rotate_action;
	QAction* m_scale_action;
	QAction* m_draw_poly_action;
	QAction* m_draw_rect_action;
	QAction* m_prefab_action;
	QAction* m_tiledprefab_action;
	QAction* m_tilemap_action;

	QAction* m_toggle_texedit;
	QAction* m_toggle_objedit;
	QAction* m_toggle_objfilter;
	QAction* m_toggle_objdesigner;
	QAction* m_toggle_tileset_window;
	QAction* m_toggle_tiledprefabs_window;

	QAction* m_toggle_visbox;
	QAction* m_visbox_conf_action;

	QAction* m_tilemap_widget_action;

	QAction* m_edgify_action;

	QString m_open_file;
	QString m_texture_file;

	QToolBar* m_file_toolbar;
	QToolBar* m_op_toolbar;
	QToolBar* m_grid_toolbar;
	QToolBar* m_editor_toolbar;
	QToolBar* m_visbox_toolbar;
	QToolBar* m_zoom_toolbar;
	QToolBar* m_deftype_toolbar;

	GLWidget* m_glwidget;
	ObjectBrowser* m_objbrowser;	

	Level* m_level;

	TextureEdit* m_texedit;
	QImage* m_texture;

	bool m_texedit_open;
	bool m_objedit_open;
	bool m_objfilter_open;
	bool m_objdesigner_open;
	bool m_tileset_window_open;
	bool m_tiledprefabs_window_open;

	bool m_enable_grid;
	bool m_snap_grid;

	ObjectEdit* m_objedit;
	ObjectFilter* m_objfilter;
	ObjectDesigner* m_objdesigner;
	ObjectPrefabs* m_objprefabs;

	bool m_enable_visbox;
	VisboxConf* m_visbox_conf;

	QShortcut* m_zoomin_shortcut;
	QShortcut* m_zoomout_shortcut;

	QColor m_bgcolor;
	QColor m_object_color;

	bool m_enable_tilemap_widget;
	TilemapWidget* m_tilemap_widget;

	TilesetWindow* m_tileset_window;
	TiledPrefabsWindow* m_tiledprefabs_window;

	LevelConf* m_level_conf;

public slots:
	void texEditClosed();
	void objEditClosed();
	void objFilterClosed();
	void objDesignerClosed();
	void tilesetWindowClosed();
	void tiledprefabsWindowClosed();
	void setPrefabScale(double scale);
	void setColor();

private slots:
	void newFile();
	void openFile();
	void openLegacyFile();
	bool saveFile();
	bool saveAsFile();
	void exportLevel();
	void exitProgram();
	void loadTexture();
	void loadPrefabs();
	void savePrefabs();

	void selectionMode();
	void moveMode();
	void rotateMode();
	void scaleMode();
	void drawPolyMode();
	void drawRectMode();
	void prefabMode();
	void tiledprefabMode();
	void tilemapMode();

	void toggleTexEdit();
	void toggleObjEdit();
	void toggleObjFilter();
	void toggleObjDesigner();
	void toggleTilesetWindow();
	void toggleTiledprefabsWindow();

	void toggleGrid();
	void snapGrid();
	void setGridSize(int size);

	void toggleVisbox();
	void visboxConfig();

	void tilemapConfig();

	void setZoomLevel(int zoom);
	void zoomIn();
	void zoomOut();

	void enableFilters(int filter);
	void disableFilters(int filter);

	void setDefType(int type);
	void prefabScaleChange(double value);
	void prefabDepthChange(int value);
	void setPrefabAlign(int align);

	void setBGColor();

	void tilemapConfigChange();

	void levelConfig();

	void edgify();
};
