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
#include <QStringList>
#include <libserialport.h>>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setWindowTitle("CasToSio");

	InitSerialPort();

	transmitter = new SIOTransmitThread(this);

	QStringList port_list = transmitter->GetAllSerialPortNames();
	ui->serial_ports->addItems(port_list);
}

MainWindow::~MainWindow()
{
	if(transmitter != nullptr)
		delete transmitter;

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
			count = cas.GetFujiChunkCount();
			out_message += tr("FUJI Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetBaudChunkCount();
			out_message += tr("BAUD Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetDataChunkCount();
			out_message += tr("DATA Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetFskChunkCount();
			out_message += tr("FSK Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetPWMSChunkCount();
			out_message += tr("PWMS Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetPWMCChunkCount();
			out_message += tr("PWMC Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetPWMDChunkCount();
			out_message += tr("PWMD Chunk Count: ") + QString::number(count) + "\n";
			count = cas.GetPWM1ChunkCount();
			out_message += tr("PWM1 Chunk Count: ") + QString::number(count) + "\n";

			QMessageBox::information(this, tr("CAS Information"), out_message);
		}
	}
}

void MainWindow::InitSerialPort()
{
	/*
	serial_port_info = new QSerialPortInfo();
	for(int i=0; i<serial_port_info->availablePorts().length(); i++)
		ui->serial_ports->addItem(serial_port_info->availablePorts()[i].portName());
	*/
}


void MainWindow::on_start_transfer_clicked()
{
	transmitter->cas = &cas;
	transmitter->progress_bar = ui->transmit_progress;
	transmitter->serial_port_name = ui->serial_ports->currentText();
	transmitter->start();
}

