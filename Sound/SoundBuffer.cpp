#include "SoundBuffer.h"

#include "../Log.h"
#include "SoundACM.h"
#include <math.h>

using namespace base;

SoundBuffer::SoundBuffer( SoundSystem* pSystem ) {
	_pSoundObject = pSystem->GetSoundObject();
	_pBuffer      = NULL;   // 未初期化防止: Load() 失敗時も NULL を保証する
	_pLoadBuffer  = NULL;
	_pFormat = new WAVEFORMATEX();
	_pFile = new File();
	_pSoundACM = new SoundACM();
}

SoundBuffer::~SoundBuffer() {
	Close();
	delete _pFile;
	delete _pSoundACM;
	delete _pFormat;
	_pFormat = NULL;
}

int SoundBuffer::GetVolume() {
	if( !_pBuffer ) return -1;
	long tmp;
	HRESULT ret;
	ret = _pBuffer->GetVolume( &tmp );
	double volume = (double)tmp;
	volume /= -100;
	volume = 100 - 50 * log10(volume);
	if( ret == DS_OK ) {
		return (int)volume;
	}
	return -1;
}

BOOL SoundBuffer::SetVolume( int volume ) {
	if( !_pBuffer ) return FALSE;
	double value = pow(100, 1.0 - (double)volume / 100.0);
	value *= -100;

	HRESULT hr = _pBuffer->SetVolume( (long)value );
	if( hr != DS_OK ) {
		return FALSE;
	}
	return TRUE;
}



BOOL SoundBuffer::Close() {
	_pFile->Close();
	_pSoundACM->Close();

	// �J��
	if( _pBuffer != NULL ){
		if( _pBuffer->Release() != DS_OK ){
			MessageBox( GetActiveWindow(), "�o�b�t�@�̊J���Ɏ��s���܂���", "ERROR", MB_OK );
			return FALSE;		// ���s
		}
		_pBuffer = NULL;
	}
	
	// ����
	return TRUE;
}


BOOL SoundBuffer::Load( BYTE* data, int size ) {
	MMIOINFO mmio;
	ZeroMemory(&mmio,sizeof(mmio));
	mmio.pchBuffer = (LPSTR)data;
	mmio.fccIOProc = FOURCC_MEM;
	mmio.cchBuffer = size;


	_hMmio = mmioOpen(NULL,&mmio,MMIO_READ);
	if( _hMmio == NULL ){
		Log::Error( "MultiMediaIO::MultiMediaIO �������̓ǂݍ��݂Ɏ��s���܂���" );
		return FALSE;
	}
	_mmckParent.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	if( mmioDescend( _hMmio, &_mmckParent, NULL, MMIO_FINDRIFF ) ){
		mmioClose( _hMmio, 0 );
		// MP3�ł���\���������D
		if( _pSoundACM->Open((BYTE*)data, (DWORD)size ) == 0 ) {
			return ReadWithACM();
		}
		return FALSE;
	}

	_mmckChild.ckid = mmioFOURCC('f', 'm', 't', ' ');
	if( mmioDescend( _hMmio, &_mmckChild, &_mmckParent, MMIO_FINDCHUNK ) ){
		mmioClose( _hMmio, 0 );
		return FALSE;
	}


	// WAVE�t�@�C������WAVE�t�H�[�}�b�g��ǂݎ��܂�
	if( mmioRead( _hMmio, (char *)_pFormat, sizeof( WAVEFORMATEX ) ) != sizeof( WAVEFORMATEX ) ){
		// fmt�`�����N��ǂݎ�邱�Ƃ��ł��܂���
		mmioClose( _hMmio, 0 );		// ����
		return FALSE;				// ���s
	}
	
	// PCM�t�H�[�}�b�g��WAVE���������܂���
	if( _pFormat->wFormatTag != WAVE_FORMAT_PCM ){
		// WAVE�t�@�C����PCM�t�H�[�}�b�g�ł͂���܂���
		mmioClose( _hMmio, 0 );		// ����
		return FALSE;				// ���s
	}
	
	// �A�Z���h
	if( mmioAscend( _hMmio, &_mmckChild, 0 ) ){
		// �A�Z���h�ł��܂���
		mmioClose( _hMmio, 0 );		// ����
		return FALSE;				// ���s
	}
	
	// data�`�����N�ւ̃f�B�Z���h
	_mmckChild.ckid = mmioFOURCC('d', 'a', 't', 'a');
	if( mmioDescend( _hMmio, &_mmckChild, &_mmckParent, MMIO_FINDCHUNK ) ){
		// WAVE�t�@�C����data�`�����N������܂���
		mmioClose( _hMmio, 0 );		// ����
		return FALSE;				// ���s
	}
		
	return Read();
}


BOOL SoundBuffer::Open( char* file_name ) {
	_pFile->Open( file_name );
	return Load( (BYTE*)_pFile->GetMemory(), _pFile->GetSize() );
}



BOOL SoundBuffer::ReadWithACM() {
	//_length��_pFormat�̐ݒ�
	if (_pSoundACM->IsOpen()) {	//	����MP3�t�@�C�����I�[�v�����Ă���
		_length	= _pSoundACM->GetSize();	//	acm����ϊ���̃T�C�Y�𓾂�
		_pFormat	= _pSoundACM->GetFormat();	//	acm����ϊ���̃t�H�[�}�b�g�𓾂�
	}

		// �t�@�C�������
	mmioClose( _hMmio, 0 );
	
	if( Create() == FALSE ) {
		return FALSE;
	}
		  
	// �o�b�t�@�ɏ�������
	if( FillWithACM() == FALSE ){
		return FALSE;				// ���s
	}
	return TRUE;
}



BOOL SoundBuffer::FillWithACM() {
	// DirectSound�o�b�t�@��Lock
	BYTE* lpDSBuffData;
	LRESULT hr;
	hr = _pBuffer->Lock(0, _length, (void**)&lpDSBuffData,
		&_length, NULL, 0, 0);
		// ����́A���́A���s���邱�Ƃ͑��X�L��̂�:p

	if (hr==DSERR_BUFFERLOST){
		_pBuffer->Restore(); // ����ŃI�b�P!�i�΁j
		hr = _pBuffer->Lock(0, _length, (void**)&lpDSBuffData,
			&_length, NULL, 0, 0);
		// ��ŁA�����������g���C����́I�I
	}

	if (hr!=DS_OK) {
		// ����Ń_���Ȃ�A�����������̂��Ⴄ�H
		Log::Error("Sound::FillWithACM()�T�E���h��Lock()�Ɏ��s�I");
		return 8;
	}

	if (_pSoundACM->IsOpen()) {
		//	acm���g���Ȃ�΁Aacm��Lock�����o�b�t�@�|�C���^��n����
		//	���ڂ����ɕϊ����Ă��炤�B�i�Ȃ�ł݂�Ȃ������[�ւ�́H�j
		if (_pSoundACM->Convert(lpDSBuffData)!=0) {
			Log::Error("Sound::Acm�ł̕ϊ��Ɏ��s");
			return 9;
		};
	} else {
		// WaveData��DirectSound�o�b�t�@�ɓ]��
		//CopyMemory(lpDSBuffData,(LPVOID)pWaveData,dwDataLen);
		return FALSE;
	}

	// DirectSound�o�b�t�@��Unlock...
	if (_pBuffer->Unlock(lpDSBuffData, _length, NULL, 0)
		!= DS_OK) {
		// ����Ȃ�ӂ[�A���s���邩��...�ǂȂ����[����[�񂶂�
		Log::Error("Sound::Load��Unlock()�Ɏ��s�I");
		return 10;
	}

	return TRUE;
}



BOOL SoundBuffer::Read( ) {
	// WAVE �f�[�^�ǂݍ��݂悤�̕ϐ����쐬����
	BYTE* data = new BYTE[ _mmckChild.cksize];		// 
	_length = _mmckChild.cksize;					// ���̒����̕ۑ�
	if( data == NULL ){					// �̈�쐬���s
		return FALSE;				// ���s
	}
	// wave �f�[�^��ǂݎ��܂�
	if( ( DWORD)mmioRead( _hMmio, ( char*)data, _mmckChild.cksize ) != _mmckChild.cksize ){
		// data�`�����N��ǂݎ�邱�Ƃ��ł��܂���
		mmioClose( _hMmio, 0 );		// ����
		return FALSE;				// ���s
	}
	
	// �t�@�C�������
	mmioClose( _hMmio, 0 );
	
	if( Create() == FALSE ) {
		return FALSE;
	}
		  
	// �o�b�t�@�ɏ�������
	if( Fill( data ) == FALSE ){
		return FALSE;				// ���s
	}

	delete[] data;
		  
	return TRUE;
}




BOOL SoundBuffer::Fill( BYTE* data ) {
	LPVOID	write1;
	DWORD	length1;
	LPVOID	write2;
	DWORD	length2;
	HRESULT hr;
	
	// �o�b�t�@���������ނ��߂Ƀ��b�N������i ��x���s���Ă�������x�s�� �j
	hr = _pBuffer->Lock( 0, _length , &write1, &length1, &write2, &length2, 0 );
	if( hr == DSERR_BUFFERLOST){
		_pBuffer->Restore();
		hr = _pBuffer->Lock( 0, _length, &write1, &length1, &write2, &length2, 0 );
	}
	// �Q�x�ڃ��b�N���s�Ȃ�Ύ��s�ɂ���
	if( hr != DS_OK ){
		return FALSE;
	}
	
	// �������� write1 �ɏ������ݐ�̃A�h���X�������Ă���
	CopyMemory( write1, data, length1 );
	
	// ��x�ڂŏ������߂Ȃ�������Q�x�ڂ���������
	if( write2 != NULL ){
		CopyMemory( write2, data + length1, length2 );
	}
	
	// ���b�N����
	hr = _pBuffer->Unlock( write1, length1, write2, length2 );
	if( hr != DS_OK ){
		return FALSE;
	}
	
	// ����
	return TRUE;
}


BOOL SoundBuffer::Create( ) {
	DSBUFFERDESC	dsbdesc;
	HRESULT			hr;

	
	// �\���̂̏�����
	ZeroMemory( &dsbdesc, sizeof( DSBUFFERDESC ) );
	dsbdesc.dwSize			= sizeof( DSBUFFERDESC );
	dsbdesc.dwBufferBytes	= _length;
	dsbdesc.lpwfxFormat 	= _pFormat;
	dsbdesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLPAN | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC | DSBCAPS_GLOBALFOCUS; 

	// �o�b�t�@�̍쐬
	hr = _pSoundObject->CreateSoundBuffer( &dsbdesc, &_pBuffer, NULL );
	if( hr != DS_OK ){
		return FALSE;
	}

	SetPosition( 0 );
	SetVolume( VOLUME_MAX );
	
	// ����
	return TRUE;
}



BOOL SoundBuffer::SetPosition( int position ) {
	if( !_pBuffer ) return FALSE;
	HRESULT hr = _pBuffer->SetCurrentPosition( position );
	if( hr != DS_OK ) {
		return FALSE;
	}
	return TRUE;

}

BOOL SoundBuffer::Play( BOOL with_loop ) {
	if( !_pBuffer ) return FALSE;
	static	long	loop_states[] = { 0, DSBPLAY_LOOPING, };	// ���[�v��ԗp
	HRESULT hr;													// ����

	// �Đ�
	hr = _pBuffer->Play( 0, 0, loop_states[ with_loop] );
	if( hr != DS_OK ){
		_pBuffer->Restore();
	}
	if( FAILED(hr) ) {
		return FALSE;
	}else{
		return TRUE;
	}
}



BOOL SoundBuffer::Stop()
{
	if( !_pBuffer ) return FALSE;
	HRESULT hr = _pBuffer->Stop();
	if( hr != DS_OK ) {
		return FALSE;
	}
	return TRUE;
}


BOOL SoundBuffer::FadeIn( int fade_msec, BOOL with_loop) {
	if( SetVolume( VOLUME_MIN ) == FALSE ) return FALSE;

	double increase = (double)(VOLUME_MAX - GetVolume());
	increase /= fade_msec;
	increase *= FADE_INTERVAL;
	double volume = (double)GetVolume();

	Play( with_loop );
	
	while( volume + increase < VOLUME_MAX ) {
		volume += increase;
		if( SetVolume( (int)volume ) == FALSE ) return FALSE;
		Sleep( FADE_INTERVAL );
	}
	
	return SetVolume( VOLUME_MAX );
}

BOOL SoundBuffer::FadeOut( int fade_msec ) {
	double decrease = (double)(GetVolume() - VOLUME_MIN);
	decrease /= fade_msec;
	decrease *= FADE_INTERVAL;
	double volume = (double)GetVolume();
	
	while( VOLUME_MIN < volume - decrease ) {
		volume -= decrease;
		if( SetVolume( (int)volume ) == FALSE ) return FALSE;
		Sleep( FADE_INTERVAL );
	}
	SetVolume( VOLUME_MIN );
	return Stop();
}

