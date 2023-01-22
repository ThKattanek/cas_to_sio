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
// Last changed at 2023-01-22                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#include "sio_transmit_thread.h"
//#include <iostream>

#define log(msg) MessageLog(msg)

SIOTransmitThread::SIOTransmitThread(QObject*)
{
	port = nullptr;
	cas = nullptr;
	pause = false;

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

bool SIOTransmitThread::TogglePause()
{
	pause = !pause;
	return pause;
}

void SIOTransmitThread::Stop()
{
	pause = false;
	thread_end = true;
}

bool SIOTransmitThread::OpenSerialPort()
{
	enum sp_return result;

	result = sp_get_port_by_name(serial_port_name.toLocal8Bit().data(), &port);
	if(result != SP_OK)
	{
		emit log("LibSerialPort: " + QString(sp_last_error_message()));
		return false;
	}

	result = sp_open(port, SP_MODE_WRITE);
	if(result != SP_OK)
	{
		emit log("LibSerialPort: " + QString(sp_last_error_message()));
		return false;
	}

	// Event set
	result = sp_new_event_set(&event_set);
	if(result != SP_OK)
	{
		emit log("LibSerialPort: " + QString(sp_last_error_message()));
		return false;
	}

	result = sp_add_port_events(event_set, port, SP_EVENT_TX_READY);
	if(result != SP_OK)
	{
		emit log("LibSerialPort: " + QString(sp_last_error_message()));
		return false;
	}

	// Configure Port
	result = sp_set_baudrate(port, 600);
	if(result != SP_OK)
	{
		emit log("LibSerialPort: " + QString(sp_last_error_message()));
		return false;
	}

	result = sp_set_bits(port, 8);
	if(result != SP_OK)
	{
		emit log("LibSerialPort: " + QString(sp_last_error_message()));
		return false;
	}

	result = sp_set_parity(port, SP_PARITY_NONE);
	if(result != SP_OK)
	{
		emit log("LibSerialPort: " + QString(sp_last_error_message()));
		return false;
	}

	result = sp_set_stopbits(port, 1);
	if(result != SP_OK)
	{
		emit log("LibSerialPort: " + QString(sp_last_error_message()));
		return false;
	}

	result = sp_set_flowcontrol(port, SP_FLOWCONTROL_NONE);
	if(result != SP_OK)
	{
		emit log("LibSerialPort: " + QString(sp_last_error_message()));
		return false;
	}

	return true;
}

void SIOTransmitThread::CloseSerialPort()
{
	enum sp_return result;

	result = sp_close(port);
	if(result != SP_OK)
		emit log("LibSerialPort: " + QString(sp_last_error_message()));

	sp_free_port(port);

	port = nullptr;
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
			QString message;
			CHUNK* chunk;
			uint16_t baudrate = 600;
			uint16_t irg_time;

			int chunk_count = cas->GetChunkCount();
			int time_counter = 0;
			int time_counter_tmp = 0;

			int data_chunk_count = cas->GetDataChunkCount();
			int data_chunk_number = 0;

			uint16_t data_chunk_tranfer_time;

			for(int i=0; i<chunk_count; i++)
			{
				while(pause)
					QThread::msleep(1);

				switch(cas->GetChunkType(i))
				{
				case CHUNK_TYPE_FUJI:
					emit MessageLog("FUJI Chunk...");
					break;

				case CHUNK_TYPE_BAUD:
					chunk = cas->GetChunk(i);
					baudrate = chunk->aux[0] | (chunk->aux[1] << 8);

					baudrate *= baudrate_factor;

					emit log("Set Baudrate: " + QString::number(baudrate));

					result = sp_set_baudrate(port, baudrate);
					if(result != SP_OK)
						emit log("LibSerialPort: " + QString(sp_last_error_message()));
					break;

				case CHUNK_TYPE_DATA:
					chunk = cas->GetChunk(i);
					irg_time = chunk->aux[0] | (chunk->aux[1] << 8);

					if(irg_time > max_irg_time)
						irg_time = max_irg_time;

					emit log("Data Chunk (" + QString::number( data_chunk_number+1) + "/" + QString::number(data_chunk_count) + ") - Inter-Record Gap: " + QString::number(irg_time) + "ms");
					data_chunk_number++;

					time_counter_tmp = time_counter;

					for(uint32_t i=0; i < irg_time / 10; i++)
					{
						time_counter_tmp += 10;
						emit ChangeProgress(time_counter_tmp, time_counter_tmp);
						QThread::msleep(10);
						if(thread_end)
							break;
					}

					time_counter += irg_time;

					if(!thread_end)
					{
						data_chunk_tranfer_time = (chunk->length * 10000) / baudrate;
						sp_blocking_write(port, chunk->data, chunk->length, data_chunk_tranfer_time * 2);

						time_counter_tmp = time_counter;

						for(uint32_t i=0; i < data_chunk_tranfer_time / 10; i++)
						{
							time_counter_tmp += 10;
							emit ChangeProgress(time_counter_tmp, time_counter_tmp);
							QThread::msleep(10);
							if(thread_end)
								break;
						}

						time_counter += data_chunk_tranfer_time;
					}
					break;

				case CHUNK_TYPE_FSK:
					emit log("FSK Chunk...");
					break;

				case CHUNK_TYPE_PWM1:
					emit log("PWM1 Chunk...");
					break;

				case CHUNK_TYPE_PWMC:
					emit log("PWMC Chunk...");
					break;

				case CHUNK_TYPE_PWMD:
					emit log("PWMD Chunk...");
					break;

				case CHUNK_TYPE_PWMS:
					emit log("PWMS Chunk...");
					break;

				default:
					emit log("Unknow Chunk...");
					break;
				}

				emit ChangeProgress(time_counter_tmp, time_counter);

				if(thread_end)
					break;
			}
			emit CasIsEnd();
		}
		thread_end = true;
	}
	CloseSerialPort();
}
