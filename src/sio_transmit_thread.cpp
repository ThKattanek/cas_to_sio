//////////////////////////////////////////////////
//                                              //
// CasToSio                                     //
// by  Thorsten Kattanek                        //
//												//
// Atari Cas files send over sio bus with uart  //
//                                              //
// #file: sio_transmit_thread.cpp               //
//                                              //
// This source code is Copyright protected!     //
//                                              //
// Last changed at 2023-01-01                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#include "sio_transmit_thread.h"
#include <iostream>

SIOTransmitThread::SIOTransmitThread(QObject*)
{
	cas = nullptr;
	progress_bar = nullptr;
}

SIOTransmitThread::~SIOTransmitThread()
{
	thread_end = true;

	if(this->isRunning())
		this->wait(10000);
}

QStringList SIOTransmitThread::GetAllSerialPortNames()
{
	QStringList port_names;

	enum sp_return result = sp_list_ports(&port_list);

	if (result != SP_OK)
			printf("sp_list_ports() failed!\n");
	else {
		for(int i=0; port_list[i] != nullptr; i++)
		{
			struct sp_port *port = port_list[i];

			/* Get the name of the port. */
			char *port_name = sp_get_port_name(port);
			port_names += QString(port_name);
		}
	}

	return port_names;
}

void SIOTransmitThread::OnBytesWritten(qint64 bytes)
{
	std::cout << "Written Bytes: " << bytes << std::endl;
}

void SIOTransmitThread::InitSerialPort()
{

}

void SIOTransmitThread::run()
{
	if(cas == nullptr)
		return;

	thread_end = false;
	while(!thread_end)
	{
		thread_end = true;
	}

	/*
	serial_port = new QSerialPort(0);
	serial_port->setPortName(serial_port_name);

	connect(serial_port, SIGNAL(bytesWritten(qint64)),this,SLOT(OnBytesWritten(qint64)));

	if(serial_port->open(QIODevice::ReadWrite))
	{
		// Setup Serial Port
		if(serial_port->setBaudRate(600))
		{
			serial_port->baudRateChanged(600,QSerialPort::Direction::AllDirections);
			serial_port->setDataBits(QSerialPort::DataBits::Data8);
			serial_port->setParity(QSerialPort::Parity::NoParity);
			serial_port->setStopBits(QSerialPort::StopBits::OneStop);
			serial_port->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

			int i=0;

			char buffer[132];

			thread_end = false;
			while(!thread_end)
			{
				serial_port->write(buffer,132);
				serial_port->bytesWritten(12);
				//serial_port->waitForBytesWritten(-1);

				std::cout << "END" << std::endl;

				//serial_port->flush();
				//serial_port->waitForBytesWritten(1000);
				//while(serial_port->bytesToWrite() != 0){QThread::msleep(1);}


				if(progress_bar != nullptr)
					progress_bar->setValue(i);
				i++;
				if(i == 11)
					thread_end = true;
				//QThread::msleep(100);

				thread_end = true;
			}
		}

		serial_port->close();
	}
	else
	{
		std::cout << "Serial Port Error..." << serial_port->errorString().toStdString() << std::endl;
	}

	delete serial_port;
	*/
}
