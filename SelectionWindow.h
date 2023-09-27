#pragma once
#include "ui_SelectionWindow.h"
#include "MainWindow.h"
#include <QButtonGroup>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QRadioButton>
#include <QMessageBox>
#include <QPushButton>
#include <QPainter>
#include <QSpinBox>
#include <QWidget>
#include <QSlider>

class SelectionWindow : public QWidget
{
	Q_OBJECT
public:
    explicit SelectionWindow(QWidget *parent = nullptr);
protected:
    void closeEvent(QCloseEvent* event) override
    {
        if (QMessageBox::question(this, "关闭 ？", "确定要关闭应用吗?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) event->accept();
        else event->ignore();
    }
    void paintEvent(QPaintEvent* event) override 
    {
        Q_UNUSED(event);
        QPainter painter(this);
        painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap("BackGround.jpg"));
    }
private:
    QMediaPlayer player;
    QAudioOutput audioOutput;
	Ui::SelectionWindowClass ui;
};