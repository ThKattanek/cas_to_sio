//////////////////////////////////////////////////
//                                              //
// CasToSio                                     //
// by  Thorsten Kattanek                        //
//												//
// Atari Cas files send over sio bus with uart  //
//                                              //
// #file: serial_data_visibly_widget.h          //
//                                              //
// This source code is Copyright protected!     //
//                                              //
// Last changed at 2023-02-09                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#ifndef SERIAL_DATA_VISIBLY_WIDGET_H
#define SERIAL_DATA_VISIBLY_WIDGET_H

#include <QWidget>

namespace Ui {
class SerialDataVisiblyWidget;
}

class SerialDataVisiblyWidget : public QWidget
{
	Q_OBJECT

public:
	explicit SerialDataVisiblyWidget(QWidget *parent = nullptr);
	~SerialDataVisiblyWidget();

private:
	Ui::SerialDataVisiblyWidget *ui;

public slots:
	void OnUpdateData(uint8_t* data, uint16_t length);

};

#endif // SERIAL_DATA_VISIBLY_WIDGET_H
