#! /usr/bin/env ruby
require 'dx-ruby/window'
require 'dx-ruby/sound'
require 'dx-ruby/resource'
require 'dx-ruby/graphics'
require 'dx-ruby/input'

window = Window.new
window.create

resource = Resource.new('data')
graphics = Graphics.new( window )
input = Input.new( window )

class LoopAction
  def initialize(input, graphics, resource)
    @input = input
    @graphics = graphics
    @resource = resource
    @graphics.regist_texture(32, resource.get('majiro.png'))
    @graphics.regist_texture(64, resource.get('hotpants.png'))
    @graphics.regist_sprite(16, 32,0, 0, 256, 256)
    @graphics.regist_sprite(8, 64, 0, 0, 128, 128)

    @x = 0
    @y = 0
  end
  def action
    @input.update
    @graphics.begin(0, 0, 0)
    @graphics.draw_sprite(16, 100, 100, 0)    
    if @input.down?('joy up') or @input.down?('key up') then
      @y = @y - 1
    end
    if @input.down?('joy down') or @input.down?('key down') then
      @y = @y + 1
    end
    if @input.down?('joy left') or @input.down?('key left') then
      @x = @x - 1
    end
    if @input.down?('joy right') or @input.down?('key right')  then
      @x = @x + 1
    end
    @graphics.draw_sprite(8, @x, @y, 0)
    @graphics.end
    sleep(0.001)
  end
end

action = LoopAction.new(input, graphics, resource )
window.loop_action = action

sound = Sound.new( window )
sound.load(1, resource.get('airman.mp3') )
sound.load(2, resource.get('airman.mp3') )
sound.load(3, resource.get('airman.mp3') )

#sound.play(1, true)
sound.play_with_fade(2, 4000, true)
sound.play_with_fade(3, 8000, true)
# sound.back_scene(2500, true)
# sound.back_scene(2500, true)
# sound.back_scene(2500, true)


#print window.handle
window.show

window.main

