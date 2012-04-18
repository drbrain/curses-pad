# -*- ruby -*-

require 'rubygems'
require 'hoe'
require 'rake/extensiontask'

Hoe.plugin :minitest
Hoe.plugin :git

hoe = Hoe.spec 'curses-pad' do
  self.version = '1.0' # remember to update pad.c
  developer 'Eric Hodel', 'drbrain@segment7.net'

  rdoc_locations <<
    'docs.seattlerb.org:/data/www/docs.seattlerb.org/curses-pad/'

  spec_extras['extensions']            = %w[ext/curses/pad/extconf.rb]
  spec_extras['required_ruby_version'] = '~> 1.9.2'
end

Rake::ExtensionTask.new 'pad', hoe.spec do |ext|
  ext.ext_dir = 'ext/curses/pad'
  ext.lib_dir = 'lib/curses'
end

# vim: syntax=ruby
