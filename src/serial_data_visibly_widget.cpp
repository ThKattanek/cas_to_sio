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
// Last changed at 2023-02-09                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////
///
#include "serial_data_visibly_widget.h"
#include "ui_serial_data_visibly_widget.h"

SerialDataVisiblyWidget::SerialDataVisiblyWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::SerialDataVisiblyWidget)
{
	ui->setupUi(this);
}

SerialDataVisiblyWidget::~SerialDataVisiblyWidget()
{
	delete ui;
}

void SerialDataVisiblyWidget::OnUpdateData(uint8_t *data, uint16_t length)
{
	qDebug("DataVisibly");
	update();
}
