# -*- ruby -*-

require 'rubygems'
require 'hoe'

Hoe.plugin :minitest
Hoe.plugin :git
Hoe.plugin :travis

hoe = Hoe.spec 'curses-pad' do
  self.version = '1.0' # remember to update pad.c
  developer 'Eric Hodel', 'drbrain@segment7.net'

  rdoc_locations <<
    'docs.seattlerb.org:/data/www/docs.seattlerb.org/curses-pad/'

  extra_dev_deps << ['rake-compiler', '~> 0.8', '>= 0.8.1']

  spec_extras['extensions']            = %w[ext/curses/pad/extconf.rb]
  spec_extras['required_ruby_version'] = '~> 1.9.2'
end

begin
  require 'rake/extensiontask'

  Rake::ExtensionTask.new 'pad', hoe.spec do |ext|
    ext.ext_dir = 'ext/curses/pad'
    ext.lib_dir = 'lib/curses'
  end
rescue LoadError
  task :compile do
    abort 'run: rake check_extra_deps'
  end
end

task default: :compile

task test: :compile do end

# vim: syntax=ruby
