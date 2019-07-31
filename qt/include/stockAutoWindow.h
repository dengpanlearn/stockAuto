#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_stockAutoWindow.h"

class CStockAutoWindow : public QMainWindow
{
    Q_OBJECT

public:
    CStockAutoWindow(QWidget *parent = Q_NULLPTR);

private:
    Ui::CStockAutoWindowClass ui;
};
