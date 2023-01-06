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
// Last changed at 2023-01-05                   //
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

private:
	Ui::MainWindow *ui;

	CASFileClass cas;
	SIOTransmitThread *transmitter;
};
#endif // MAINWINDOW_H
