#include <ruby.h>

#include "Sound.h"
#include "SoundClass.h"



struct SoundData {
	SoundClass* klass;
	VALUE sound_data;
};

static void sound_dfree( struct SoundData* data ) {
	if( data->klass ) {
		//TODO: delete����Ɨ����邱�Ƃ�����
		//delete data->klass;
		data->klass = NULL;
	}
}

static VALUE sound_alloc(VALUE klass) {
    struct SoundData *data = ALLOC(struct SoundData);
    return Data_Wrap_Struct(klass, 0, sound_dfree , data);
}

static VALUE sound_initialize( VALUE self, VALUE window ) {
    struct SoundData *data;
	HWND hWnd = (HWND)NUM2LONG(rb_funcall( window, rb_intern("handle") , 0 ));
	Data_Get_Struct(self, struct SoundData, data);
	data->klass = new SoundClass( hWnd );
    return Qnil;
}

static VALUE sound_close( VALUE self, VALUE id ) {
	struct SoundData *data;
	Data_Get_Struct(self, struct SoundData, data);
    
	BOOL ret = data->klass->Close( NUM2INT(id) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE sound_load( VALUE self, VALUE id, VALUE sound_data ) {
	struct SoundData *data;
	Data_Get_Struct(self, struct SoundData, data);
    
	data->sound_data = sound_data;
	BOOL ret = data->klass->Load( NUM2INT(id),
		(BYTE*)RSTRING_PTR(sound_data), (int)RSTRING_LEN(sound_data) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE sound_open( VALUE self, VALUE id, VALUE name ) {
	struct SoundData *data;
	Data_Get_Struct(self, struct SoundData, data);
    
	StringValue(name);
	BOOL ret = data->klass->Open( NUM2INT(id), RSTRING_PTR(name) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE sound_rewind( VALUE self, VALUE id ) {
	struct SoundData *data;
	Data_Get_Struct(self, struct SoundData, data);
	BOOL ret = data->klass->SetPosition( NUM2INT(id), 0 );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE sound_play( VALUE self, VALUE id, VALUE with_loop ) {
	struct SoundData *data;
	Data_Get_Struct(self, struct SoundData, data);
    BOOL loop = with_loop == Qtrue? TRUE:FALSE;
	BOOL ret = data->klass->Play( NUM2INT(id), loop);
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE sound_play_with_fade( VALUE self, VALUE id, VALUE fade_msec, VALUE with_loop ) {
	struct SoundData *data;
	Data_Get_Struct(self, struct SoundData, data);
    BOOL loop = with_loop == Qtrue? TRUE:FALSE;
	BOOL ret = data->klass->PlayWithFade( NUM2INT(id), NUM2INT(fade_msec) ,loop);
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE sound_back_scene( VALUE self, VALUE fade_msec , VALUE with_loop) {
	struct SoundData *data;
	Data_Get_Struct(self, struct SoundData, data);
    BOOL loop = with_loop == Qtrue? TRUE:FALSE;
	BOOL ret = data->klass->BackScene( NUM2INT(fade_msec), loop );
	return ret==TRUE?Qtrue:Qfalse;
}

void Init_sound() {
    VALUE Sound;

    Sound = rb_define_class("Sound", rb_cObject);
    rb_define_alloc_func(Sound, sound_alloc);
    rb_define_private_method(Sound, "initialize", (VALUE (*)(...))sound_initialize, 1);
	rb_define_private_method(Sound, "close", (VALUE (*)(...))sound_close, 1);
	rb_define_method(Sound, "load", (VALUE (*)(...))sound_load, 2);
	rb_define_method(Sound, "open", (VALUE (*)(...))sound_open, 2);
	rb_define_method(Sound, "rewind", (VALUE (*)(...))sound_rewind, 1);
	rb_define_method(Sound, "play", (VALUE (*)(...))sound_play, 2);
	rb_define_method(Sound, "play_with_fade", (VALUE (*)(...))sound_play_with_fade, 3);
	rb_define_method(Sound, "back_scene", (VALUE (*)(...))sound_back_scene, 2);
}
