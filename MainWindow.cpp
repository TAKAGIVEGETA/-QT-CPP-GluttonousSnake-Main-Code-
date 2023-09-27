#include "MainWindow.h"
time_t begin_time;
time_t end_time;

MainWindow::MainWindow(int width, int height, QWidget* parent) : QWidget(parent)
{
    this->setFixedSize(width * BLOCKSIZE, height * BLOCKSIZE);
    this->setWindowFlags(Qt::FramelessWindowHint);

    this->shakeTimer = new QTimer(this);
    this->shakeTimer->setInterval(24);
    connect(this->shakeTimer, &QTimer::timeout, this, [this] {
        static int shakeCount = 0;
        if (shakeCount < 10) {
            int offsetX = (shakeCount % 2 == 0) ? 20 : -20;
            int offsetY = (shakeCount % 2 == 0) ? -10 : 10;
            this->move(this->pos() + QPoint(offsetX, offsetY));
            ++shakeCount;
        }
        else {
            shakeCount = 0;
            this->shakeTimer->stop();
        }
    });

    int rows = this->height() / BLOCKSIZE + 2;
    int cols = this->width() / BLOCKSIZE + 2;
    this->Map = new int* [rows];
    for (int i = 0; i < rows; i++) this->Map[i] = new int[cols];
    for (int i = 0; i < rows; i++) for (int j = 0; j < cols; j++) this->Map[i][j] = 0;
    for (int i = 0; i < rows; i++) {
        this->Map[i][0] = -1;
        this->Map[i][cols - 1] = -1;
    }
    for (int j = 0; j < cols; j++) {
        this->Map[0][j] = -1;
        this->Map[rows - 1][j] = -1;
    }
    for (int i = rows / 2; i < rows / 2 + 3; i++) {
        this->Map[i][cols / 2] = 1;
        this->Snacks.push_back(new Block(i, cols / 2));
    }
    this->generateFood();

    this->updateTimer = new QTimer(this);
    connect(this->updateTimer, &QTimer::timeout, this, [this] {this->updatePosition(); this->update(); });
    this->updateTimer->start(120);
}

MainWindow::~MainWindow()
{
    for (auto& snack : this->Snacks) delete snack;
    for (auto& Triangle : this->Triangles) delete Triangle;
    for (int i = 0; i < this->height() / BLOCKSIZE + 2; i++) delete[] this->Map[i];
    delete[] this->Map;
}

void MainWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (this->isGameOver == true) {
        painter.drawPixmap(0, 0, this->width(), this->height(), QPixmap("BackGround2.jpg"));
        QString text = QString("~Best Score:%1~").arg(QString::number(this->MaxScore));
        QFont font1("Ink Free", 50);
        font1.setBold(true);
        QFontMetrics fm1(font1);
        QRect textRect = fm1.boundingRect(text);
        int x = (this->width() - textRect.width()) / 2;
        int y = (this->height() - textRect.height()) / 2;
        painter.setFont(font1);
        painter.setPen(QPen(QColor(254, 181, 192, 64), 3, Qt::SolidLine));
        painter.drawText(x, y + fm1.ascent(), text);

        text = "-Press any key to continue-";
        QFont font2("Segoe Print", 25);
        font2.setBold(true);
        QFontMetrics fm2(font2);
        textRect = fm2.boundingRect(text);
        x = (this->width() - textRect.width()) / 2;
        y = (this->height() - textRect.height()) / 2;
        painter.setFont(font2);
        painter.setPen(QPen(QColor(0, 0, 0), 3, Qt::SolidLine));
        painter.drawText(x, y + fm2.ascent() + 175, text);

        text = QString("*Current Score:%1*").arg(QString::number(this->score));
        QFont font3("Segoe Print", 30);
        font3.setBold(true);
        QFontMetrics fm3(font3);
        textRect = fm3.boundingRect(text);
        x = (this->width() - textRect.width()) / 2;
        y = (this->height() - textRect.height()) / 2;
        painter.setFont(font3);
        painter.setPen(QPen(QColor(254, 126, 63, 120), 3, Qt::SolidLine));
        painter.drawText(x, y + fm3.ascent() - 80, text);
        begin_time = clock();
        return;
    }

    static int RGB = 0;
    static bool RGBFlag = true;
    if (RGB == 100) RGBFlag = false;
    else if (RGB == 0) RGBFlag = true;
    RGB += RGBFlag ? 2 : -2;
    painter.fillRect(this->rect(), QColor(RGB, RGB, RGB));
    int x, y, i;
    for (i = 0; i < 1000; i++) {
        x = QRandomGenerator::global()->bounded(0, this->width());
        y = QRandomGenerator::global()->bounded(0, this->height());
        painter.setPen(TAKAGI);
        painter.drawEllipse(x, y, 1.7, 1.7);
    }

    painter.setPen(QPen(TAKAGI, 2, Qt::SolidLine));
    painter.setBrush(TAKAGI);
    painter.drawPolygon(this->food);

    i = 0;
    double size = BLOCKSIZE + 10;
    for (const auto& snack : this->Snacks) {
        size -= i++ < 80 ? 0.5 : 0;
        if (i <= 36) painter.setBrush(QBrush(QColor(255 - i * 7, 0, i * 7)));
        else painter.setBrush(TAKAGI);
        painter.drawRect((snack->COL - 1) * BLOCKSIZE, (snack->ROW - 1) * BLOCKSIZE, size, size);
    }

    QString text = QString("Score:%1").arg(QString::number(this->score));
    QFont font("Segoe Print", 20);
    font.setBold(true);
    QFontMetrics fm(font);
    painter.setFont(font);
    painter.drawText(this->width() - fm.boundingRect(text).width() - 10, 10 + fm.ascent(), text);

    for (auto it = this->Triangles.begin(); it != this->Triangles.end();) {
        if ((*it)->isOutOfRange == true) {
            delete* it;
            it = this->Triangles.erase(it);
        }
        else {
            QPolygonF triangle;
            triangle << QPointF(0, -20) << QPointF(-10, 10) << QPointF(10, 10);
            QPainter painter(this);
            painter.setPen(QPen(Qt::gray, 2, Qt::SolidLine));
            painter.setBrush(QBrush(QColor(3, 4, 5, 84)));
            painter.translate((*it)->position);
            painter.rotate((*it)->angle);
            painter.drawPolygon(triangle);
            ++it;
        }
    }
    this->ListenFlag = true;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if (this->isGameOver == true) {
        end_time = clock();
        if (double(end_time - begin_time) / CLOCKS_PER_SEC > 1) emit MainWindow::Quit(this);
        return;
    }
    if (this->ListenFlag == true) this->ListenFlag = false;
    else {
        event->ignore();
        return;
    }
    if (event->modifiers() == Qt::ShiftModifier) this->updateTimer->setInterval(50);
    if ((event->key() == Qt::Key_W || event->key() == Qt::Key_Up) && direction != Direction::DOWN)
        this->direction = Direction::UP;
    else if ((event->key() == Qt::Key_S || event->key() == Qt::Key_Down) && direction != Direction::UP)
        this->direction = Direction::DOWN;
    else if ((event->key() == Qt::Key_A || event->key() == Qt::Key_Left) && direction != Direction::RIGHT)
        this->direction = Direction::LEFT;
    else if ((event->key() == Qt::Key_D || event->key() == Qt::Key_Right) && direction != Direction::LEFT)
        this->direction = Direction::RIGHT;
}

void MainWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Shift) this->updateTimer->setInterval(120);
}

void MainWindow::updatePosition()
{
    int ROW, COL;
    switch (this->direction) {
        case Direction::UP:
            ROW = this->Snacks.front()->ROW - 1;
            COL = this->Snacks.front()->COL;
            break;
        case Direction::DOWN:
            ROW = this->Snacks.front()->ROW + 1;
            COL = this->Snacks.front()->COL;
            break;
        case Direction::LEFT:
            ROW = this->Snacks.front()->ROW;
            COL = this->Snacks.front()->COL - 1;
            break;
        case Direction::RIGHT:
            ROW = this->Snacks.front()->ROW;
            COL = this->Snacks.front()->COL + 1;
            break;
    }
    switch (this->Map[ROW][COL]) {
        case 1: case 0: {
            bool flag = false;
            if (!this->Map[ROW][COL] ||
                ROW == this->Snacks.back()->ROW && COL == this->Snacks.back()->COL) flag = true;
            this->Snacks.push_front(new Block(ROW, COL));
            this->Map[this->Snacks.back()->ROW][this->Snacks.back()->COL] = 0;
            delete this->Snacks.back();
            this->Snacks.pop_back();
            this->Map[ROW][COL] = 1;
            if (flag == true) return;
            this->repaint();
        }
        case -1:
            emit MainWindow::StopPlayingMusic();
            this->GameOver();
            return;
        case 2:
            this->Snacks.push_front(new Block(ROW, COL));
            this->Map[ROW][COL] = 1;
            this->shakeTimer->start();
            for (int i = 0; i < 50; i++)
                this->Triangles.push_back(
                    new FlyingTriangle(QPointF((COL - 1) * BLOCKSIZE, (ROW - 1) * BLOCKSIZE)));
            this->score += 10;
            this->generateFood();
            return;
    }
}

void MainWindow::GameOver()
{
    this->updateTimer->stop();
    this->shakeTimer->stop();
    for (auto& Triangle : this->Triangles) Triangle->timer->stop();

    QPoint pre_pos = this->pos();
    for (int i = 0; i < 50; i++) {
        int offsetX = QRandomGenerator::global()->bounded(-50, 50);
        int offsetY = QRandomGenerator::global()->bounded(-50, 50);
        this->move(pre_pos + QPoint(offsetX, offsetY));
        Sleep(30);
    }
    this->move(pre_pos);
    this->save();
    this->isGameOver = true;
    this->repaint();
}

void MainWindow::generateFood()
{
    std::vector<std::pair<int, int>> emptyPositions;
    for (int i = 0; i < this->height() / BLOCKSIZE; i++) 
        for (int j = 0; j < this->width() / BLOCKSIZE; j++)
            if (this->Map[i][j] == 0) emptyPositions.push_back(std::make_pair(i, j));
    if (!emptyPositions.empty()) {
        this->food.clear();
        int randomIndex = QRandomGenerator::global()->bounded(emptyPositions.size());
        this->Map[emptyPositions[randomIndex].first][emptyPositions[randomIndex].second] = 2;
        int Shape = QRandomGenerator::global()->bounded(3, 9);
        double angle = 2 * M_PI / Shape;
        for (int i = 0; i < Shape; i++) {
            double xVertex = (emptyPositions[randomIndex].second - 1) * BLOCKSIZE + BLOCKSIZE / 2 + BLOCKSIZE / 2 * qCos(i * angle);
            double yVertex = (emptyPositions[randomIndex].first - 1) * BLOCKSIZE + BLOCKSIZE / 2 + BLOCKSIZE / 2 * qSin(i * angle);
            this->food << QPointF(xVertex, yVertex);
        }
    }
}

void MainWindow::save()
{
    QFile file("ScoreRecord.txt");
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) file.close();
        else QMessageBox::critical(this, "错误", "创建文件失败");
    }

    QStringList Records;
    QString targetRecord = QString("%1 X %2").arg(
        QString::number(this->width() / BLOCKSIZE)).arg(QString::number(this->height() / BLOCKSIZE));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        int flag = false;
        while (!in.atEnd()) {
            QString record = in.readLine();
            if (record.split(':')[0] == targetRecord) {
                this->MaxScore = record.split(':')[1].toInt();
                this->MaxScore = this->score > this->MaxScore ? this->score : this->MaxScore;
                record = targetRecord + ":" + QString::number(this->MaxScore);
                flag = true;
            }
            Records << record;
        }
        if (flag == false) {
            QString record = targetRecord + ":" + QString::number(this->MaxScore = this->score);
            Records << record;
        }
        file.close();
    }
    else QMessageBox::critical(this, "错误", "无法打开文件");

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        QTextStream out(&file);
        for(const QString & data : Records) out << data << "\n";
        file.close();
    }
    else QMessageBox::critical(this, "错误", "无法打开文件");
}

FlyingTriangle::FlyingTriangle(QPointF pos, QWidget* parent) : QWidget(parent), position(pos)
{
    this->velocityX = QRandomGenerator::global()->bounded(-20, 20);
    this->velocityY = QRandomGenerator::global()->bounded(-20, 20);
    this->angle = QRandomGenerator::global()->bounded(-360, 360);
    if (this->velocityX == 0) this->velocityX += 5;
    this->timer = new QTimer(this);
    connect(this->timer, &QTimer::timeout, this, &FlyingTriangle::updatePosition);
    this->timer->start(20);
}

void FlyingTriangle::updatePosition()
{
    this->velocityY += GRAVITY;
    this->position.setX(this->position.x() + this->velocityX);
    this->position.setY(this->position.y() + this->velocityY);
    this->angle += this->velocityX > 0 ? 10 : -10;
    if (this->position.x() > 1550 || this->position.y() > 950 || this->position.x() < -50) {
        this->timer->stop();
        this->isOutOfRange = true;
    }
}