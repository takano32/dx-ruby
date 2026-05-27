#! /usr/bin/env ruby

#
# ���\�[�X���W���[���̃e�X�g
#
require 'dx-ruby/resource'
require 'dx-ruby/window'
require 'dx-ruby/sound'

# new�Ŏw�肵���p�X���A�[�J�C�u�łȂ���΁A�����\�[�X���[�h�ɂȂ�
# get�Ŏ擾�����t�@�C����String�^�ɂȂ�B
#�o�C�i���̎�������̂�\0�Ŏ~�܂�Ȃ��悤�ɁA�I�u�W�F�N�g�̃T�C�Y���擾���鎖�B
resource = Resource.new('./hoge')
file = resource.get('test.txt')
print file

# new�Ŏw�肵���p�X���A�[�J�C�u�Ȃ�΁A�A�[�J�C�u���[�h�ɂȂ�
# get�œ��l�ɃA�[�J�C�u������o����i�悤�ɂȂ�\��j�B
resource2 = Resource.new('./fuga')
file = resource2.get('test2.txt');
print file

# �ꉞ�A����I�ɂ͐����������A
# �𓀒��v���O���X�o�[���\�������̂́A�����Ȃ��͗l�B
resource3 = Resource.new('./sample03.zip')
file = resource3.get('sample03.mp3')

window = Window.new
window.create
sound = Sound.new( window )
sound.load(1, 'sound\xeno.wav')

sound.play_with_fade(1, 6000, true)

#print window.handle
window.show
window.main
