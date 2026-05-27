#include <ruby.h>

#include "Graphics.h"
#include "GraphicsClass.h"

using namespace base;

struct GraphicsData {
	GraphicsClass* klass;
	VALUE texture_data;
};

static void graphics_dfree( struct GraphicsData* data ) {
	if( data->klass ) {
		delete data->klass;
		data->klass = NULL;
	}
}

static VALUE graphics_alloc(VALUE klass) {
    struct GraphicsData *data = ALLOC(struct GraphicsData);
    return Data_Wrap_Struct(klass, 0, graphics_dfree , data);
}

static VALUE graphics_initialize( VALUE self, VALUE window ) {
    struct GraphicsData *data;
	HWND hWnd = (HWND)NUM2LONG(rb_funcall( window, rb_intern("handle") , 0 ));
	Data_Get_Struct(self, struct GraphicsData, data);
	data->klass = new GraphicsClass( hWnd );
    return Qnil;
}

static VALUE graphics_regist_texture( VALUE self, VALUE texture_id, VALUE texture_data ) {
	struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
    
	data->texture_data = texture_data;
	BOOL ret = data->klass->RegistTexture( NUM2INT(texture_id),
		(BYTE*)RSTRING_PTR(texture_data), (int)RSTRING_LEN(texture_data) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE graphics_unregist_texture( VALUE self, VALUE texture_id ) {
	struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
	BOOL ret = data->klass->UnRegistTexture( NUM2INT(texture_id) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE graphics_regist_sprite( VALUE self, VALUE sprite_id, VALUE texture_id, 
									VALUE x, VALUE y, VALUE width, VALUE height ) {
	struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
	BOOL ret = data->klass->RegistSprite( NUM2INT(sprite_id), NUM2INT(texture_id), 
		NUM2INT(x), NUM2INT(y), NUM2INT(width), NUM2INT(height) );
	return ret==TRUE?Qtrue:Qfalse;
}


static VALUE graphics_unregist_sprite( VALUE self, VALUE sprite_id ) {
	struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
	BOOL ret = data->klass->UnRegistSprite( NUM2INT(sprite_id) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE graphics_draw_sprite( VALUE self, VALUE sprite_id, 
								  VALUE x, VALUE y, VALUE z ) {
	struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
	BOOL ret = data->klass->DrawSprite( NUM2INT( sprite_id), 
		NUM2INT(x), NUM2INT(y), NUM2INT(z) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE graphics_begin( VALUE self, VALUE red, VALUE green, VALUE blue ) {
	struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
	BOOL ret = data->klass->Begin( NUM2INT(red), NUM2INT(green), NUM2INT(blue) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE graphics_end( VALUE self ) {
	struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
	BOOL ret = data->klass->End();
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE graphics_create_texture( VALUE self, VALUE texture_id, VALUE width, VALUE height ) {
	struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
	BOOL ret = data->klass->CreateTexture( NUM2INT(texture_id), NUM2INT(width), 
		NUM2INT(height) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE graphics_draw_text( VALUE self, VALUE string,
								VALUE x, VALUE y ) {
	struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
	if( string == Qnil ) return Qfalse;
	BOOL ret = data->klass->DrawText( RSTRING_PTR(string),
		NUM2INT(x), NUM2INT(y) );
	return ret==TRUE?Qtrue:Qfalse;
}

static VALUE graphics_font_color( VALUE self, VALUE red, VALUE green,
                                 VALUE blue, VALUE alpha ) {
    struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
    BOOL ret = data->klass->SetFontColor( NUM2INT(red), NUM2INT(green),
        NUM2INT(blue), NUM2INT(alpha));
    return ret==TRUE?Qtrue:Qfalse;
}

static VALUE graphics_get_sprite_width( VALUE self, VALUE sprite_id ) {
    struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
    int ret = data->klass->GetSpriteWidth( NUM2INT(sprite_id) );
    return INT2NUM(ret);
}

static VALUE graphics_get_sprite_height( VALUE self, VALUE sprite_id ) {
    struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
    int ret = data->klass->GetSpriteHeight( NUM2INT(sprite_id) );
    return INT2NUM(ret);
}

static VALUE graphics_fade( VALUE self, VALUE red, VALUE green, VALUE blue, VALUE alpha ){
    struct GraphicsData *data;
	Data_Get_Struct(self, struct GraphicsData, data);
    BOOL ret = data->klass->SetFade( NUM2INT(red), NUM2INT(green),
        NUM2INT(blue), NUM2INT(alpha));
    return ret==TRUE?Qtrue:Qfalse;
}

void Init_graphics() {
    VALUE Graphics;

    Graphics = rb_define_class("Graphics", rb_cObject);
    rb_define_alloc_func(Graphics, graphics_alloc);
    rb_define_private_method(Graphics, "initialize", (VALUE (*)(...))graphics_initialize, 1);

	rb_define_method(Graphics, "create_texture", (VALUE (*)(...))graphics_create_texture, 3);
	rb_define_method(Graphics, "regist_texture", (VALUE (*)(...))graphics_regist_texture, 2);
	rb_define_method(Graphics, "unregist_texture", (VALUE (*)(...))graphics_regist_texture, 1);
	rb_define_method(Graphics, "regist_sprite", (VALUE (*)(...))graphics_regist_sprite, 6);
	rb_define_method(Graphics, "unregist_sprite", (VALUE (*)(...))graphics_unregist_sprite, 1);
	rb_define_method(Graphics, "draw_sprite", (VALUE (*)(...))graphics_draw_sprite, 4);
	rb_define_method(Graphics, "begin", (VALUE (*)(...))graphics_begin, 3);
	rb_define_method(Graphics, "end", (VALUE (*)(...))graphics_end, 0);
	rb_define_method(Graphics, "get_sprite_width", (VALUE (*)(...))graphics_get_sprite_width, 1);
	rb_define_method(Graphics, "get_sprite_height", (VALUE (*)(...))graphics_get_sprite_height, 1);
	

	rb_define_method(Graphics, "draw_text", (VALUE (*)(...))graphics_draw_text, 3);
	rb_define_method(Graphics, "font_color", (VALUE (*)(...))graphics_font_color, 4);

	rb_define_method(Graphics, "fade", (VALUE (*)(...))graphics_fade, 4);
}



/*

GraphicsTexture* pTexture = new GraphicsTexture( pGraphicsSystem );
pTexture->Open( "test.jpg" );
GraphicsSprite* pSprite = new GraphicsSprite( pGraphicsSystem );
pSprite->Load( pTexture, 0, 0, 256, 256 );


  1��̕`��
  Begin
pGraphicsSystem->Clear( D3DCOLOR_XRGB(0,0,0) );
pGraphicsSystem->BeginScene();

  Draw
  pSprite->Begin();
  pSprite->Draw( 20, 20, 0 );
  pSprite->Draw( 30, 30, 0 );
  pSprite->Draw( 40, 40, 0 );
  pSprite->End();

  End
  pGraphicsSystem->EndScene();
pGraphicsSystem->Present();



delete pSprite;
delete pTexture;
*/