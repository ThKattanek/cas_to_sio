//////////////////////////////////////////////////
//                                              //
// CasToSio                                     //
// by  Thorsten Kattanek                        //
//												//
// Atari Cas files send over sio bus with uart  //
//                                              //
// #file: mainwindow.h                          //
//                                              //
// This source code is Copyright protected!     //
//                                              //
// Last changed at 2023-01-22                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>

#ifdef __WIN32      // MXE
		#include <quazip/quazip.h>
		#include <quazip/quazipfile.h>
#else
		#include <quazip5/quazip.h>
		#include <quazip5/quazipfile.h>
#endif

#include "./cas_file_class.h"
#include "./sio_transmit_thread.h"
#include "./qutils.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_actionOpen_CAS_Image_A8CAS_triggered();
	void on_cas_open_button_clicked();
	void on_cas_start_button_clicked();
	void on_baudrate_spin_valueChanged(int);
	void on_irg_time_spin_valueChanged(int);
	void on_cas_pause_button_clicked();
	void on_cas_stop_button_clicked();
	void on_actionExit_triggered();

	void OnCasIsEnd();
	void OnChangeProgress(int progress, int time_counter);
	void OnMessageLog(QString message);

private:
	void SetPlayTime();
	QString ConvertTimeToString(int time_ms);
	void SetCasButtons(bool open_btn, bool start_btn, bool pause_btn, bool stop_btn);

	Ui::MainWindow *ui;
	CASFileClass cas;
	SIOTransmitThread *transmitter;

	int current_playtime;
};
#endif // MAINWINDOW_H
