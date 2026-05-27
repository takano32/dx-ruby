#pragma once

#include <windows.h>
#include <dsound.h>
#include "SoundSystem.h"
#include "SoundACM.h"
#include "../File.h"

#define FADE_INTERVAL 100
#define VOLUME_MAX 100
#define VOLUME_MIN 0

namespace base{
	class SoundBuffer{
	private:
		File* _pFile;
		SoundACM* _pSoundACM;
		LPDIRECTSOUND			_pSoundObject;
		BYTE					*_pLoadBuffer;
		DWORD					_length;
		LPDIRECTSOUNDBUFFER		_pBuffer;	// �o�b�t�@
		WAVEFORMATEX* _pFormat;

		HMMIO _hMmio;
		MMCKINFO _mmckParent, _mmckChild;

		BOOL OpenACM();
		BOOL Create();
		
		BOOL Fill( BYTE* data );
		BOOL FillWithACM();
		BOOL Read();
		BOOL ReadWithACM();
	public:
		SoundBuffer( SoundSystem* pSystem );
		~SoundBuffer();

		int GetVolume();
		BOOL SetVolume( int volume );

		BOOL SetPosition( int position );
		BOOL Play( BOOL with_loop );
		BOOL FadeIn( int fade_msec, BOOL with_loop );
		BOOL FadeOut( int fade_msec );

		BOOL Stop();
		
		BOOL Load( BYTE* data, int size );
		BOOL Open( char* file_name );
		BOOL Close();
	};
}

