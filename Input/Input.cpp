#include <ruby.h>

#include "Input.h"
#include "InputClass.h"

using namespace base;

struct InputData {
	InputClass* klass;
	VALUE input_data;
};

static void input_dfree( struct InputData* data ) {
	if( data->klass ) {
		//TODO: delete����Ɨ����邱�Ƃ�����
		delete data->klass;
		data->klass = NULL;
	}
}

static VALUE input_alloc(VALUE klass) {
    struct InputData *data = ALLOC(struct InputData);
    return Data_Wrap_Struct(klass, 0, input_dfree , data);
}

static VALUE input_initialize( VALUE self, VALUE window ) {
    struct InputData *data;
	HWND hWnd = (HWND)NUM2LONG(rb_funcall( window, rb_intern("handle") , 0 ));
	Data_Get_Struct(self, struct InputData, data);
	data->klass = new InputClass( hWnd );
    return Qnil;
}

static VALUE input_update( VALUE self ) {
	struct InputData *data;
	Data_Get_Struct(self, struct InputData, data);
	return data->klass->Update() == TRUE?Qtrue:Qfalse;
}

static VALUE input_is_down( VALUE self, VALUE key ) {
	struct InputData *data;
	Data_Get_Struct(self, struct InputData, data);
	// key = 'joy 1', 'joy 2'
	// key = 'key a', 'key b'
	//char* RSTRING(str)->ptr
	//char* joy_head = "joy";
	//char* key_head = "key";
	return data->klass->IsDown( RSTRING_PTR(key) ) == TRUE?Qtrue:Qfalse;
}

static VALUE input_is_up( VALUE self, VALUE key ) {
	struct InputData *data;
	Data_Get_Struct(self, struct InputData, data);
	return data->klass->IsUp( RSTRING_PTR(key) ) == TRUE?Qtrue:Qfalse;
}

void Init_input() {
    VALUE Input;

    Input = rb_define_class("Input", rb_cObject);
    rb_define_alloc_func(Input, input_alloc);
    rb_define_private_method(Input, "initialize", (VALUE (*)(...))input_initialize, 1);
	rb_define_method(Input, "update", (VALUE (*)(...))input_update, 0);
	rb_define_method(Input, "down?", (VALUE (*)(...))input_is_down, 1);
	rb_define_method(Input, "up?", (VALUE (*)(...))input_is_up, 1);

}
