/* config.h.  Generated from config.h.in by configure.  */
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
 * \file config.h
 * \brief Header configuration file of the lights off game.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2016, Javier Burguete Tolosa.
 */
#ifndef CONFIG__H
#define CONFIG__H 1

#define _(str) (gettext (str))  ///< Macro to simplify gettext calls.
#define DEFAULT_ROWS 5          ///< Default number of rows.
#define DEFAULT_COLUMNS 5       ///< Default number of columns.
#define DEFAULT_LEVEL 3         ///< Default game level.

#endif
