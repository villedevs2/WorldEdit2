#pragma once

#include <QtGui>
#include <qdockwidget.h>
#include <qmainwindow.h>
#include <qtoolbar.h>
#include <qmenu.h>
#include <qcombobox.h>
#include <qlabel.h>
#include <qboxlayout.h>
#include <qpushbutton.h>
#include <qinputdialog.h>
#include <qmessagebox.h>

#include "Level.h"
#include "PolygonDef.h"
#include "ObjectPrefabs.h"

class ObjectDesignerWidget : public QWidget
{
	Q_OBJECT

public:
	enum OperationMode
	{
		MODE_DRAW_POLY,
		MODE_DRAW_RECT,
		MODE_MOVE,
		MODE_EDIT_VERTICES,
		MODE_EDIT_EDGES,
	};

	ObjectDesignerWidget(QWidget* parent, ObjectPrefabs* prefabs, Level* level);
	~ObjectDesignerWidget();

	void setTexture(QImage* texture);
	void setMode(OperationMode mode);

	int numPoints();

signals:
	void onInsertTile(int tile_id);

public slots:
	void setScale(int scale);
	void setZoom(int zoom);
	void setGrid(int grid);
	void enableShowGrid(bool enable);
	void enableSnapGrid(bool enable);
	void resetObject();
	void insertObject(QString& name);
	void insertTile(QString& name);

protected:
	void paintEvent(QPaintEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void keyReleaseEvent(QKeyEvent* event);
	void mouseMoveEvent(QMouseEvent* event);

private:
	glm::vec2 toScreenCoords(glm::vec2& point);
	glm::vec2 toUVCoords(glm::vec2& point);
	glm::vec2 snapToGrid(glm::vec2& point);

	void renderDrawPolyMode(QPainter& painter);
	void renderDrawRectMode(QPainter& painter);
	void renderClosedPoly(QPainter& painter);
	void drawGrid(QPainter& painter);

	static const int POINT_CLICKING_THRESHOLD = 6;

	QColor m_bgcolor;
	QColor m_validpoly_color;
	QColor m_errorpoly_color;
	QColor m_validline_color;
	QColor m_errorline_color;
	QColor m_point_color;
	QColor m_closedpoly_color;
	QColor m_closedline_color;

	QImage* m_texture;

	PolygonDef* m_polydef;
	ObjectPrefabs* m_prefabs;
	Level* m_level;

	unsigned int m_color;
	
	float m_zoom;

	bool m_panning;
	glm::vec2 m_scroll;
	glm::vec2 m_scroll_saved;
	glm::vec2 m_pan_point;
	bool m_poly_closed;
	
	bool m_rect_drawing;
	glm::vec2 m_rect_start_point;

	int m_selected_point;

	bool m_line_dragging;
	int m_line_point0;
	int m_line_point1;

	bool m_move_dragging;
	glm::vec2 m_move_reference;

	OperationMode m_mode;

	bool m_snap_grid;
	bool m_show_grid;
	int m_grid_size;
};



class ObjectDesigner : public QDockWidget
{
	Q_OBJECT

public:
	ObjectDesigner(QWidget* parent, Level* level, ObjectPrefabs* prefabs);
	~ObjectDesigner();

	void setTexture(QImage* texture);

	static const int NUM_GRID_SIZES = 8;
	static const float GRID_SIZE[NUM_GRID_SIZES];

	static const int NUM_ZOOM_LEVELS = 4;
	static const float ZOOM_LEVELS[NUM_ZOOM_LEVELS];

protected:
	void closeEvent(QCloseEvent* event);

signals:
	void onClose();
	void onInsertTile(int tile_id);

public slots:
	void setDrawPolyMode();
	void setDrawRectMode();
	void setMoveMode();
	void setEditVertexMode();
	void setEditEdgeMode();
	void reset();
	void insert();
	void insertTile();
	void toggleGrid();
	void snapGrid();
	void setGridSize(int size);

private:
	QMainWindow* m_window;	
	QMenu* m_menu;
	ObjectDesignerWidget* m_widget;

	QToolBar* m_edit_toolbar;
	QToolBar* m_zoom_toolbar;
	QToolBar* m_grid_toolbar;
	QToolBar* m_control_toolbar;

	QActionGroup* m_toolgroup;
	QAction* m_draw_poly_action;
	QAction* m_draw_rect_action;
	QAction* m_edit_vertex_action;
	QAction* m_edit_edge_action;
	QAction* m_move_action;
	
	QActionGroup* m_zoomgroup;
	QWidget* m_zoomlevel_widget;
	QLabel* m_zoomlevel_label;
	QComboBox* m_zoom_box;

	QActionGroup* m_gridgroup;
	QAction* m_togglegrid_action;
	QAction* m_snapgrid_action;
	QLabel* m_gridsize_label;
	QComboBox* m_gridsize_combo;
	QWidget* m_gridsize_widget;

	QActionGroup* m_controlgroup;
	QPushButton* m_reset_button;
	QPushButton* m_insert_button;
	QPushButton* m_inserttile_button;

	Level* m_level;
	ObjectPrefabs* m_prefabs;
};