#! /usr/bin/env ruby
require 'dx-ruby/window'

class ActionListener
  def action
    print 'hoge'
  end
end


window = Window.new
window.create

listener = ActionListener.new
window.set_interval_action_listener(listener)

#print window.handle

window.show
window.main


