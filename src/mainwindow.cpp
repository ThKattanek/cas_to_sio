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

#include <QMessageBox>

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
		last_dir = dir.absolutePath();

		FILE *file = qfopen(filename, "rb");

		if(!cas.Open(file))
		{
			QMessageBox::critical(this, tr("CAS not open ..."), QString::fromStdString(cas.GetLastErrorString()));
		}
		else
		{
			QString out_message;

			int count;
			count = cas.GetChunkTypeCount(CHUNK_TYPE_FUJI);
			out_message += tr("FUJI Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetChunkTypeCount(CHUNK_TYPE_BAUD);
			out_message += tr("BAUD Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetChunkTypeCount(CHUNK_TYPE_DATA);
			out_message += tr("DATA Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetChunkTypeCount(CHUNK_TYPE_FSK);
			out_message += tr("FSK Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetChunkTypeCount(CHUNK_TYPE_PWMS);
			out_message += tr("PWMS Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetChunkTypeCount(CHUNK_TYPE_PWMC);
			out_message += tr("PWMC Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetChunkTypeCount(CHUNK_TYPE_PWMD);
			out_message += tr("PWMD Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetChunkTypeCount(CHUNK_TYPE_PWM1);
			out_message += tr("PWM1 Chunk Count: ") + QString::number(count) + "\n";

			QMessageBox::information(this, tr("CAS Information"), out_message);
		}
	}
}

