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
// Last changed at 2023-01-04                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#include "sio_transmit_thread.h"
#include <iostream>

SIOTransmitThread::SIOTransmitThread(QObject*)
{
	cas = nullptr;
	progress_bar = nullptr;

	max_irg_time = 20000;
	baudrate_factor = 1.0f;
}

SIOTransmitThread::~SIOTransmitThread()
{
	thread_end = true;

	if(this->isRunning())
		this->wait(20000);
}

QStringList SIOTransmitThread::GetAllSerialPortNames()
{
	QStringList port_names;
	struct sp_port **port_list;

	enum sp_return result = sp_list_ports(&port_list);

	if (result != SP_OK)
			printf("sp_list_ports() failed!\n");
	else
	{
		int i;
		for(i=0; port_list[i] != nullptr; i++)
		{
			struct sp_port *port = port_list[i];

			/* Get the name of the port. */
			char *port_name = sp_get_port_name(port);
			port_names += QString(port_name);
		}
	}
	return port_names;
}

void SIOTransmitThread::SetBaudRateFactor(float baudrate_factor)
{
	this->baudrate_factor = baudrate_factor;
}

void SIOTransmitThread::SetMaxIrgTime(int max_irg_time)
{
	this->max_irg_time = max_irg_time;
}

float SIOTransmitThread::GetBaudRateFactor()
{
	return baudrate_factor;
}

int SIOTransmitThread::GetMaxIrgTime()
{
	return max_irg_time;
}

void SIOTransmitThread::OnBytesWritten(qint64 bytes)
{
	std::cout << "Written Bytes: " << bytes << std::endl;
}

bool SIOTransmitThread::OpenSerialPort()
{
	enum sp_return result;

	result = sp_get_port_by_name(serial_port_name.toLocal8Bit().data(), &port);
	if(result != SP_OK)
	{
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
		return false;
	}

	result = sp_open(port, SP_MODE_WRITE);
	if(result != SP_OK)
	{
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
		return false;
	}

	// Event set
	result = sp_new_event_set(&event_set);
	if(result != SP_OK)
	{
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
		return false;
	}

	result = sp_add_port_events(event_set, port, SP_EVENT_TX_READY);
	if(result != SP_OK)
	{
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
		return false;
	}

	// Configure Port
	result = sp_set_baudrate(port, 600);
	if(result != SP_OK)
	{
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
		return false;
	}

	result = sp_set_bits(port, 8);
	if(result != SP_OK)
	{
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
		return false;
	}

	result = sp_set_parity(port, SP_PARITY_NONE);
	if(result != SP_OK)
	{
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
		return false;
	}

	result = sp_set_stopbits(port, 1);
	if(result != SP_OK)
	{
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
		return false;
	}

	result = sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
	if(result != SP_OK)
	{
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
		return false;
	}

	return true;
}

void SIOTransmitThread::CloseSerialPort()
{
	enum sp_return result;

	result = sp_close(port);
	if(result != SP_OK)
		std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;

	sp_free_port(port);
}

void SIOTransmitThread::run()
{
	if(cas == nullptr)
		return;

	if(!OpenSerialPort())
		return;

	enum sp_return result;

	thread_end = false;
	while(!thread_end)
	{
		if(cas != nullptr)
		{
			CHUNK* chunk;
			uint16_t baudrate;
			uint16_t irg_time;

			int chunk_count = cas->GetChunkCount();

			int data_chunk_count = cas->GetDataChunkCount();
			int data_chunk_number = 0;

			int data_chunk_tranfer_time;

			progress_bar->setMaximum(data_chunk_count);

			int data_counter = 0;

			for(int i=0; i<chunk_count; i++)
			{
				switch(cas->GetChunkType(i))
				{
				case CHUNK_TYPE_FUJI:
					std::cout << "FUJI Chunk..." << std::endl;
					break;

				case CHUNK_TYPE_BAUD:
					chunk = cas->GetChunk(i);
					baudrate = chunk->aux[0] | (chunk->aux[1] << 8);

					baudrate *= baudrate_factor;

					std::cout << "Set Baudrate: " << baudrate << std::endl;

					result = sp_set_baudrate(port, baudrate);
					if(result != SP_OK)
						std::cout << "LibSerialPort: " << sp_last_error_message() << std::endl;
					break;

				case CHUNK_TYPE_DATA:
					chunk = cas->GetChunk(i);
					irg_time = chunk->aux[0] | (chunk->aux[1] << 8);

					if(irg_time > max_irg_time)
						irg_time = max_irg_time;

					std::cout << "Data CHunk (" << data_chunk_number+1 << "/" << data_chunk_count << ") - Inter-Record Gap: " << irg_time << "ms" << std::endl;
					data_chunk_number++;

					QThread::msleep(irg_time);

					data_chunk_tranfer_time = (chunk->length * 10000) / baudrate;
					sp_blocking_write(port, chunk->data, chunk->length, data_chunk_tranfer_time * 2);

					QThread::msleep(data_chunk_tranfer_time);

					break;

				case CHUNK_TYPE_FSK:
					std::cout << "FSK Chunk..." << std::endl;
					break;

				case CHUNK_TYPE_PWM1:
					std::cout << "PWM1 Chunk..." << std::endl;
					break;

				case CHUNK_TYPE_PWMC:
					std::cout << "PWMC Chunk..." << std::endl;
					break;

				case CHUNK_TYPE_PWMD:
					std::cout << "PWMD Chunk..." << std::endl;
					break;

				case CHUNK_TYPE_PWMS:
					std::cout << "PWMS Chunk..." << std::endl;
					break;

				default:
					std::cout << "Unknow Chunk..." << std::endl;
					break;
				}

				progress_bar->setValue(data_counter++);
				if(thread_end)
					break;
			}
		}
		thread_end = true;
	}

	CloseSerialPort();
}
