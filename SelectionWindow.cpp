#include "SelectionWindow.h"

SelectionWindow::SelectionWindow(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(410, 400);
	this->setWindowTitle("贪吃蛇");
	this->setWindowIcon(QIcon("icon1.ico"));

	connect(ui.Slider_Width, &QSlider::valueChanged, ui.spinBox_Width, &QSpinBox::setValue);
	connect(ui.Slider_Height, &QSlider::valueChanged, ui.spinBox_Height, &QSpinBox::setValue);
	connect(ui.spinBox_Width, &QSpinBox::valueChanged, ui.Slider_Width, &QSlider::setValue);
	connect(ui.spinBox_Height, &QSpinBox::valueChanged, ui.Slider_Height, &QSlider::setValue);
	ui.spinBox_Width->findChild<QLineEdit*>()->setStyleSheet("background-color: rgb(233,100,111);");
	ui.spinBox_Height->findChild<QLineEdit*>()->setStyleSheet("background-color: rgb(100,188,244);");

	QButtonGroup* group = new QButtonGroup(this);
	group->addButton(ui.Button_Open);
	group->addButton(ui.Button_Close);
	ui.Button_Close->setChecked(true);

	this->player.setAudioOutput(&this->audioOutput);
	this->player.setSource(QUrl::fromLocalFile("MagnusTheMagnus - Area.mp3"));
	this->player.setLoops(QMediaPlayer::Infinite);

	connect(ui.Button_OK, &QPushButton::clicked, this, [this] {
		ui.Button_OK->setEnabled(false);
		MainWindow* window = new MainWindow(ui.Slider_Width->value(), ui.Slider_Height->value());
		if (ui.Button_Open->isChecked()) this->player.play();

		connect(window, &MainWindow::StopPlayingMusic, this, [this] {this->player.stop(); });
		connect(window, &MainWindow::Quit, this, [this] (MainWindow* window) {
			disconnect(window);
			ui.Button_OK->setEnabled(true);
			window->close(); window->deleteLater(); this->show(); 
		});

		QRect screenGeometry = QGuiApplication::primaryScreen()->geometry();
		int x = (screenGeometry.width() - window->width()) / 2;
		int y = (screenGeometry.height() - window->height()) / 2;
		window->move(x, y); this->hide(); window->show();
	});
	connect(ui.Button_Exit, &QPushButton::clicked, this, &SelectionWindow::close);
}