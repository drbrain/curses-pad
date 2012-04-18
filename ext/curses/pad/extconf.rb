require 'mkmf'

begin
  require "curses"
rescue LoadError
  abort "could not require 'curses', make sure your ruby was built with curses"
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

newpad = have_func("newpad") or
         have_macro("newpad", headers)

abort "unable to find newpad() in #{header} and #{library}" unless newpad

puts
create_header
create_makefile "pad"

