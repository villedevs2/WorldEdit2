#pragma once

#include <qspinbox.h>
#include <qdialog.h>
#include <qformlayout.h>
#include <qdialogbuttonbox.h>

#include "Level.h"

class TilemapWidget : public QDialog
{
	Q_OBJECT

public:
	TilemapWidget(QWidget* parent, Level*  level);
	~TilemapWidget();

	int getXStart();
	int getXEnd();
	int getYStart();
	int getYEnd();
	void setValues(int xstart, int xend, int ystart, int yend);
	float getSize();
	void setSize(float size);

signals:
	void onConfigChanged();

private slots:
	void configAccept();

private:
	Level* m_level;

	QFormLayout* m_layout;

	QSpinBox* m_xstart;
	QSpinBox* m_xend;
	QSpinBox* m_ystart;
	QSpinBox* m_yend;
	QDoubleSpinBox* m_size;

	QDialogButtonBox* m_button_box;
};