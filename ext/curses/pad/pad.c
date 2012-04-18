#include "ruby.h"
#include "extconf.h"

/* begin code from ruby's ext/curses/curses.c */
#if defined(HAVE_NCURSES_H)
# include <ncurses.h>
#elif defined(HAVE_NCURSES_CURSES_H)
# include <ncurses/curses.h>
#elif defined(HAVE_CURSES_COLR_CURSES_H)
# ifdef HAVE_STDARG_PROTOTYPES
#  include <stdarg.h>
# else
#  include <varargs.h>
# endif
# include <curses_colr/curses.h>
#else
# include <curses.h>
#endif

static VALUE mCurses;

struct windata {
  WINDOW *window;
};

static void
no_window(void)
{
    rb_raise(rb_eRuntimeError, "already closed window");
}

#define GetWINDOW(obj, winp) do {\
    if (!OBJ_TAINTED(obj) && rb_safe_level() >= 4)\
	rb_raise(rb_eSecurityError, "Insecure: operation on untainted window");\
    Data_Get_Struct((obj), struct windata, (winp));\
    if ((winp)->window == 0) no_window();\
} while (0)

static void
window_free(void *p)
{
    struct windata *winp = p;
    if (winp->window && winp->window != stdscr) delwin(winp->window);
    winp->window = 0;
    xfree(winp);
}

static size_t
window_memsize(const void *p)
{
    const struct windata *winp = p;
    size_t size = sizeof(*winp);
    if (!winp) return 0;
    if (winp->window && winp->window != stdscr) size += sizeof(winp->window);
    return size;
}

static VALUE
prep_window(VALUE class, WINDOW *window)
{
    VALUE obj;
    struct windata *winp;

    if (window == NULL) {
	rb_raise(rb_eRuntimeError, "failed to create window");
    }

    obj = rb_obj_alloc(class);
    Data_Get_Struct(obj, struct windata, winp);
    winp->window = window;

    return obj;
}
/* end code from ruby's ext/curses/curses.c */

/*
 * Document-method: Curses::Pad.new
 *
 * call-seq:
 *   new(height, width)
 *
 * Construct a new Curses::Pad with constraints of +height+ lines, +width+
 * columns
 *
 */
static VALUE
pad_initialize(VALUE obj, VALUE h, VALUE w)
{
    struct windata *padp;
    WINDOW *window;

    rb_secure(4);
    rb_funcall(mCurses, rb_intern("init_screen"), 0);
    Data_Get_Struct(obj, struct windata, padp);
    if (padp->window) delwin(padp->window);
    window = newpad(NUM2INT(h), NUM2INT(w));
    wclear(window);
    padp->window = window;

    return obj;
}

/*
 * Document-method: Curses::Pad.subwin
 *
 * call-seq:
 *   subpad(height, width, begin_x, begin_y)
 *
 * Construct a new subpad with constraints of +height+ lines, +width+ columns,
 * begin at +begin_x+ line, and +begin_y+ columns on the pad.
 *
 */
static VALUE
pad_subpad(VALUE obj, VALUE height, VALUE width, VALUE begin_x, VALUE begin_y)
{
    struct windata *padp;
    WINDOW *subpad;
    VALUE pad;
    int h, w, x, y;

    h = NUM2INT(height);
    w = NUM2INT(width);
    x = NUM2INT(begin_x);
    y = NUM2INT(begin_y);
    GetWINDOW(obj, padp);
    subpad = subwin(padp->window, h, w, x, y);
    pad = prep_window(rb_obj_class(obj), subpad);

    return pad;
}

/*
 * Document-method: Curses::Pad.refresh
 *
 * call-seq:
 *   pad.refresh(pad_minrow, pad_mincol, screen_minrow, screen_mincol, screen_maxrow, screen_maxcol)
 *
 * Refreshes the pad immediately.  +pad_minrow+ and pad_mincol+ define the
 * upper-left corner of the rectangle to be displayed.  +screen_minrow+,
 * +screen_mincol+, +screen_maxrow+, +screen_maxcol+ define the edges of the
 * rectangle to be displayed on the screen.
 *
 */
static VALUE
pad_refresh(VALUE obj, VALUE pminrow, VALUE pmincol, VALUE sminrow,
	    VALUE smincol, VALUE smaxrow, VALUE smaxcol)
{
    struct windata *padp;
    int pmr, pmc, smr, smc, sxr, sxc;

    pmr = NUM2INT(pminrow);
    pmc = NUM2INT(pmincol);
    smr = NUM2INT(sminrow);
    smc = NUM2INT(smincol);
    sxr = NUM2INT(smaxrow);
    sxc = NUM2INT(smaxcol);

    GetWINDOW(obj, padp);
    prefresh(padp->window, pmr, pmc, smr, smc, sxr, sxc);

    return Qnil;
}

/*
 * Document-method: Curses::Pad.noutrefresh
 *
 * call-seq:
 *   pad.noutrefresh(pad_minrow, pad_mincol, screen_minrow, screen_mincol, screen_maxrow, screen_maxcol)
 *
 * Marks the pad for refresh at the next call to Curses.doupdate.
 * +pad_minrow+ and pad_mincol+ define the upper-left corner of the rectangle
 * to be displayed.  +screen_minrow+, +screen_mincol+, +screen_maxrow+,
 * +screen_maxcol+ define the edges of the rectangle to be displayed on the
 * screen.
 *
 */
static VALUE
pad_noutrefresh(VALUE obj, VALUE pminrow, VALUE pmincol, VALUE sminrow,
		VALUE smincol, VALUE smaxrow, VALUE smaxcol)
{
    struct windata *padp;
    int pmr, pmc, smr, smc, sxr, sxc;

    pmr = NUM2INT(pminrow);
    pmc = NUM2INT(pmincol);
    smr = NUM2INT(sminrow);
    smc = NUM2INT(smincol);
    sxr = NUM2INT(smaxrow);
    sxc = NUM2INT(smaxcol);

    GetWINDOW(obj, padp);
#ifdef HAVE_DOUPDATE
    pnoutrefresh(padp->window, pmr, pmc, smr, smc, sxr, sxc);
#else
    prefresh(padp->window, pmr, pmc, smr, smc, sxr, sxc);
#endif

    return Qnil;
}

void
Init_pad(void) {
    VALUE cWindow, cPad;

    rb_require("curses");

    mCurses = rb_path2class("Curses");

    if (rb_const_defined_at(mCurses, rb_intern("Pad")))
	return;

    cWindow = rb_path2class("Curses::Window");

    /*
     * Document-class: Curses::Pad
     *
     * == Description
     *
     * A Pad is like a Window but allows for scrolling of contents that cannot
     * fit on the screen.  Pads do not refresh automatically, use Pad#refresh
     * or Pad#noutrefresh instead.
     *
     */
    cPad = rb_define_class_under(mCurses, "Pad", cWindow);

    /* remember to update Rakefile */
    rb_const_set(cPad, rb_intern("VERSION"), rb_str_new2("1.0"));

    /* inherits alloc_func from cWindow */
    rb_define_method(cPad, "initialize", pad_initialize, 2);
    rb_define_method(cPad, "subpad", pad_subpad, 4);
    rb_define_method(cPad, "refresh", pad_refresh, 6);
    rb_define_method(cPad, "noutrefresh", pad_noutrefresh, 6);
    rb_undef_method(cPad, "subwin");
}
