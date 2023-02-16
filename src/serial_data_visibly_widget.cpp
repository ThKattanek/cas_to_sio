//////////////////////////////////////////////////
//                                              //
// CasToSio                                     //
// by  Thorsten Kattanek                        //
//												//
// Atari Cas files send over sio bus with uart  //
//                                              //
// #file: serial_data_visibly_widget.cpp        //
//                                              //
// This source code is Copyright protected!     //
//                                              //
// Last changed at 2023-02-16                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////
///
#include "serial_data_visibly_widget.h"
#include "ui_serial_data_visibly_widget.h"

#include <QPainter>

SerialDataVisiblyWidget::SerialDataVisiblyWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SerialDataVisiblyWidget)
{
	ui->setupUi(this);

	enable_antialiasing = false;
	background_color = QColor(Qt::black);
	foreground_color = QColor(Qt::red);
}

SerialDataVisiblyWidget::~SerialDataVisiblyWidget()
{
	delete ui;
}

void SerialDataVisiblyWidget::EnableAntialiasing(bool enable)
{
	enable_antialiasing = enable;
}

void SerialDataVisiblyWidget::SetBackgroundColor(QColor bg_color)
{
	background_color = bg_color;
}

void SerialDataVisiblyWidget::SetForegroundColor(QColor fg_color)
{
	foreground_color = fg_color;
}

void SerialDataVisiblyWidget::OnUpdateData(uint8_t *data, uint16_t length)
{
	qDebug("DataVisibly");
	update();
}

void SerialDataVisiblyWidget::paintEvent(QPaintEvent *event)
{
	QPainter p(this);

	p.setRenderHint(QPainter::Antialiasing, enable_antialiasing);

	// paint background
	p.fillRect(0, 0, this->width(), this->height(), QBrush(background_color));

	p.setPen(QPen(foreground_color, 2));

	p.drawLine(100, 0, 100, this->height());

	QWidget::paintEvent(event);

}
