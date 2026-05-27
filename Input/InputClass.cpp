#include "InputClass.h"
using namespace base;

InputClass::InputClass( HWND hWnd ) {
	_hWnd = hWnd;
	_pSystem = new InputSystem( hWnd );
	_pKeyboardDevice = new KeyboardDevice( _pSystem );
	_pJoystickDevice = new JoystickDevice( _pSystem );
}

InputClass::~InputClass() {
}


BOOL InputClass::Update() {
	return (_pKeyboardDevice->Update() && _pJoystickDevice->Update() );
}

BOOL InputClass::IsDown( const char* key ) {
	if( strstr( key, "joy" ) ) {
		return _pJoystickDevice->IsDown( key );
	}
	if( strstr( key, "key") ) {
		return _pKeyboardDevice->IsDown( key );
	}
	return FALSE;
}

BOOL InputClass::IsUp( const char* key ) {
	return !IsDown( key );
}