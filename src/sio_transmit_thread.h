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
// Last changed at 2023-01-23                   //
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

	void SetBaudRateFactor(float baudrate_factor);
	void SetMaxIrgTime(int max_irg_time);

	float GetBaudRateFactor();
	int GetMaxIrgTime();

	bool TogglePause();
	void Stop();

	QString serial_port_name;

	QProgressBar *progress_bar;
	CASFileClass *cas;

signals:
	void CasIsEnd();
	void ChangeProgress(int progress, int time_counter);
	void MessageLog(QString message);

private:
	bool OpenSerialPort();
	void CloseSerialPort();

	sp_port* port;
	struct sp_event_set *event_set;
	bool pause;
	bool thread_end;

	int max_irg_time;			// in ms (Default 20000)
	float baudrate_factor;		// Default 1.0 (Example 1.1 = 110%, 0.9 = 90%

protected:
	void run() override;
};

#endif // SIOTRANSMITTHREAD_H
