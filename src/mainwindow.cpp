//////////////////////////////////////////////////
//                                              //
// CasToSio                                     //
// by  Thorsten Kattanek                        //
//												//
// Atari Cas files send over sio bus with uart  //
//                                              //
// #file: mainwindow.cpp                        //
//                                              //
// This source code is Copyright protected!     //
//                                              //
// Last changed at 2022-12-29                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	this->setWindowTitle("CasToSio");
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::on_actionOpen_CAS_Image_A8CAS_triggered()
{
	static QString last_dir = "";

	if(last_dir.isEmpty())
		last_dir = QDir::homePath();

	QString filename = QFileDialog::getOpenFileName(this, tr("Atari CAS File open "), last_dir, tr("CAS Files ") + "(*.cas);;" + tr("All Files ") + "(*.*)", 0, QFileDialog::DontUseNativeDialog);
	if(filename != "")
	{
		QFileInfo dir(filename);
		last_dir = dir.absoluteDir().absolutePath();
		qDebug("%s", last_dir.toLatin1().data());
	}
}

