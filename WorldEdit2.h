#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WorldEdit2.h"

class WorldEdit2 : public QMainWindow
{
    Q_OBJECT

public:
    WorldEdit2(QWidget *parent = Q_NULLPTR);

private:
    Ui::WorldEdit2Class ui;
};
