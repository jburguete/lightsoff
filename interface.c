/*
LightsOff:
A clone of the Tim Horton's LightsOff program written in C, adding variable
board sizes, undo and clear movements and a solver.

Copyright 2016, Javier Burguete Tolosa.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
        this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
        this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY AUTHORS ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
SHALL AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
OF SUCH DAMAGE.
*/

/**
 * \file interface.c
 * \brief Source file of the lights off game interface.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2016, Javier Burguete Tolosa.
 */
#include <stdlib.h>
#include <stdint.h>
#include <libintl.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "config.h"
#include "game.h"
#include "interface.h"

///> Type of buttons theme.
enum WindowTheme
{
  WINDOW_THEME_GEOMETRIC = 0,   ///< Geometric buttons theme.
  WINDOW_THEME_FACES = 1,       ///< Faces buttons theme.
};
///> Number of button themes.
#define N_THEMES (WINDOW_THEME_FACES + 1)
///> Theme images for the light buttons.
const char *light_images[N_THEMES] = {
  "media-playback-stop",
  "face-smile",
};

///> Theme images for the optimal solution buttons.
const char *solution_images[N_THEMES] = {
  "media-record",
  "face-devilish",
};

unsigned int window_rows = DEFAULT_ROWS;
///< Number of window rows.
unsigned int window_columns = DEFAULT_COLUMNS;
///< Number of window columns.
unsigned int window_squares = 0;
///< Number of window squares.
unsigned int window_input = 0;
///< New games input method: 0 level-based random, 1 user set.
unsigned int window_theme = WINDOW_THEME_GEOMETRIC;
///< Light buttons theme.
unsigned int array_ids[N_MAX_SQUARES];
///< Array of button signal identifiers.
unsigned int window_movements;
///< Number of user movements.
GList *list_movements = NULL;
///< List of user movements.

GtkSpinButton *spin_rows;       ///< GtkSpinButton to set the rows number.
GtkSpinButton *spin_columns;    ///< GtkSpinButton to set the columns number.
GtkSpinButton *spin_level;      ///< GtkSpinButton to set the game level.
GtkLabel *label_movements;      ///< Label showing the number of user movements.
GtkCheckButton *button_input;   ///< Button to set the new games input method.
GtkToggleButton **array_buttons = NULL; ///< Array of light buttons.
GtkToolButton *button_new;      ///< New game tool button.
GtkToolButton *button_options;  ///< Game options tool button.
GtkToolButton *button_clear;    ///< Clear game tool button.
GtkToolButton *button_undo;     ///< Undo last movement tool button.
GtkToolButton *button_solution; ///< Solution tool button.
GtkGrid *grid;                  ///< Light buttons grid.
GtkWindow *window;              ///< Main window.
GtkApplication *application;    ///< Application.

///> XPM logo.
const char *logo_xpm[] = {
  "74 86 131 2",
  "  	c #B2B2B2B2AFAF",
  ". 	c #CBCBCBCBC8C8",
  "X 	c #CBCBCBCBC9C9",
  "o 	c #BDBDBDBDB9B9",
  "O 	c #D2D2D2D2CFCF",
  "+ 	c #D9D9D9D9D7D7",
  "@ 	c #D6D6D6D6D4D4",
  "# 	c #BEBEBEBEBBBB",
  "$ 	c #CACACACAC9C9",
  "% 	c #CDCDCDCDCBCB",
  "& 	c #B1B1B1B1ADAD",
  "* 	c #C5C5C5C5C2C2",
  "= 	c #D0D0D0D0CECE",
  "- 	c #CACACACAC8C8",
  "; 	c #BBBBBBBBB9B9",
  ": 	c #E3E3E3E3E2E2",
  "> 	c #E4E4E4E4E3E3",
  ", 	c #CDCDCDCDCACA",
  "< 	c #B6B6B6B6B4B4",
  "1 	c #B6B6B6B6B3B3",
  "2 	c #B7B7B7B7B4B4",
  "3 	c #CFCFCFCFCDCD",
  "4 	c #E5E5E5E5E4E4",
  "5 	c #E1E1E1E1E0E0",
  "6 	c #B8B8B8B8B4B4",
  "7 	c #929292928E8E",
  "8 	c #919191918C8C",
  "9 	c #939393938E8E",
  "0 	c #C1C1C1C1BEBE",
  "q 	c #C9C9C9C9C7C7",
  "w 	c #CCCCCCCCCACA",
  "e 	c #C4C4C4C4C1C1",
  "r 	c #F3F3F3F3F3F3",
  "t 	c #FAFAFAFAFAFA",
  "y 	c #EBEBEBEBEAEA",
  "u 	c #BEBEBEBEBCBC",
  "i 	c #D2D2D2D2D0D0",
  "p 	c #EEEEEEEEEEEE",
  "a 	c #BBBBBBBBB8B8",
  "s 	c #B8B8B8B8B5B5",
  "d 	c #EFEFEFEFEEEE",
  "f 	c #EAEAEAEAE9E9",
  "g 	c #E8E8E8E8E7E7",
  "h 	c #ECECECECEBEB",
  "j 	c #D5D5D5D5D3D3",
  "k 	c #D4D4D4D4D3D3",
  "l 	c #B9B9B9B9B6B6",
  "z 	c #EAEAEAEAEAEA",
  "x 	c #E9E9E9E9E8E8",
  "c 	c #E7E7E7E7E6E6",
  "v 	c #E6E6E6E6E5E5",
  "b 	c #E6E6E6E6E4E4",
  "n 	c #B9B9B9B9B7B7",
  "m 	c #D8D8D8D8D6D6",
  "M 	c #E2E2E2E2E1E1",
  "N 	c #BCBCBCBCBABA",
  "B 	c #565658585454",
  "V 	c #555557575353",
  "C 	c #D7D7D7D7D5D5",
  "Z 	c #BFBFBFBFBDBD",
  "A 	c #5A5A5C5C5858",
  "S 	c #FFFFFFFFFFFF",
  "D 	c #C2C2C2C2C0C0",
  "F 	c #5D5D5F5F5B5B",
  "G 	c #DCDCDFDFD8D8",
  "H 	c #E3E3E5E5E0E0",
  "J 	c #EAEAEBEBE8E8",
  "K 	c #F0F0F2F2EFEF",
  "L 	c #F7F7F8F8F7F7",
  "P 	c #FDFDFDFDFDFD",
  "I 	c #5E5E60605C5C",
  "U 	c #C6C6C6C6C4C4",
  "Y 	c #606062625E5E",
  "T 	c #DFDFE2E2DCDC",
  "R 	c #E6E6E8E8E4E4",
  "E 	c #EDEDEEEEEBEB",
  "W 	c #F3F3F4F4F2F2",
  "Q 	c #F8F8F8F8F7F7",
  "! 	c #F7F7F8F8F6F6",
  "~ 	c #616163635F5F",
  "^ 	c #E0E0E0E0DFDF",
  "/ 	c #636365656161",
  "( 	c #E1E1E4E4DFDF",
  ") 	c #E6E6E9E9E4E4",
  "_ 	c #EBEBEDEDEAEA",
  "` 	c #EFEFF1F1EEEE",
  "' 	c #F0F0F1F1EFEF",
  "] 	c #EEEEEFEFECEC",
  "[ 	c #646467676262",
  "{ 	c #666668686464",
  "} 	c #E0E0E3E3DDDD",
  "| 	c #E3E3E6E6E1E1",
  " .	c #E7E7E9E9E4E4",
  "..	c #E8E8EAEAE6E6",
  "X.	c #E7E7E9E9E5E5",
  "o.	c #E4E4E7E7E2E2",
  "O.	c #68686A6A6565",
  "+.	c #DFDFDFDFDEDE",
  "@.	c #6A6A6C6C6767",
  "#.	c #DDDDE0E0DADA",
  "$.	c #DEDEE1E1DBDB",
  "%.	c #E0E0E2E2DDDD",
  "&.	c #DBDBDEDED8D8",
  "*.	c #6B6B6D6D6969",
  "=.	c #D3D3D3D3D1D1",
  "-.	c #6D6D6F6F6A6A",
  ";.	c #D8D8DBDBD5D5",
  ":.	c #D7D7DBDBD4D4",
  ">.	c #D8D8DBDBD4D4",
  ",.	c #D7D7DBDBD3D3",
  "<.	c #D5D5D9D9D1D1",
  "1.	c #D3D3D7D7CFCF",
  "2.	c #6E6E71716C6C",
  "3.	c #D7D7D7D7D4D4",
  "4.	c #707072726D6D",
  "5.	c #717174746F6F",
  "6.	c #DEDEDEDEDDDD",
  "7.	c #DADADADAD8D8",
  "8.	c #747476767171",
  "9.	c #737375757070",
  "0.	c #DDDDDDDDDCDC",
  "q.	c #DDDDDDDDDBDB",
  "w.	c #DCDCDCDCDBDB",
  "e.	c #DBDBDBDBDADA",
  "r.	c #DADADADAD9D9",
  "t.	c #D9D9D9D9D8D8",
  "y.	c #D7D7D7D7D6D6",
  "u.	c #D6D6D6D6D5D5",
  "i.	c #D3D3D3D3D2D2",
  "p.	c #CDCDCDCDCCCC",
  "a.	c #D0D0D0D0CFCF",
  "  . X o O + + + + + + + + + + + + + + + + + + + + + + + + + + + + + @ # $ % & * = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -   . X o ",
  "; : > , < 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 3 4 5 6 7 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 9 ; : > , ",
  ", : 4 3 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 < , : 4 3 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 , : 4 3 ",
  "0 q w e r t t t t t t t t t t t t t t t t t t t t t t t t t t t t t y 0 q w u @ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + i o . w e ",
  "p a s d f g g g g g g g g g g g g g g g g g g g g g g g g g g g g g h p a s j + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + k l s d ",
  "z 1 1 x c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c z 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 x ",
  "c 1 1 c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 c ",
  "c 1 1 c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 c ",
  "v 1 1 v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 v ",
  "b 1 1 b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 b ",
  "4 1 1 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 4 ",
  "4 1 1 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 4 ",
  "> 1 1 > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 > ",
  "> 1 1 > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 > ",
  ": 1 1 : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 : ",
  ": 1 1 : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 : ",
  ": 1 1 : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : 1 1 + + + + + + + + + + n n n n n n n n n n n n m + + + + + + + + + + 1 1 : ",
  "M 1 1 M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M 1 1 + + + + + + + + + + N B V V V V V V V V V ; C + + + + + + + + + + 1 1 M ",
  "M 1 1 M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M 1 1 + + + + + + + + + + Z A S S S S S S S S A Z C + + + + + + + + + + 1 1 M ",
  "5 1 1 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 1 1 + + + + + + + + + + D F S G H J K L P S I D C + + + + + + + + + + 1 1 5 ",
  "5 1 1 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 1 1 + + + + + + + + + + U Y S T R E W Q ! S ~ U C + + + + + + + + + + 1 1 5 ",
  "^ 1 1 ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ 1 1 + + + + + + + + + + q / S ( ) _ ` ' ] S [ q C + + + + + + + + + + 1 1 ^ ",
  "^ 1 1 ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ 1 1 + + + + + + + + + + % { S } |  ...X.o.S O.w C + + + + + + + + + + 1 1 ^ ",
  "+.1 1 +.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.1 1 + + + + + + + + + + = @.S #.$.%.%.$.&.S *.= C + + + + + + + + + + 1 1 +.",
  "+.1 1 +.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.1 1 + + + + + + + + + + =.-.S ;.:.>.,.<.1.S 2.=.C + + + + + + + + + + 1 1 +.",
  "+.1 1 +.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.1 1 + + + + + + + + + + 3.4.S S S S S S S S 5.@ m + + + + + + + + + + 1 1 +.",
  "6.1 1 6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.1 1 + + + + + + + + + + 7.8.9.9.9.9.9.9.9.9.9.7.+ + + + + + + + + + + 1 1 6.",
  "0.1 1 0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1 1 + + + + + + + + + + q.q.q.q.q.q.q.q.q.q.q.q.+ + + + + + + + + + + 1 1 0.",
  "w.1 1 w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 w.",
  "e.1 1 e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 e.",
  "r.1 1 r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 r.",
  "t.1 1 t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 t.",
  "+ 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 + ",
  "y.1 1 y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 y.",
  "u.1 1 u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 u.",
  "@ 1 1 @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 @ ",
  "j 1 1 j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 j ",
  "i.1 1 i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 i.",
  "=.1 1 =.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 =.",
  "i 1 1 i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 i ",
  "p.l l p.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.p.l l j + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + j l l p.",
  "  . % & * = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -   . X o O + + + + + + + + + + + + + + + + + + + + + + + + + + + + + @ # $ % & ",
  "; : 5 6 7 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 9 ; : > , < 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 3 4 5 6 ",
  ", : 4 3 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 , : 4 3 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 < , : 4 3 ",
  "0 q w u @ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + i o . w e r t t t t t t t t t t t t t t t t t t t t t t t t t t t t t y 0 q w e ",
  "p a s j + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + k l s d f g g g g g g g g g g g g g g g g g g g g g g g g g g g g g h p a s d ",
  "z 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 x c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c z 1 1 x ",
  "c 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c 1 1 c ",
  "c 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c c 1 1 c ",
  "v 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v v 1 1 v ",
  "b 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b b 1 1 b ",
  "4 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 1 1 4 ",
  "4 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 4 1 1 4 ",
  "> 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > 1 1 > ",
  "> 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > > 1 1 > ",
  ": 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : 1 1 : ",
  ": 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : 1 1 : ",
  ": 1 1 + + + + + + + + + + n n n n n n n n n n n n m + + + + + + + + + + 1 1 : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : : 1 1 : ",
  "M 1 1 + + + + + + + + + + N B V V V V V V V V V ; C + + + + + + + + + + 1 1 M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M 1 1 M ",
  "M 1 1 + + + + + + + + + + Z A S S S S S S S S A Z C + + + + + + + + + + 1 1 M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M M 1 1 M ",
  "5 1 1 + + + + + + + + + + D F S G H J K L P S I D C + + + + + + + + + + 1 1 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 1 1 5 ",
  "5 1 1 + + + + + + + + + + U Y S T R E W Q ! S ~ U C + + + + + + + + + + 1 1 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 5 1 1 5 ",
  "^ 1 1 + + + + + + + + + + q / S ( ) _ ` ' ] S [ q C + + + + + + + + + + 1 1 ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ 1 1 ^ ",
  "^ 1 1 + + + + + + + + + + % { S } |  ...X.o.S O.w C + + + + + + + + + + 1 1 ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ 1 1 ^ ",
  "+.1 1 + + + + + + + + + + = @.S #.$.%.%.$.&.S *.= C + + + + + + + + + + 1 1 +.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.1 1 +.",
  "+.1 1 + + + + + + + + + + =.-.S ;.:.>.,.<.1.S 2.=.C + + + + + + + + + + 1 1 +.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.1 1 +.",
  "+.1 1 + + + + + + + + + + 3.4.S S S S S S S S 5.@ m + + + + + + + + + + 1 1 +.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.+.1 1 +.",
  "6.1 1 + + + + + + + + + + 7.8.9.9.9.9.9.9.9.9.9.7.+ + + + + + + + + + + 1 1 6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.6.1 1 6.",
  "0.1 1 + + + + + + + + + + q.q.q.q.q.q.q.q.q.q.q.q.+ + + + + + + + + + + 1 1 0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.0.1 1 0.",
  "w.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.w.1 1 w.",
  "e.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.e.1 1 e.",
  "r.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.r.1 1 r.",
  "t.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.1 1 t.",
  "+ 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 + ",
  "y.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.y.1 1 y.",
  "u.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.u.1 1 u.",
  "@ 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ @ 1 1 @ ",
  "j 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j j 1 1 j ",
  "i.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.i.1 1 i.",
  "=.1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 =.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.=.1 1 =.",
  "i 1 1 + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + 1 1 i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i i 1 1 i ",
  "p.l l j + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + + j l l p.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.a.p.l l p.",
  "  . X o O + + + + + + + + + + + + + + + + + + + + + + + + + + + + + @ # $ % & * = = = = = = = = = = = = = = = = = = = = = = = = = = = = = -   . % & ",
  "; : > , < 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 2 3 4 5 6 7 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 9 ; : 5 6 ",
  ", : 4 3 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 < , : 4 3 2 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 , : 4 3 ",
  "o . w e r t t t t t t t t t t t t t t t t t t t t t t t t t t t t t y 0 q w u @ + + + + + + + + + + + + + + + + + + + + + + + + + + + + + i o . w u "
};

/**
 * \fn void window_destroy ()
 * \brief Function to free memory assigned by the main window.
 */
void
window_destroy ()
{
  GList *list;
  free (array_buttons);
  for (list = list_movements; list; list = list->next)
    {
      free (list->data);
    }
  g_list_free (list_movements);
  list_movements = NULL;
}

/**
 * \fn static void window_set ()
 * \brief Function to set the light buttons.
 */
static void
window_set ()
{
  char label[64];
  GtkImage *image;
  unsigned int i;
  for (i = 0; i < window_squares; ++i)
    {
      g_signal_handler_block (array_buttons[i], array_ids[i]);
      if (status & (1L << i))
        {
          gtk_toggle_button_set_active (array_buttons[i], 1);
          image = (GtkImage *)
            gtk_image_new_from_icon_name (light_images[window_theme],
                                          GTK_ICON_SIZE_BUTTON);
        }
      else
        {
          gtk_toggle_button_set_active (array_buttons[i], 0);
          image = NULL;
        }
      gtk_button_set_image (GTK_BUTTON (array_buttons[i]),
                            GTK_WIDGET (image));
      g_signal_handler_unblock (array_buttons[i], array_ids[i]);
    }
  snprintf (label, 64, _("Number of movements: %u"), window_movements);
  gtk_label_set_text (label_movements, label);
}

/**
 * \fn static void window_toggle (GtkToggleButton *button)
 * \brief Function to set a square.
 * \param button
 * \brief button to set.
 */
static void
window_toggle (GtkToggleButton * button)
{
  unsigned int i;
  for (i = 0; button != array_buttons[i]; ++i);
  set (&status, i);
  window_set ();
}

/**
 * \fn static void window_move (GtkToggleButton *button)
 * \brief Function to do a movement.
 * \param button
 * \brief button to move.
 */
static void
window_move (GtkToggleButton * button)
{
  GtkMessageDialog *dialog;
  unsigned int *data;
  unsigned int i;
  for (i = 0; button != array_buttons[i]; ++i);
  move (&status, i);
  data = (unsigned int *) malloc (sizeof (unsigned int));
  *data = i;
  list_movements = g_list_prepend (list_movements, data);
  ++window_movements;
  window_set ();
  if (!status)
    {
      dialog = (GtkMessageDialog *)
        gtk_message_dialog_new (window,
                                GTK_DIALOG_MODAL,
                                GTK_MESSAGE_INFO,
                                GTK_BUTTONS_CLOSE,
                                _("Game solved in %u movements"),
                                window_movements);
      gtk_window_set_title (GTK_WINDOW (dialog), _("Congratulations!"));
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (GTK_WIDGET (dialog));
    }
}

/**
 * \fn static void window_undo ()
 * \brief Function to undo an user movement.
 */
static void
window_undo ()
{
  if (window_movements)
    {
      move (&status, *(unsigned int *) (list_movements->data));
      free (list_movements->data);
      list_movements = list_movements->next;
      --window_movements;
    }
  window_set ();
}

/**
 * \fn static void window_clear ()
 * \brief Function to clear all user movements.
 */
static void
window_clear ()
{
  while (window_movements)
    window_undo ();
}

/**
 * \fn static void window_custom ()
 * \brief Function to input a custom game.
 */
static void
window_custom ()
{
  GtkWidget *widget[] = {
    GTK_WIDGET (button_new),
    GTK_WIDGET (button_options),
    GTK_WIDGET (button_clear),
    GTK_WIDGET (button_undo),
    GTK_WIDGET (button_solution),
  };
  GtkMessageDialog *dialog;
  unsigned int i;
  for (i = 0; i < 5; ++i)
    gtk_widget_set_sensitive (widget[i], 0);
  for (i = 0; i < window_squares; ++i)
    {
      g_signal_handler_disconnect (array_buttons[i], array_ids[i]);
      array_ids[i] = g_signal_connect (array_buttons[i], "clicked",
                                       G_CALLBACK (window_toggle), NULL);
    }
  dialog = (GtkMessageDialog *)
    gtk_message_dialog_new (window,
                            GTK_DIALOG_DESTROY_WITH_PARENT,
                            GTK_MESSAGE_INFO,
                            GTK_BUTTONS_OK,
                            _("Click on the wanted buttons and press OK "
                              "button"));
  gtk_window_set_title (GTK_WINDOW (dialog), _("Set the game"));
  g_signal_connect (dialog, "response", G_CALLBACK (gtk_main_quit), NULL);
  gtk_widget_show_all (GTK_WIDGET (dialog));
  gtk_main ();
  gtk_widget_destroy (GTK_WIDGET (dialog));
  for (i = 0; i < window_squares; ++i)
    {
      g_signal_handler_disconnect (array_buttons[i], array_ids[i]);
      array_ids[i] = g_signal_connect (array_buttons[i], "clicked",
                                       G_CALLBACK (window_move), NULL);
    }
  for (i = 0; i < 5; ++i)
    gtk_widget_set_sensitive (widget[i], 1);
}

/**
 * \fn static void window_new_game ()
 * \brief Function to do a new game.
 */
static void
window_new_game ()
{
  unsigned int i, j, k;
  for (i = 0; i < window_squares; ++i)
    gtk_widget_destroy (GTK_WIDGET (array_buttons[i]));
  window_destroy ();
  nrows = window_rows;
  ncolumns = window_columns;
  if (window_input)
    game_init ();
  else
    game_new ();
  array_buttons = (GtkToggleButton **)
    malloc (nsquares * sizeof (GtkToggleButton *));
  for (i = k = 0; i < window_rows; ++i)
    for (j = 0; j < window_columns; ++j, ++k)
      {
        array_buttons[k] = (GtkToggleButton *) gtk_toggle_button_new ();
        gtk_widget_set_hexpand (GTK_WIDGET (array_buttons[k]), 1);
        gtk_widget_set_vexpand (GTK_WIDGET (array_buttons[k]), 1);
        gtk_grid_attach (grid, GTK_WIDGET (array_buttons[k]), j, i, 1, 1);
        array_ids[k] = g_signal_connect (array_buttons[k], "clicked",
                                         G_CALLBACK (window_move), NULL);
      }
  window_squares = k;
  window_movements = 0;
  window_set ();
  gtk_widget_show_all (GTK_WIDGET (grid));
  if (window_input)
    window_custom ();
}

/**
 * \fn void window_options_update ()
 * \brief Function to update the options dialog properties.
 */
void
window_options_update ()
{
  unsigned int rows, columns;
  rows = gtk_spin_button_get_value_as_int (spin_rows);
  columns = gtk_spin_button_get_value_as_int (spin_columns);
  gtk_spin_button_set_range (spin_level, 1., rows * columns);
  gtk_widget_set_sensitive (GTK_WIDGET (spin_level),
                            !gtk_toggle_button_get_active
                            (GTK_TOGGLE_BUTTON (button_input)));
}

/**
 * \fn static void window_options ()
 * \brief Function to set the game options.
 */
static void
window_options ()
{
  char *window_themes[N_THEMES] = {
    _("Geometric"),
    _("Faces"),
  };
  GtkDialog *dialog;
  GtkGrid *grid;
  GtkLabel *label;
  GtkComboBoxText *combo;
  GtkContainer *content;
  unsigned int i;
  dialog = (GtkDialog *)
    gtk_dialog_new_with_buttons (_("Options"),
                                 window,
                                 GTK_DIALOG_MODAL,
                                 _("_OK"),
                                 GTK_RESPONSE_OK,
                                 _("_Cancel"), GTK_RESPONSE_CANCEL, NULL);
  content = (GtkContainer *) gtk_dialog_get_content_area (dialog);
  grid = (GtkGrid *) gtk_grid_new ();
  gtk_container_add (content, GTK_WIDGET (grid));
  label = (GtkLabel *) gtk_label_new (_("Number of rows"));
  gtk_grid_attach (grid, GTK_WIDGET (label), 0, 0, 1, 1);
  label = (GtkLabel *) gtk_label_new (_("Number of columns"));
  gtk_grid_attach (grid, GTK_WIDGET (label), 0, 1, 1, 1);
  label = (GtkLabel *) gtk_label_new (_("Game level"));
  gtk_grid_attach (grid, GTK_WIDGET (label), 0, 2, 1, 1);
  label = (GtkLabel *) gtk_label_new (_("Theme"));
  gtk_grid_attach (grid, GTK_WIDGET (label), 0, 4, 1, 1);
  spin_rows = (GtkSpinButton *)
    gtk_spin_button_new_with_range (2., N_MAX_ROWS, 1.);
  gtk_grid_attach (grid, GTK_WIDGET (spin_rows), 1, 0, 1, 1);
  g_signal_connect (spin_rows, "value-changed",
                    G_CALLBACK (window_options_update), NULL);
  spin_columns = (GtkSpinButton *)
    gtk_spin_button_new_with_range (2., N_MAX_COLUMNS, 1.);
  gtk_grid_attach (grid, GTK_WIDGET (spin_columns), 1, 1, 1, 1);
  g_signal_connect (spin_columns, "value-changed",
                    G_CALLBACK (window_options_update), NULL);
  spin_level = (GtkSpinButton *)
    gtk_spin_button_new_with_range (1., N_MAX_SQUARES, 1.);
  gtk_grid_attach (grid, GTK_WIDGET (spin_level), 1, 2, 1, 1);
  button_input = (GtkCheckButton *)
    gtk_check_button_new_with_mnemonic (_("_Custom input"));
  gtk_grid_attach (grid, GTK_WIDGET (button_input), 0, 3, 2, 1);
  g_signal_connect (button_input, "clicked",
                    G_CALLBACK (window_options_update), NULL);
  combo = (GtkComboBoxText *) gtk_combo_box_text_new ();
  for (i = 0; i < N_THEMES; ++i)
    gtk_combo_box_text_append_text (combo, window_themes[i]);
  gtk_grid_attach (grid, GTK_WIDGET (combo), 1, 4, 1, 1);
  gtk_widget_show_all (GTK_WIDGET (content));
  gtk_spin_button_set_value (spin_rows, window_rows);
  gtk_spin_button_set_value (spin_columns, window_columns);
  gtk_spin_button_set_value (spin_level, level);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (button_input),
                                window_input);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), window_theme);
  if (gtk_dialog_run (dialog) == GTK_RESPONSE_OK)
    {
      window_rows = gtk_spin_button_get_value_as_int (spin_rows);
      window_columns = gtk_spin_button_get_value_as_int (spin_columns);
      level = gtk_spin_button_get_value_as_int (spin_level);
      window_input
        = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button_input));
      window_theme = gtk_combo_box_get_active (GTK_COMBO_BOX (combo));
    }
  gtk_widget_destroy (GTK_WIDGET (dialog));
}

/**
 * \fn static void window_solve ()
 * \brief Function to print the optimal solution.
 */
static void
window_solve ()
{
  GtkMessageDialog *dialog;
  GtkImage *image;
  int i;
  nmovements = play ();
  for (i = 0; i < nmovements; ++i)
    {
      image = (GtkImage *)
        gtk_image_new_from_icon_name (solution_images[window_theme],
                                      GTK_ICON_SIZE_BUTTON);
      gtk_button_set_image (GTK_BUTTON (array_buttons[movement[i]]),
                            GTK_WIDGET (image));
    }
  if (nmovements < 0)
    {
      dialog = (GtkMessageDialog *)
        gtk_message_dialog_new (window,
                                GTK_DIALOG_MODAL,
                                GTK_MESSAGE_INFO,
                                GTK_BUTTONS_OK,
                                "%s", _("Unable to find a solution"));
      gtk_window_set_title (GTK_WINDOW (dialog), _("Sorry!"));
      gtk_dialog_run (GTK_DIALOG (dialog));
      gtk_widget_destroy (GTK_WIDGET (dialog));
    }
}

/**
 * \fn static void window_about ()
 * \brief Function to show an about dialog.
 */
static void
window_about ()
{
  static const gchar *authors[] = {
    "Javier Burguete Tolosa <jburguete@eead.csic.es>",
    NULL
  };
  gtk_show_about_dialog
    (window,
     "program-name", "LightsOff",
     "comments",
     _("Lights off game"),
     "authors", authors,
     "copyright", "Copyright 2016 Javier Burguete Tolosa",
     "license-type", GTK_LICENSE_BSD,
     "version", "0.0",
     "website", "http://github.com/jburguete/lightsoff", NULL);
}

/**
 * \fn static void window_activate (GtkApplication *application)
 * \brief Function to activate the window.
 */
void
window_activate (GtkApplication * application)
{
  GtkToolbar *toolbar;
  GtkToolButton *button;
  GtkImage *image;
  GdkPixbuf *pixbuf;

  // Main window
  window = (GtkWindow *) gtk_application_window_new (application);
  gtk_window_set_title (window, _("Lights off"));
  gtk_widget_set_size_request (GTK_WIDGET (window), 320, 440);

  // Logo
  pixbuf = gdk_pixbuf_new_from_xpm_data (logo_xpm);
  gtk_window_set_default_icon (pixbuf);

  // Grid
  grid = (GtkGrid *) gtk_grid_new ();
  gtk_container_add (GTK_CONTAINER (window), GTK_WIDGET (grid));

  // Toolbar
  toolbar = (GtkToolbar *) gtk_toolbar_new ();
  gtk_grid_attach (grid, GTK_WIDGET (toolbar), 0, -1, N_MAX_COLUMNS, 1);

  // Exit tool
  image = (GtkImage *)
    gtk_image_new_from_icon_name ("application-exit",
                                  GTK_ICON_SIZE_SMALL_TOOLBAR);
  button = (GtkToolButton *)
    gtk_tool_button_new (GTK_WIDGET (image), _("Exit"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (button), _("Exit"));
  gtk_toolbar_insert (toolbar, GTK_TOOL_ITEM (button), 0);
  g_signal_connect_swapped (button, "clicked",
                            G_CALLBACK (g_application_quit),
                            G_APPLICATION (application));

  // About tool
  image = (GtkImage *)
    gtk_image_new_from_icon_name ("help-about", GTK_ICON_SIZE_SMALL_TOOLBAR);
  button = (GtkToolButton *)
    gtk_tool_button_new (GTK_WIDGET (image), _("About"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (button), _("About"));
  gtk_toolbar_insert (toolbar, GTK_TOOL_ITEM (button), 0);
  g_signal_connect (button, "clicked", G_CALLBACK (window_about), NULL);

  // Solution tool
  image = (GtkImage *)
    gtk_image_new_from_icon_name ("system-help", GTK_ICON_SIZE_SMALL_TOOLBAR);
  button_solution =
    (GtkToolButton *) gtk_tool_button_new (GTK_WIDGET (image),
                                           _("Optimal solution"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_solution),
                               _("Show the optimal solution"));
  gtk_toolbar_insert (toolbar, GTK_TOOL_ITEM (button_solution), 0);
  g_signal_connect (button_solution, "clicked", G_CALLBACK (window_solve),
                    NULL);

  // Undo tool
  image = (GtkImage *)
    gtk_image_new_from_icon_name ("edit-undo", GTK_ICON_SIZE_SMALL_TOOLBAR);
  button_undo =
    (GtkToolButton *) gtk_tool_button_new (GTK_WIDGET (image), _("Undo"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_undo),
                               _("Undo the last movement"));
  gtk_toolbar_insert (toolbar, GTK_TOOL_ITEM (button_undo), 0);
  g_signal_connect (button_undo, "clicked", G_CALLBACK (window_undo), NULL);

  // Clear tool
  image = (GtkImage *)
    gtk_image_new_from_icon_name ("edit-clear", GTK_ICON_SIZE_SMALL_TOOLBAR);
  button_clear =
    (GtkToolButton *) gtk_tool_button_new (GTK_WIDGET (image), _("Clear"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_clear),
                               _("Clear all movements"));
  gtk_toolbar_insert (toolbar, GTK_TOOL_ITEM (button_clear), 0);
  g_signal_connect (button_clear, "clicked", G_CALLBACK (window_clear), NULL);

  // Options tool
  image = (GtkImage *)
    gtk_image_new_from_icon_name ("preferences-desktop",
                                  GTK_ICON_SIZE_SMALL_TOOLBAR);
  button_options =
    (GtkToolButton *) gtk_tool_button_new (GTK_WIDGET (image),
                                           _("Preferences"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_options), _("Preferences"));
  gtk_toolbar_insert (toolbar, GTK_TOOL_ITEM (button_options), 0);
  g_signal_connect (button_options, "clicked", G_CALLBACK (window_options),
                    NULL);

  // New game tool
  image = (GtkImage *)
    gtk_image_new_from_icon_name ("document-new",
                                  GTK_ICON_SIZE_SMALL_TOOLBAR);
  button_new =
    (GtkToolButton *) gtk_tool_button_new (GTK_WIDGET (image), _("New game"));
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_new), _("New game"));
  gtk_toolbar_insert (toolbar, GTK_TOOL_ITEM (button_new), 0);
  g_signal_connect (button_new, "clicked", G_CALLBACK (window_new_game),
                    NULL);

  // Movements label
  label_movements = (GtkLabel *) gtk_label_new (NULL);
  gtk_grid_attach (grid, GTK_WIDGET (label_movements),
                   0, N_MAX_ROWS, N_MAX_COLUMNS, 1);

  // First game
  window_new_game ();

  // Showing all
  gtk_widget_show_all (GTK_WIDGET (window));
}
