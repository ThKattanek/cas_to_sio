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
// Last changed at 2023-01-21                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>

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

	SetCasButtons(true, false, false, false);

	ui->cas_play_status->setText("");

	ui->remaining_time_label->setText(ConvertTimeToString(0));
	ui->remaining_time_label->setToolTip(tr("Remaining Time"));

	connect(transmitter, SIGNAL(CasIsEnd()), this, SLOT(OnCasIsEnd()));
	connect(transmitter, SIGNAL(ChangeProgress(int, int)), this, SLOT(OnChangeProgress(int, int)));
	connect(transmitter, SIGNAL(MessageLog(QString)), this, SLOT(OnMessageLog(QString)));

#ifdef _WIN32
	setWindowTitle("CasToSio Version " + QString(VERSION_STRING) + " --- [Windows " + QString(ARCHITECTURE_STRING) + "]");
#else
# ifdef __linux__
	setWindowTitle("CasToSio Version " + QString(VERSION_STRING) + " --- [Linux " + QString(ARCHITECTURE_STRING) + "]");
# else
#  ifdef __FreeBSD__
	setWindowTitle("CasToSio Version " + QString(VERSION_STRING) + " --- [FreeBSD " + QString(ARCHITECTURE_STRING) + "]");
#  else
	setWindowTitle("CasToSio Version " + QString(VERSION_STRING) + " --- [POSIX (unknown) " + QString(ARCHITECTURE_STRING) + "]");
#  endif
# endif
#endif
}

MainWindow::~MainWindow()
{
	transmitter->Stop();

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

				//std::cout << "Zip is Open. " << zip.getEntriesCount() << " [" << first_file_name.toLocal8Bit().data() << "]" << std::endl;

				QuaZipFile zf(&zip);
				if(!zip.setCurrentFile(first_file_name))
				{
					std::cout << tr("Error in zip file: ").toLocal8Bit().data() << first_file_name.toLocal8Bit().data() << std::endl;
					return;
				}

				if(!zf.open(QIODevice::ReadOnly))
				{
					std::cout << tr("Error when opening the zip file: ").toLocal8Bit().data() << first_file_name.toLocal8Bit().data();
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

				ui->filename_label->setText(first_file_name.toLocal8Bit().data());
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
		else
		{
			SetPlayTime();
		}

		if(is_zip)
		{
			QFile file(unzip_filename);
			if(file.exists())
				file.remove();
		}

		if(cas.IsOpen())
		{
			SetCasButtons(true, true, false, false);
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
		ui->log_list->clear();
		ui->log_list->scrollToBottom();

		transmitter->SetBaudRateFactor(ui->baudrate_spin->value() / 100.0f);
		transmitter->SetMaxIrgTime(ui->irg_time_spin->value());

		transmitter->cas = &cas;
		transmitter->progress_bar = ui->transmit_progress;
		transmitter->serial_port_name = ui->serial_ports->currentText();
		transmitter->start();

		SetCasButtons(false, false, true, true);
		ui->cas_play_status->setText(tr("playing..."));
	}
}

void MainWindow::SetPlayTime()
{
	if(cas.IsOpen())
	{
		int time = cas.GetPlayTime(ui->irg_time_spin->value(), ui->baudrate_spin->value() / 100.0f);
		current_playtime = time;
		ui->playtime_label->setText(ConvertTimeToString(current_playtime));
	}
}

QString MainWindow::ConvertTimeToString(int time_ms)
{
	time_ms /=100;

	int ss = time_ms % 10;
	time_ms /= 10;
	int s = time_ms % 60;
	time_ms /= 60;
	int m = time_ms % 60;
	time_ms /= 60;
	int h = time_ms % 60;
	time_ms /= 60;

	return QString::number(h).rightJustified(2, '0') + ":" + QString::number(m).rightJustified(2, '0') + ":" + QString::number(s).rightJustified(2, '0') + "." + QString::number(ss).rightJustified(1, '0');
}

void MainWindow::SetCasButtons(bool open_btn, bool start_btn, bool pause_btn, bool stop_btn)
{
	ui->cas_open_button->setDisabled(!open_btn);
	ui->cas_start_button->setDisabled(!start_btn);
	ui->cas_pause_button->setDisabled(!pause_btn);
	ui->cas_stop_button->setDisabled(!stop_btn);

	ui->actionOpen_CAS_Image_A8CAS->setDisabled(!open_btn);
}


void MainWindow::on_baudrate_spin_valueChanged(int arg1)
{
	SetPlayTime();
}


void MainWindow::on_irg_time_spin_valueChanged(int arg1)
{
	SetPlayTime();
}


void MainWindow::on_cas_pause_button_clicked()
{
	if(transmitter->TogglePause())
	{
		ui->cas_pause_button->setText(tr("Continue"));
		ui->cas_play_status->setText(tr("pause..."));
	}
	else
	{
		ui->cas_pause_button->setText(tr("Pause"));
		ui->cas_play_status->setText(tr("playing..."));
	}
}


void MainWindow::on_cas_stop_button_clicked()
{
	transmitter->Stop();
	ui->cas_pause_button->setText(tr("Pause"));
	ui->cas_play_status->setText("");
	SetCasButtons(true, true, false, false);
}


void MainWindow::on_actionExit_triggered()
{
	this->close();
}

void MainWindow::OnCasIsEnd()
{
	SetCasButtons(true, true, false, false);
	ui->transmit_progress->setValue(0);
	ui->cas_play_status->setText("");
}

void MainWindow::OnChangeProgress(int progress, int time_counter)
{
	ui->transmit_progress->setValue(progress);
	ui->remaining_time_label->setText(ConvertTimeToString(current_playtime - time_counter));
}

void MainWindow::OnMessageLog(QString message)
{
	ui->log_list->addItem(message);
	ui->log_list->scrollToBottom();
}

