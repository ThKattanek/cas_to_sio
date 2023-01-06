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
// Last changed at 2023-01-04                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <zip.h>

#include <QMessageBox>
#include <QStringList>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	this->setWindowTitle("CasToSio");

	transmitter = new SIOTransmitThread(this);

	QStringList port_list = transmitter->GetAllSerialPortNames();
	ui->serial_ports->addItems(port_list);

	ui->baudrate_spin->setValue(transmitter->GetBaudRateFactor() * 100.0f);
	ui->irg_time_spin->setValue(transmitter->GetMaxIrgTime());
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

	QString unzip_filename = QDir::tempPath() + "/";

	bool is_zip = false;

	QString filename = QFileDialog::getOpenFileName(this, tr("Atari CAS File open "), last_dir, tr("CAS Files ") + "(*.cas *.zip);;" + tr("All Files ") + "(*.*)", 0, QFileDialog::DontUseNativeDialog);
	if(filename != "")
	{
		QFileInfo file_info(filename);
		last_dir = file_info.absolutePath();

		if(file_info.suffix().toLower() == "zip")
		{
			QuaZip zip(filename);
			if(zip.open(QuaZip::mdUnzip))
			{
				QString first_file_name = zip.getFileNameList()[0];
				unzip_filename += first_file_name;

				std::cout << "Zip is Open. " << zip.getEntriesCount() << " [" << first_file_name.toLocal8Bit().data() << "]" << std::endl;

				QuaZipFile zf(&zip);
				if(!zip.setCurrentFile(first_file_name))
				{
					std::cout << "Fehler in ZIP-Datei: " << first_file_name.toLocal8Bit().data() << std::endl;
					return;
				}

				if(!zf.open(QIODevice::ReadOnly))
				{
					std::cout << "Fehler beim Öffnen des ZIPFiles: " << first_file_name.toLocal8Bit().data();
				}

				QFile outfile(unzip_filename);
				if(outfile.open(QIODevice::ReadWrite))
				{
					outfile.write(zf.readAll());
					outfile.close();
				}
				else
				{
					zf.close();
					zip.close();
					return;
				}

				zf.close();
				zip.close();
				is_zip = true;
			}
		}

		FILE *file;

		if(!is_zip)
			file = qfopen(filename, "rb");
		else
			file = qfopen(unzip_filename, "rb");

		if(!cas.Open(file))
		{
			QMessageBox::critical(this, tr("CAS not open ..."), QString::fromStdString(cas.GetLastErrorString()));
		}

		if(is_zip)
		{
			QFile file(unzip_filename);
			if(file.exists())
				file.remove();
		}
	}
}

void MainWindow::on_cas_open_button_clicked()
{
	on_actionOpen_CAS_Image_A8CAS_triggered();
}


void MainWindow::on_cas_start_button_clicked()
{
	if(cas.IsOpen())
	{
		transmitter->SetBaudRateFactor(ui->baudrate_spin->value() / 100.0f);
		transmitter->SetMaxIrgTime(ui->irg_time_spin->value());

		transmitter->cas = &cas;
		transmitter->progress_bar = ui->transmit_progress;
		transmitter->serial_port_name = ui->serial_ports->currentText();
		transmitter->start();
	}
}

