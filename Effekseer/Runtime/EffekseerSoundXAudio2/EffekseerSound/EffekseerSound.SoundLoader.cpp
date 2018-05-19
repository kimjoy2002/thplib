
//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include <string.h>
#include "EffekseerSound.SoundImplemented.h"
#include "EffekseerSound.SoundLoader.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerSound
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoader::SoundLoader()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
SoundLoader::~SoundLoader()
{
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void* SoundLoader::Load( const EFK_CHAR* path )
{
	FILE* fp = _wfopen((const wchar_t*)path, L"rb");
	if (fp == NULL) {
		return NULL;
	}

	uint32_t chunkIdent, chunkSize;
	// RIFFチャンクをチェック
	fread(&chunkIdent, 1, 4, fp);
	fread(&chunkSize, 1, 4, fp);
	if (memcmp(&chunkIdent, "RIFF", 4) != 0) {
		return NULL;
	}

	// WAVEシンボルをチェック
	fread(&chunkIdent, 1, 4, fp);
	if (memcmp(&chunkIdent, "WAVE", 4) != 0) {
		return NULL;
	}
	
	WAVEFORMATEX wavefmt = {0};
	for (;;) {
		fread(&chunkIdent, 1, 4, fp);
		fread(&chunkSize, 1, 4, fp);

		if (memcmp(&chunkIdent, "fmt ", 4) == 0) {
			// フォーマットチャンク
			uint32_t size = min(chunkSize, sizeof(wavefmt));
			fread(&wavefmt, 1, size, fp);
			if (size < chunkSize) {
				fseek(fp, chunkSize - size, SEEK_CUR);
			}
		} else if (memcmp(&chunkIdent, "data", 4) == 0) {
			// データチャンク
			break;
		} else {
			// 不明なチャンクはスキップ
			fseek(fp, chunkSize, SEEK_CUR);
		}
	}
	
	// フォーマットチェック
	if (wavefmt.wFormatTag != WAVE_FORMAT_PCM || wavefmt.nChannels > 2) {
		return NULL;
	}

	BYTE* buffer;
	uint32_t size;
	switch (wavefmt.wBitsPerSample) {
	case 8:
		// 16bitPCMに変換
		size = chunkSize * 2;
		buffer = new BYTE[size];
		fread(&buffer[size / 2], 1, chunkSize, fp);
		{
			int16_t* dst = (int16_t*)&buffer[0];
			uint8_t* src = (uint8_t*)&buffer[size / 2];
			for (uint32_t i = 0; i < chunkSize; i++) {
				*dst++ = (int16_t)(((int32_t)*src++ - 128) << 8);
			}
		}
		break;
	case 16:
		// そのまま読み込み
		buffer = new BYTE[chunkSize];
		size = fread(buffer, 1, chunkSize, fp);
		break;
	}

	fclose(fp);

	SoundData* soundData = new SoundData;
	memset(soundData, 0, sizeof(SoundData));
	soundData->channels = wavefmt.nChannels;
	soundData->sampleRate = wavefmt.nSamplesPerSec;
	soundData->buffer.Flags = XAUDIO2_END_OF_STREAM;
	soundData->buffer.AudioBytes = size;
	soundData->buffer.pAudioData = buffer;
	
	return soundData;
}
	
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
void SoundLoader::Unload( void* data )
{
	SoundData* soundData = (SoundData*)data;
	if (soundData == NULL) {
		return;
	}

	delete[] soundData->buffer.pAudioData;
	delete soundData;
}

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
}
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
