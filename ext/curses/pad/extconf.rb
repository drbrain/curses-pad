require 'mkmf'

begin
  require "curses"
rescue LoadError
  abort "could not require 'curses', make sure your ruby was built with curses"
end

def have_feature name, header, library, fatal = true
  feature = have_func(name) or have_macro(name, headers)

  return unless fatal

  abort "unable to find #{name}() in #{header} and #{library}" unless feature
end

dir_config 'curses'
dir_config 'ncurses'

libs = [
  ["ncurses.h",            ["ncursesw", "ncurses"]],
  ["ncurses/curses.h",     ["ncurses"]],
  ["curses_colr/curses.h", ["cur_colr"]],
  ["curses.h",             ["curses"]],
]

header  = nil
library = nil

libs.each do |hdr, libraries|
  next unless have_header hdr

  header = hdr

  library = libraries.find do |lib|
    have_library lib, "initscr", hdr
  end

  break if library
end

abort "unable to find curses header and library" unless library

puts
puts "using header:  #{header}"
puts "using library: #{library}"
puts

headers = [header]
headers.unshift "varargs.h" if header == "curses_colr/curses.h"

have_feature "newpad",   headers, library
have_feature "doupdate", headers, library, false

puts
create_header
create_makefile "pad"

