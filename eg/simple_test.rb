#! /usr/bin/env ruby
# Simple test to verify dx-ruby modules load correctly
require 'window'
require 'graphics'
require 'sound'
require 'resource'
require 'input'

puts "All modules loaded successfully!"
puts "Ruby version: #{RUBY_VERSION}"
puts "Platform: #{RUBY_PLATFORM}"

# Quick smoke test - create window
window = Window.new
window.create

resource = Resource.new('Z:/home/takano32/GitHub/dx-ruby/eg/data')
graphics = Graphics.new(window)
input = Input.new(window)
sound = Sound.new(window)

puts "Window created: #{window.handle}"
puts "All objects created OK!"

# Load a texture
img_data = resource.get('majiro.png')
graphics.regist_texture(1, img_data)
graphics.regist_sprite(1, 1, 0, 0, 256, 256)

# Load sound
snd_data = resource.get('tiuntiun.wav')
sound.load(1, snd_data)

puts "Texture and sound loaded!"

class DemoAction
  def initialize(graphics, input)
    @graphics = graphics
    @input = input
    @x = 100
    @y = 100
    @frame = 0
  end

  def action
    @input.update
    @graphics.begin(0, 0, 64)  # dark blue background
    @graphics.draw_sprite(1, @x, @y, 0)
    @graphics.draw_text("dx-ruby demo (Ruby #{RUBY_VERSION})", 10, 10)
    @graphics.draw_text("Press arrow keys to move, Esc/F12 to quit", 10, 30)
    @graphics.end

    @x -= 2 if @input.down?('key left')
    @x += 2 if @input.down?('key right')
    @y -= 2 if @input.down?('key up')
    @y += 2 if @input.down?('key down')
    @frame += 1
  end
end

action = DemoAction.new(graphics, input)
window.loop_action = action
sound.play(1, true)
window.show
window.main
