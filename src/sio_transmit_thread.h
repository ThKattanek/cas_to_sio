//////////////////////////////////////////////////
//                                              //
// CasToSio                                     //
// by  Thorsten Kattanek                        //
//												//
// Atari Cas files send over sio bus with uart  //
//                                              //
// #file: sio_transmit_thread.h                 //
//                                              //
// This source code is Copyright protected!     //
//                                              //
// Last changed at 2023-01-01                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#ifndef SIOTRANSMITTHREAD_H
#define SIOTRANSMITTHREAD_H

#include <QProgressBar>
#include <QThread>
#include <QStringList>

#include <libserialport.h>

#include "./cas_file_class.h"

class SIOTransmitThread : public QThread
{
	Q_OBJECT

public:
	SIOTransmitThread(QObject*);
	~SIOTransmitThread();
	QStringList GetAllSerialPortNames();

	QString serial_port_name;

	QProgressBar *progress_bar;
	CASFileClass *cas;

public slots:
	void OnBytesWritten(qint64 bytes);

private:
	void InitSerialPort();
	struct sp_port **port_list;
	bool thread_end;

protected:
	void run() override;
};

#endif // SIOTRANSMITTHREAD_H
