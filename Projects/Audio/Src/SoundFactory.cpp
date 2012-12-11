#include "SoundFactory.h"

SoundFactory::SoundFactory(IXAudio2* p_soundDevice)
{
	m_soundDevice = p_soundDevice;
}

SoundFactory::~SoundFactory()
{

}
Sound* SoundFactory::createNonPositionalSound( const char* p_filePath )
{
	IXAudio2SourceVoice* soundVoice;
	WAVEFORMATEXTENSIBLE waveFormatEx;
	XAUDIO2_BUFFER buffer;
	ZeroMemory(&m_file, sizeof(HANDLE));
	initFile(p_filePath);

	fillBuffer(waveFormatEx,buffer);
	
	m_soundDevice->CreateSourceVoice( &soundVoice, (WAVEFORMATEX*)&waveFormatEx, 0,
		1.0f, NULL, NULL, NULL);

	Sound* newSound = new Sound(soundVoice,buffer);
	return newSound;
}


PositionalSound* SoundFactory::createPositionalSound( const char* p_filePath )
{
	IXAudio2SourceVoice* soundVoice;
	WAVEFORMATEXTENSIBLE waveFormatEx;
	XAUDIO2_BUFFER buffer;
	PositionalSound* newSound;

	/************************************************************************/
	/* PositionalSoundInfo should be sent into the sound factory.			*/
	/************************************************************************/
	PositionalSoundInfo info = PositionalSoundInfo(0,0,0);

	ZeroMemory(&m_file, sizeof(HANDLE));
	initFile(p_filePath);

	fillBuffer(waveFormatEx,buffer);

	m_soundDevice->CreateSourceVoice( &soundVoice, (WAVEFORMATEX*)&waveFormatEx, 0, 1.0f,
		NULL,NULL,NULL);
	newSound = new PositionalSound(soundVoice,buffer,info);
	return newSound;
}

void SoundFactory::fillBuffer( WAVEFORMATEXTENSIBLE& p_waveFormatEx, 
							  XAUDIO2_BUFFER& p_buffer )
{
	DWORD chunkSize, chunkPosition, fileType;
	findChunk( m_file, fourccRIFF, chunkSize, chunkPosition);

	readChunkData(m_file,&fileType,sizeof(DWORD), chunkPosition);

	if (fileType != fourccWAVE)
		throw XAudio2Exception("Unsupported sound format",__FILE__,__FUNCTION__,__LINE__);

	/*************************************************************************/
	/* Locate the fmt chunk and copy its contents into a WAVEFORMATEXTENSIBLE*/
	/*************************************************************************/
	findChunk(m_file,fourccFMT,chunkSize,chunkPosition);
	readChunkData(m_file,&p_waveFormatEx,chunkSize,chunkPosition);

	/************************************************************************/
	/* Locate the data chunk and read its contents into a buffer            */
	/************************************************************************/
	findChunk(m_file, fourccDATA,chunkSize,chunkPosition);
	BYTE* dataBuffer = new BYTE[chunkSize];
	readChunkData(m_file,dataBuffer,chunkSize, chunkPosition);

	/************************************************************************/
	/* HACK: Most options of the buffer should be sent into the function	*/
	/* rather than the hard coded way below									*/
	/************************************************************************/
	ZeroMemory(&p_buffer, sizeof(XAUDIO2_BUFFER));
	p_buffer.AudioBytes = chunkSize;
	p_buffer.pAudioData = dataBuffer;
	p_buffer.Flags = XAUDIO2_END_OF_STREAM;
	p_buffer.LoopBegin = 0;
	p_buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
}

void SoundFactory::findChunk(HANDLE hFile, DWORD fourcc, DWORD& dwChunkSize, 
							   DWORD& dwChunkDataPosition)
{
	HRESULT hr = S_OK;
	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
	{
		hr = HRESULT_FROM_WIN32( GetLastError() );
		throw XAudio2Exception(hr, __FILE__,__FUNCTION__,__LINE__);
	}
	DWORD dwChunkType;
	DWORD dwChunkDataSize;
	DWORD dwRIFFDataSize = 0;
	DWORD dwFileType;
	DWORD bytesRead = 0;
	DWORD dwOffset = 0;

	while (hr == S_OK)
	{
		DWORD dwRead;
		if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
		{
			hr = HRESULT_FROM_WIN32( GetLastError() );
			throw XAudio2Exception(hr, __FILE__,__FUNCTION__,__LINE__);
		}

		if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
		{
			hr = HRESULT_FROM_WIN32( GetLastError() );
			throw XAudio2Exception(hr, __FILE__,__FUNCTION__,__LINE__);
		}

		switch (dwChunkType)
		{
		case fourccRIFF:
			dwRIFFDataSize = dwChunkDataSize;
			dwChunkDataSize = 4;
			if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
				hr = HRESULT_FROM_WIN32( GetLastError() );
			break;

		default:
			if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, 
				NULL, FILE_CURRENT ) )
			{
				hr = HRESULT_FROM_WIN32( GetLastError() );
				throw XAudio2Exception(hr, __FILE__,__FUNCTION__,__LINE__);
			}
		}

		dwOffset += sizeof(DWORD) * 2;

		if (dwChunkType == fourcc)
		{
			dwChunkSize = dwChunkDataSize;
			dwChunkDataPosition = dwOffset;
			return;
		}

		dwOffset += dwChunkDataSize;

		if (bytesRead >= dwRIFFDataSize) 
			throw XAudio2Exception("Bytes to read is larger then file",__FILE__,
			__FUNCTION__,__LINE__);

	}
}
void SoundFactory::readChunkData(HANDLE hFile, void* buffer, DWORD bufferSize, 
								   DWORD bufferOffset)
{
	HRESULT hr = S_OK;
	if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferOffset, NULL, 
		FILE_BEGIN ) )
	{
		hr = HRESULT_FROM_WIN32( GetLastError() );
		throw XAudio2Exception(hr, __FILE__,__FUNCTION__,__LINE__);
	}
	DWORD dwRead;
	if( 0 == ReadFile( hFile, buffer, bufferSize, &dwRead, NULL ) )
	{
		hr = HRESULT_FROM_WIN32( GetLastError() );
		throw XAudio2Exception(hr, __FILE__,__FUNCTION__,__LINE__);
	}
}


void SoundFactory::initFile(string p_filePath)
{
	HRESULT hr = S_OK;
	m_file = CreateFile(p_filePath.c_str(),GENERIC_READ,FILE_SHARE_READ,NULL,
		OPEN_EXISTING,0,NULL);

	if (INVALID_HANDLE_VALUE == m_file)
	{
		hr = HRESULT_FROM_WIN32( GetLastError() );
		throw XAudio2Exception(hr, __FILE__,__FUNCTION__,__LINE__);
	}

	if ( INVALID_SET_FILE_POINTER == SetFilePointer(m_file,0,NULL,FILE_BEGIN) )
	{
		hr = HRESULT_FROM_WIN32( GetLastError() );
		throw XAudio2Exception(hr, __FILE__,__FUNCTION__,__LINE__);
	}
}

