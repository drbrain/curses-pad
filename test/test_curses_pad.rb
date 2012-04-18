require 'minitest/autorun'
require 'tempfile'
require 'curses'
require 'curses/pad'

class TestCursesPad < MiniTest::Unit::TestCase

  def setup
    skip 'using built-in Curses::Pad' unless
      Curses::Pad.const_defined? :VERSION

    Curses.init_screen
  end

  def teardown
    Curses.close_screen
  end

  def test_initialize
    pad = Curses::Pad.new Curses.lines, Curses.cols

    assert_equal 0, pad.curx
    assert_equal 0, pad.cury

    assert_equal Curses.cols,  pad.maxx
    assert_equal Curses.lines, pad.maxy

    assert_equal 0, pad.begx
    assert_equal 0, pad.begy
  end

  def test_noutrefresh
    pad = Curses::Pad.new Curses.lines * 2, Curses.cols

    pad.noutrefresh 3, 4, 5, 6, 7, 8

    Curses.doupdate

    assert_equal 0, pad.curx
    assert_equal 0, pad.cury

    assert_equal Curses.cols,      pad.maxx
    assert_equal Curses.lines * 2, pad.maxy

    assert_equal 6, pad.begx
    assert_equal 5, pad.begy
  end

  def test_refresh
    pad = Curses::Pad.new Curses.lines * 2, Curses.cols

    pad.refresh 3, 4, 5, 6, 7, 8

    assert_equal 0, pad.curx
    assert_equal 0, pad.cury

    assert_equal Curses.cols,      pad.maxx
    assert_equal Curses.lines * 2, pad.maxy

    assert_equal 6, pad.begx
    assert_equal 5, pad.begy
  end

  def test_subpad
    pad = Curses::Pad.new Curses.lines, Curses.cols

    subpad = pad.subpad 5, 4, 3, 2

    assert_equal 0, subpad.curx
    assert_equal 0, subpad.cury

    assert_equal 4, subpad.maxx
    assert_equal 5, subpad.maxy

    assert_equal 2, subpad.begx
    assert_equal 3, subpad.begy
  end

end

