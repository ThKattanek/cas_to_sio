//////////////////////////////////////////////////
//                                              //
// CasToSio                                     //
// by  Thorsten Kattanek                        //
//												//
// Atari Cas files send over sio bus with uart  //
//                                              //
// #file: cas_file_class.cpp                    //
//                                              //
// This source code is Copyright protected!     //
//                                              //
// Last changed at 2022-12-29                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#include "cas_file_class.h"
#include <iostream>

CASFileClass::CASFileClass()
{
	last_error_string = "";
	chunk_count = 0;
}

CASFileClass::~CASFileClass()
{
	Close();
}

bool CASFileClass::Open(FILE *file)
{
	int ret;

	for(int i=0; i<chunk_count; i++)
		ClearChunk(i);
	chunk_count = 0;

	do{
		ret = ReadNextChunk(file);
	}while(ret == 0);

	switch(ret)
	{
	case -1:
		last_error_string = "CAS Read Error: Unknow Chunk Type.";
		Close();
		fclose(file);
		return false;
	break;

	case -2:
		last_error_string = "CAS Read Error: Wrong chunk size.";
		Close();
		fclose(file);
		return false;
	break;

	case -3:
		last_error_string = "CAS Read Error: Chunk count is greater as ";
		last_error_string += std::to_string(MAX_CHUNK_NUM);
		last_error_string += ".";
		Close();
		fclose(file);
		return false;
	break;
	}

	fclose(file);
	return true;
}

void CASFileClass::Close()
{
	for(int i=0; i<chunk_count; i++)
		ClearChunk(i);
	chunk_count = 0;
}

bool CASFileClass::IsOpen()
{
	if(chunk_count > 0)
		return true;
	else
		return false;
}

std::string CASFileClass::GetLastErrorString()
{
	return last_error_string;
}

int CASFileClass::ReadNextChunk(FILE *file)
{
	uint32_t chunk_type;
	uint16_t chunk_length;
	uint8_t chunk_aux[2];
	uint8_t* chunk_data;

	if(chunk_count == MAX_CHUNK_NUM)
		return -3;

	if(fread(&chunk_type, 4, 1, file) != 1)
		return 1;

	if(chunk_type != CHUNK_TYPE_FUJI && chunk_type != CHUNK_TYPE_BAUD && chunk_type != CHUNK_TYPE_DATA && chunk_type != CHUNK_TYPE_FSK && chunk_type != CHUNK_TYPE_PWMC && chunk_type != CHUNK_TYPE_PWMD && chunk_type != CHUNK_TYPE_PWMS && chunk_type != CHUNK_TYPE_PWM1)
		return -1;

	if(fread(&chunk_length, 2, 1, file) != 1)
		return -2;

	if(fread(chunk_aux, 2, 1, file) != 1)
		return -2;

	chunk_data = new uint8_t[chunk_length];
	if(fread(chunk_data, 1, chunk_length, file) != chunk_length)
		return -2;

	chunk[chunk_count].type = chunk_type;
	chunk[chunk_count].length = chunk_length;
	chunk[chunk_count].aux[0] = chunk_aux[0];
	chunk[chunk_count].aux[1] = chunk_aux[1];
	chunk[chunk_count].data = chunk_data;

	chunk_count++;

	return 0;
}

void CASFileClass::ClearChunk(int number)
{
	chunk[number].type = 0;
	chunk[number].length = 0;
	chunk[number].aux[0] = 0;
	chunk[number].aux[1] = 0;

	if(chunk[number].data != nullptr)
	{
		delete[] chunk[number].data;
		chunk[number].data = nullptr;
	}
}
