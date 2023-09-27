#pragma once
#include <QRandomGenerator>
#include <QFontMetrics>
#include <QTextStream>
#include <QMessageBox>
#include <QTextStream>
#include <QtWidgets>
#include <QPainter>
#include <QTimer>
#include <vector>
#include <QFile>
#include <list>
#define BLOCKSIZE 50
#define GRAVITY 0.3
#define TAKAGI QColor(QRandomGenerator::global()->bounded(0, 256), QRandomGenerator::global()->bounded(0, 256),QRandomGenerator::global()->bounded(0, 256))

enum Direction { UP, DOWN, RIGHT, LEFT };
class Block
{
public:
	Block(int ROW, int COL) : ROW(ROW), COL(COL) {}
	int ROW;
	int COL;
};

class MainWindow;
class FlyingTriangle : public QWidget
{
	Q_OBJECT
	friend class MainWindow;
public:
	FlyingTriangle(QPointF pos, QWidget* parent = nullptr);
private slots:
	void updatePosition();
private:
	QPointF position;
	double angle;
	double velocityX;
	double velocityY;
	bool isOutOfRange = false;
	QTimer* timer;
};

class MainWindow : public QWidget
{
    Q_OBJECT
public:
	explicit MainWindow(int width, int height, QWidget* parent = nullptr);
	~MainWindow();
protected:
	void paintEvent(QPaintEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;
signals:
	void StopPlayingMusic();
	void Quit(MainWindow*);
private:
	void save();
	void GameOver();
	void generateFood();
	void updatePosition();
	int** Map;
	int score = 0;
	int MaxScore = 0;
	bool ListenFlag = false;
	bool isGameOver = false;
	QPolygonF food;
	QTimer* updateTimer;
	QTimer* shakeTimer;
    std::list<Block*> Snacks;
	std::vector<FlyingTriangle*> Triangles;
	Direction direction = Direction::UP;
};