//////////////////////////////////////////////////
//                                              //
// CasToSio                                     //
// by  Thorsten Kattanek                        //
//												//
// Atari Cas files send over sio bus with uart  //
//                                              //
// #file: cas_file_class.h                      //
//                                              //
// This source code is Copyright protected!     //
//                                              //
// Last changed at 2022-12-29                   //
// https://github.com/ThKattanek/cas_to_sio     //
//                                              //
//////////////////////////////////////////////////

#ifndef CASFILECLASS_H
#define CASFILECLASS_H

#include <stdio.h>
#include <inttypes.h>
#include <string>

// Chunk Types
#define CHUNK_TYPE_FUJI 'IJUF'
#define CHUNK_TYPE_BAUD 'duab'
#define CHUNK_TYPE_DATA 'atad'
#define CHUNK_TYPE_FSK ' ksf'
#define CHUNK_TYPE_PWMS 'smwp'
#define CHUNK_TYPE_PWMC 'cmwp'
#define CHUNK_TYPE_PWMD 'dmwp'
#define CHUNK_TYPE_PWM1 '1mwp'

#define MAX_CHUNK_NUM 1024

struct CHUNK{
	uint32_t type = 0;
	uint16_t length = 0;
	uint8_t aux[2] = {0,0};
	uint8_t *data = nullptr;
};

class CASFileClass
{
public:
	CASFileClass();
	~CASFileClass();

	bool Open(FILE *file);
	void Close();
	bool IsOpen();
	std::string GetLastErrorString();

	int GetFujiChunkCount();
	int GetBaudChunkCount();
	int GetDataChunkCount();
	int GetFskChunkCount();
	int GetPWMSChunkCount();
	int GetPWMCChunkCount();
	int GetPWMDChunkCount();
	int GetPWM1ChunkCount();

	uint8_t *GetChunkDataPointer(int chunk_number);

private:
	int ReadNextChunk(FILE *file);	// return 0=ok, 1=end of file, -1=unknow_chunk, -2=wrong chunk size, -3=chunk count is greater as 1024
	void ClearChunk(int number);
	int GetChunkTypeCount(uint32_t chunk_type);

	std::string last_error_string;
	CHUNK chunk[MAX_CHUNK_NUM];
	int chunk_count;
};

#endif // CASFILECLASS_H
