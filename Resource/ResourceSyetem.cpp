#include "ResourceSystem.h"
using namespace base;

ResourceSystem::ResourceSystem(const char *path)
:_path(path), _isArchive(false)
{
	if(!LoadLibrary()){
		printf("�x���FUNZIP32.DLL��������܂���BZIP���ɂ͒T���܂���\n");
	}
	if( !InitializeFunction() ) return;

	HARC hArc = _fpUnZipOpenArchive(NULL, path, M_REGARDLESS_INIT_FILE
		| M_CHECK_ALL_PATH
		| M_ERROR_MESSAGE_OFF
		| M_BAR_WINDOW_OFF);
	_isArchive = (hArc != NULL);
	if( _isArchive ){
		_fpUnZipCloseArchive(hArc);
	}
}

ResourceSystem::~ResourceSystem() {
	if( _hLib ) {
		::FreeLibrary( _hLib );
	}
}

BOOL ResourceSystem::LoadLibrary() {
	_hLib = ::LoadLibrary( "UNZIP32.DLL");
	if( _hLib == NULL ) {
		return FALSE;
	}
	return TRUE;
}


BOOL ResourceSystem::InitializeFunction() {
	_fpUnZipExtractMem = 0;
	_fpUnZipFindFirst = 0;
	_fpUnZipOpenArchive = 0;
	_fpUnZipCloseArchive = 0;

	_fpUnZipExtractMem = (typeUnZipExtractMem)GetProcAddress(_hLib, "UnZipExtractMem");
	if(!_fpUnZipExtractMem) return FALSE;

	_fpUnZipFindFirst = (typeUnZipFindFirst)GetProcAddress(_hLib, "UnZipFindFirst");
	if(!_fpUnZipFindFirst) return FALSE;

	_fpUnZipOpenArchive = (typeUnZipOpenArchive)GetProcAddress(_hLib, "UnZipOpenArchive");
	if(!_fpUnZipOpenArchive) return FALSE;

	_fpUnZipCloseArchive = (typeUnZipCloseArchive)GetProcAddress(_hLib, "UnZipCloseArchive");
	if(!_fpUnZipCloseArchive) return FALSE;
	return TRUE;
}

//	r = (*fpTestFunc)(NULL,"c:�_�_test test/test2.jpg", buf, 100, &lpTime, NULL, &lpdwWriteSize);

DWORD ResourceSystem::GetFileSize(LPCSTR szFileName)
{
	if( _isArchive ){
		// �A�[�J�C�u���[�h
		if( ! _fpUnZipOpenArchive ){
			printf("�x���F���ɗpDLL���L���ł͂���܂���\n");
			return 0;
		}
		int retval;
		HARC hArc;
		hArc = _fpUnZipOpenArchive(NULL, _path.c_str(), M_REGARDLESS_INIT_FILE
			| M_CHECK_ALL_PATH
			| M_ERROR_MESSAGE_ON
			| M_BAR_WINDOW_OFF);
		INDIVIDUALINFO subInfo;
		memset(&subInfo, 0, sizeof(subInfo));
		retval = _fpUnZipFindFirst(hArc, szFileName, &subInfo);
		_fpUnZipCloseArchive(hArc);

		return subInfo.dwOriginalSize;
	}
	else{
		// �����\�[�X���[�h
		DWORD result;
		HANDLE hFile = CreateFile(AddPath(szFileName).c_str(), GENERIC_READ, 0,NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE){
			return 0;
		}
		result = ::GetFileSize(hFile, NULL);
		CloseHandle(hFile);
		return result;
	}
}

BOOL ResourceSystem::ReadIn(LPCSTR szFileName, LPBYTE szBuffer,
								const DWORD dwSize, LPDWORD lpdwWriteSize)
{
	if( _isArchive ){
		// �A�[�J�C�u���[�h
		if( ! _fpUnZipExtractMem ){
			printf("�x���F���ɗpDLL���L���ł͂���܂���\n");
			return FALSE;
		}
		string command = string(" --i ") + _path + " " + szFileName;
		int retval = _fpUnZipExtractMem(NULL, command.c_str(), szBuffer, dwSize,
			NULL, NULL, lpdwWriteSize);
		return (retval == 0);
	}
	else{
		// �����\�[�X���[�h
		HANDLE hFile = CreateFile(AddPath(szFileName).c_str(), GENERIC_READ, 0,NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(hFile == INVALID_HANDLE_VALUE){
			return FALSE;
		}
		SetFilePointer(hFile, 0, 0, FILE_BEGIN);
		BOOL result = ReadFile(hFile, (LPVOID)szBuffer, ::GetFileSize(hFile,NULL), lpdwWriteSize, NULL);

		CloseHandle(hFile);
		return result;
	}
}

string ResourceSystem::AddPath(const char *filename)
{
	return (_path+"/"+filename);
}
