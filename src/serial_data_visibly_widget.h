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
// Last changed at 2023-02-16                   //
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

	void EnableAntialiasing(bool enable);
	void SetBackgroundColor(QColor bg_color);
	void SetForegroundColor(QColor fg_color);

	QColor getBackground_color() const;
	void setBackground_color(const QColor &newBackground_color);

private:
	Ui::SerialDataVisiblyWidget *ui;

	bool   enable_antialiasing;

	QColor background_color;
	QColor foreground_color;

public slots:
	void OnUpdateData(uint8_t* data, uint16_t length);

	// QWidget interface
protected:
	void paintEvent(QPaintEvent *event);
};

#endif // SERIAL_DATA_VISIBLY_WIDGET_H
