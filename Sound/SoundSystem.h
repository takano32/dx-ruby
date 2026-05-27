
#pragma once


#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ddraw.h>
#include <mmsystem.h>
#include <dsound.h>


#define	RELEASE( obj )	if( obj != NULL ){ obj->Release(); obj = NULL; }

namespace base{
	class SoundSystem{
		static int __channels;
		static int __sample_rate;
		static int __bits_per_sample;
		LPDIRECTSOUND			_pSoundObject;
		LPDIRECTSOUNDBUFFER		_pPrimaryBuffer;
		HWND _hWnd;
	private:
		BOOL InitializeSystem();
		BOOL InitializeSound();
		BOOL InitializeCooperativeLevel();
		BOOL InitializeSpeaker();
		BOOL InitializeSoundBuffer();
		BOOL InitializeWaveFormat();
	public:
		SoundSystem( HWND hWnd );
		~SoundSystem();
		LPDIRECTSOUND GetSoundObject();
	};
}


