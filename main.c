/*
LightsOff:
A clone of the Tim Horton's LightsOff program written in C, adding variable
board sizes, undo and clear movements and a solver.

Copyright 2016-2020, Javier Burguete Tolosa.

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
 * \file main.c
 * \brief Source file of the lights off game main function.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2016-2020, Javier Burguete Tolosa.
 */
#include <stdlib.h>
#include <stdint.h>
#include <libintl.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "config.h"
#include "game.h"
#include "interface.h"

/**
 * Main function.
 *
 * \return 0 on succes, error code on error.
 */
int
main (int argn,                 ///< arguments number.
      char **argc)              ///< array of argument chains.
{
  char *buffer, *directory;
  directory = g_get_current_dir ();
  buffer = g_build_filename (directory, "po", NULL);
  bindtextdomain ("lightsoff", buffer);
  bind_textdomain_codeset ("lightsoff", "UTF-8");
  textdomain ("lightsoff");
  application = gtk_application_new ("es.csic.eead.auladei.lightsoff",
                                     G_APPLICATION_FLAGS_NONE);
  g_signal_connect (application, "activate", G_CALLBACK (window_activate),
                    NULL);
  g_application_run (G_APPLICATION (application), argn, argc);
  g_object_unref (application);
  window_destroy ();
  g_free (buffer);
  g_free (directory);
  return 0;
}
