/*
LightsOff:
A clone of the Tim Horton's LightsOff program written in C, adding variable
board sizes, undo and clear movements and a solver.

Copyright 2016-2021, Javier Burguete Tolosa.

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
 * \copyright Copyright 2016-2021, Javier Burguete Tolosa.
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
GList *list_undo = NULL;
///< List of undo movements.

GtkImage *image;                ///< GtkImage to draw the logo.
GtkSpinButton *spin_rows;       ///< GtkSpinButton to set the rows number.
GtkSpinButton *spin_columns;    ///< GtkSpinButton to set the columns number.
GtkSpinButton *spin_level;      ///< GtkSpinButton to set the game level.
GtkLabel *label_movements;      ///< Label showing the number of user movements.
GtkCheckButton *button_input;   ///< Button to set the new games input method.
GtkToggleButton **array_buttons = NULL; ///< Array of light buttons.
GtkImage **array_images = NULL; ///< Array of light images.
GtkImage **array_solutions = NULL;      ///< Array of solution images.
GtkButton *button_new;          ///< New game tool button.
GtkButton *button_options;      ///< Game options tool button.
GtkButton *button_clear;        ///< Clear game tool button.
GtkButton *button_undo;         ///< Undo last movement tool button.
GtkButton *button_redo;         ///< Redo last undo movement tool button.
GtkButton *button_solution;     ///< Solution tool button.
GtkComboBoxText *combo_theme;   ///< GtkComboBoxText to set the theme.
GtkGrid *grid;                  ///< Light buttons grid.
GtkWindow *window;              ///< Main window.
GtkApplication *application;    ///< Application.

/**
 * Function to free to undo movements list.
 */
static void
window_destroy_undo ()
{
  GList *list;
#if DEBUG
  fprintf (stderr, "window_destroy_undo: start\n");
#endif
  for (list = list_undo; list; list = list->next)
    free (list->data);
  g_list_free (list_undo);
  list_undo = NULL;
#if DEBUG
  fprintf (stderr, "window_destroy_undo: end\n");
#endif
}

/**
 * Function to free memory assigned by the main window.
 */
void
window_destroy ()
{
  GList *list;
  unsigned int i;
#if DEBUG
  fprintf (stderr, "window_destroy: start\n");
#endif
  for (i = 0; i < window_squares; ++i)
    gtk_grid_remove (grid, GTK_WIDGET (array_buttons[i]));
  free (array_buttons);
  window_destroy_undo ();
  for (list = list_movements; list; list = list->next)
    free (list->data);
  g_list_free (list_movements);
  list_movements = NULL;
#if DEBUG
  fprintf (stderr, "window_destroy: end\n");
#endif
}

/**
 * Function to update the buttons sensitivity.
 */
static inline void
window_update ()
{
#if DEBUG
  fprintf (stderr, "window_update: start\n");
#endif
#if !GTK4
  gtk_widget_show_all (GTK_WIDGET (grid));
#endif
  gtk_widget_set_sensitive (GTK_WIDGET (button_clear), window_movements);
  gtk_widget_set_sensitive (GTK_WIDGET (button_undo), window_movements);
  gtk_widget_set_sensitive (GTK_WIDGET (button_redo), (size_t) list_undo);
#if DEBUG
  fprintf (stderr, "window_update: end\n");
#endif
}

/**
 * Function to set the light buttons.
 */
static void
window_set ()
{
  char label[64];
  GtkToggleButton *button;
  GtkImage *image;
#if !GTK4
  GList *child;
#endif
  unsigned int i, id;
#if DEBUG
  fprintf (stderr, "window_set: start\n");
#endif
  for (i = 0; i < window_squares; ++i)
    {
      button = array_buttons[i];
      id = array_ids[i];
#if !GTK4
      child = gtk_container_get_children (GTK_CONTAINER (button));
      if (child)
        gtk_widget_destroy (GTK_WIDGET (child->data));
#endif
      g_signal_handler_block (button, id);
      if (status & (1L << i))
        {
          gtk_toggle_button_set_active (button, 1);
          image = (GtkImage *)
            image_new_from_icon_name (light_images[window_theme]);
        }
      else
        {
          gtk_toggle_button_set_active (button, 0);
          image = NULL;
        }
#if !GTK4
      if (image)
        gtk_container_add (GTK_CONTAINER (button), GTK_WIDGET (image));
#else
      gtk_button_set_child (GTK_BUTTON (button), GTK_WIDGET (image));
#endif
      g_signal_handler_unblock (button, id);
    }
  snprintf (label, 64, _("Number of movements: %u"), window_movements);
  gtk_label_set_text (label_movements, label);
  window_update ();
#if DEBUG
  fprintf (stderr, "window_set: end\n");
#endif
}

/**
 * Function to set a square.
 */
static void
window_toggle (GtkToggleButton * button)        ///< button to set.
{
  unsigned int i;
#if DEBUG
  fprintf (stderr, "window_toggle: start\n");
#endif
  for (i = 0; button != array_buttons[i]; ++i);
  set (&status, i);
  window_set ();
#if DEBUG
  fprintf (stderr, "window_toggle: end\n");
#endif
}

/**
 * Function to do a movement.
 */
static void
window_move (GtkToggleButton * button)  ///< button to move.
{
  GtkMessageDialog *dialog;
  unsigned int *data;
  unsigned int i;
#if DEBUG
  fprintf (stderr, "window_move: start\n");
#endif
  for (i = 0; button != array_buttons[i]; ++i);
  move (&status, i);
  window_destroy_undo ();
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
      widget_show (GTK_WIDGET (dialog));
      g_signal_connect_swapped (dialog, "response",
                                G_CALLBACK (gtk_window_destroy),
                                GTK_WINDOW (dialog));
    }
#if DEBUG
  fprintf (stderr, "window_move: end\n");
#endif
}

/**
 * Function to undo an user movement.
 */
static void
window_undo ()
{
  unsigned int *data;
  unsigned int i;
#if DEBUG
  fprintf (stderr, "window_undo: start\n");
#endif
  i = *(unsigned int *) (list_movements->data);
  move (&status, i);
  data = (unsigned int *) malloc (sizeof (unsigned int));
  *data = i;
  list_undo = g_list_prepend (list_undo, data);
  free (list_movements->data);
  list_movements = list_movements->next;
  --window_movements;
  window_set ();
#if DEBUG
  fprintf (stderr, "window_undo: end\n");
#endif
}

/**
 * Function to clear all user movements.
 */
static void
window_clear ()
{
#if DEBUG
  fprintf (stderr, "window_clear: start\n");
#endif
  while (window_movements)
    window_undo ();
#if DEBUG
  fprintf (stderr, "window_clear: end\n");
#endif
}

/**
 * Function to undo an user movement.
 */
static void
window_redo ()
{
  unsigned int *data;
  unsigned int i;
#if DEBUG
  fprintf (stderr, "window_redo: start\n");
#endif
  data = (unsigned int *) malloc (sizeof (unsigned int));
  i = *(unsigned int *) (list_undo->data);
  *data = i;
  move (&status, i);
  list_movements = g_list_prepend (list_movements, data);
  ++window_movements;
  free (list_undo->data);
  list_undo = g_list_delete_link (list_undo, list_undo);
  window_set ();
#if DEBUG
  fprintf (stderr, "window_redo: end\n");
#endif
}

/**
 * Function to input a custom game.
 */
static void
window_custom ()
{
  GtkWidget *widget[] = {
    GTK_WIDGET (button_new),
    GTK_WIDGET (button_options),
    GTK_WIDGET (button_clear),
    GTK_WIDGET (button_undo),
    GTK_WIDGET (button_redo),
    GTK_WIDGET (button_solution),
  };
  GtkMessageDialog *dialog;
  GMainLoop *loop;
  unsigned int i;
  for (i = 0; i < 6; ++i)
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
  widget_show (GTK_WIDGET (dialog));
  loop = g_main_loop_new (NULL, 0);
  g_signal_connect_swapped (dialog, "response", G_CALLBACK (g_main_loop_quit),
                            loop);
  g_main_loop_run (loop);
  g_main_loop_unref (loop);
#if !GTK4
  gtk_widget_destroy (GTK_WIDGET (dialog));
#else
  gtk_window_destroy (GTK_WINDOW (dialog));
#endif
  for (i = 0; i < window_squares; ++i)
    {
      g_signal_handler_disconnect (array_buttons[i], array_ids[i]);
      array_ids[i] = g_signal_connect (array_buttons[i], "clicked",
                                       G_CALLBACK (window_move), NULL);
    }
  for (i = 0; i < 6; ++i)
    gtk_widget_set_sensitive (widget[i], 1);
  window_update ();
}

/**
 * Function to do a new game.
 */
static void
window_new_game ()
{
  unsigned int i, j, k;
#if DEBUG
  fprintf (stderr, "window_new_game: start\n");
#endif
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
#if DEBUG
        fprintf (stderr, "window_new_game: i=%u j=%u k=%u\n", i, j, k);
#endif
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
  if (window_input)
    window_custom ();
#if DEBUG
  fprintf (stderr, "window_new_game: end\n");
#endif
}

/**
 * Function to update the options dialog properties.
 */
void
window_options_update ()
{
  unsigned int rows, columns;
  rows = gtk_spin_button_get_value_as_int (spin_rows);
  columns = gtk_spin_button_get_value_as_int (spin_columns);
  gtk_spin_button_set_range (spin_level, 1., rows * columns);
  gtk_widget_set_sensitive (GTK_WIDGET (spin_level),
                            !gtk_check_button_get_active (button_input));
}

/**
 * Function to close the options dialog.
 */
static void
window_options_close (GtkDialog * dlg,  ///< options GtkDialog.
                      int response_id)  ///< response identifier.
{
  if (response_id == GTK_RESPONSE_OK)
    {
      window_rows = gtk_spin_button_get_value_as_int (spin_rows);
      window_columns = gtk_spin_button_get_value_as_int (spin_columns);
      level = gtk_spin_button_get_value_as_int (spin_level);
      window_input = gtk_check_button_get_active (button_input);
      window_theme = gtk_combo_box_get_active (GTK_COMBO_BOX (combo_theme));
    }
#if !GTK4
  gtk_widget_destroy (GTK_WIDGET (dlg));
#else
  gtk_window_destroy (GTK_WINDOW (dlg));
#endif
}

/**
 * Function to set the game options.
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
  GtkBox *content;
  unsigned int i;
  dialog = (GtkDialog *)
    gtk_dialog_new_with_buttons (_("Options"),
                                 window,
                                 GTK_DIALOG_MODAL,
                                 _("_OK"), GTK_RESPONSE_OK,
                                 _("_Cancel"), GTK_RESPONSE_CANCEL, NULL);
  content = (GtkBox *) gtk_dialog_get_content_area (dialog);
  grid = (GtkGrid *) gtk_grid_new ();
  gtk_box_append (content, GTK_WIDGET (grid));
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
  g_signal_connect (button_input, "toggled",
                    G_CALLBACK (window_options_update), NULL);
  combo_theme = (GtkComboBoxText *) gtk_combo_box_text_new ();
  for (i = 0; i < N_THEMES; ++i)
    gtk_combo_box_text_append_text (combo_theme, window_themes[i]);
  gtk_grid_attach (grid, GTK_WIDGET (combo_theme), 1, 4, 1, 1);
  gtk_spin_button_set_value (spin_rows, window_rows);
  gtk_spin_button_set_value (spin_columns, window_columns);
  gtk_spin_button_set_value (spin_level, level);
  gtk_check_button_set_active (button_input, window_input);
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo_theme), window_theme);
  g_signal_connect (dialog, "response", G_CALLBACK (window_options_close),
                    NULL);
  widget_show (GTK_WIDGET (dialog));
}

/**
 * Function to print the optimal solution.
 */
static void
window_solve ()
{
  GtkMessageDialog *dialog;
  GtkToggleButton *button;
  GtkImage *image;
#if !GTK4
  GList *list;
#endif
  int i;
#if DEBUG
  fprintf (stderr, "window_solve: start\n");
#endif
  nmovements = play ();
  for (i = 0; i < nmovements; ++i)
    {
      button = array_buttons[movement[i]];
#if !GTK4
      list = gtk_container_get_children (GTK_CONTAINER (button));
      if (list)
        gtk_widget_destroy (GTK_WIDGET (list->data));
#endif
      image = (GtkImage *)
        image_new_from_icon_name (solution_images[window_theme]);
#if !GTK4
      gtk_widget_show (GTK_WIDGET (image));
#endif
      gtk_button_set_child (GTK_BUTTON (button), GTK_WIDGET (image));
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
      g_signal_connect_swapped (dialog, "response",
                                G_CALLBACK (gtk_window_destroy),
                                GTK_WINDOW (dialog));
      widget_show (GTK_WIDGET (dialog));
    }
#if DEBUG
  fprintf (stderr, "window_solve: end\n");
#endif
}

/**
 * Function to show an about dialog.
 */
static void
window_about ()
{
  static const gchar *authors[] = {
    "Javier Burguete Tolosa <jburguete@eead.csic.es>",
    NULL
  };
#if !GTK4
  GdkPixbuf *pixbuf, *paintable;
#else
  GdkPaintable *paintable;
#endif
#if !GTK4
  pixbuf = gtk_image_get_pixbuf (image);
  paintable = gdk_pixbuf_scale_simple (pixbuf, 16, 16, GDK_INTERP_BILINEAR);
#else
  paintable = gtk_image_get_paintable (image);
#endif
  gtk_show_about_dialog
    (window,
     "program-name", "LightsOff",
     "comments", _("Lights off game"),
     "authors", authors,
     "copyright", "Copyright 2016-2021, Javier Burguete Tolosa",
     "license-type", GTK_LICENSE_BSD,
     "version", "1.2",
     "logo", paintable,
     "website", "http://github.com/jburguete/lightsoff", NULL);
}

/**
 * Function to activate the window.
 */
void
window_activate (GtkApplication * application)
{
  GtkBox *box;
  GtkButton *button;
  GtkHeaderBar *bar;
#if !GTK4
  GdkPixbuf *pixbuf, *paintable;
#endif

#if DEBUG
  fprintf (stderr, "window_activate: start\n");
#endif

  // Main window
  window = (GtkWindow *) gtk_application_window_new (application);
#if GTK4
  gtk_window_set_title (window, _("Lights off"));
#endif
  gtk_widget_set_size_request (GTK_WIDGET (window), 320, 440);

  // Logo
  image = (GtkImage *) gtk_image_new_from_file ("logo.png");
#if !GTK4
  pixbuf = gtk_image_get_pixbuf (image);
  gtk_window_set_default_icon (pixbuf);
#endif

  // Header bar
  bar = (GtkHeaderBar *) gtk_header_bar_new ();
#if !GTK4
  gtk_header_bar_set_title (bar, _("Lights off"));
  gtk_header_bar_set_show_close_button (bar, 1);
  paintable = gdk_pixbuf_scale_simple (pixbuf, 16, 16, GDK_INTERP_BILINEAR);
  gtk_header_bar_pack_start (bar, gtk_image_new_from_pixbuf (paintable));
#else
  gtk_header_bar_pack_start (bar, GTK_WIDGET (image));
#endif
  gtk_window_set_titlebar (window, GTK_WIDGET (bar));

  // Grid
  grid = (GtkGrid *) gtk_grid_new ();
  gtk_window_set_child (window, GTK_WIDGET (grid));

  // Toolbar
  box = (GtkBox *) gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_grid_attach (grid, GTK_WIDGET (box), 0, -1, N_MAX_COLUMNS, 1);

  // New game tool
  button_new = (GtkButton *) button_new_from_icon_name ("document-new");
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_new), _("New game"));
  gtk_box_append (box, GTK_WIDGET (button_new));
  g_signal_connect (button_new, "clicked", G_CALLBACK (window_new_game), NULL);

  // Options tool
  button_options = (GtkButton *)
    button_new_from_icon_name ("preferences-desktop");
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_options), _("Preferences"));
  gtk_box_append (box, GTK_WIDGET (button_options));
  g_signal_connect (button_options, "clicked", G_CALLBACK (window_options),
                    NULL);

  // Clear tool
  button_clear = (GtkButton *) button_new_from_icon_name ("edit-clear");
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_clear),
                               _("Clear all movements"));
  gtk_box_append (box, GTK_WIDGET (button_clear));
  g_signal_connect (button_clear, "clicked", G_CALLBACK (window_clear), NULL);

  // Undo tool
  button_undo = (GtkButton *) button_new_from_icon_name ("edit-undo");
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_undo),
                               _("Undo the last movement"));
  gtk_box_append (box, GTK_WIDGET (button_undo));
  g_signal_connect (button_undo, "clicked", G_CALLBACK (window_undo), NULL);

  // Redo tool
  button_redo = (GtkButton *) button_new_from_icon_name ("edit-redo");
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_redo),
                               _("Redo the last undo movement"));
  gtk_box_append (box, GTK_WIDGET (button_redo));
  g_signal_connect (button_redo, "clicked", G_CALLBACK (window_redo), NULL);

  // Solution tool
  button_solution = (GtkButton *) button_new_from_icon_name ("system-help");
  gtk_widget_set_tooltip_text (GTK_WIDGET (button_solution),
                               _("Show the optimal solution"));
  gtk_box_append (box, GTK_WIDGET (button_solution));
  g_signal_connect (button_solution, "clicked", G_CALLBACK (window_solve),
                    NULL);

  // About tool
  button = (GtkButton *) button_new_from_icon_name ("help-about");
  gtk_widget_set_tooltip_text (GTK_WIDGET (button), _("About"));
  gtk_box_append (box, GTK_WIDGET (button));
  g_signal_connect (button, "clicked", G_CALLBACK (window_about), NULL);

  // Exit tool
  button = (GtkButton *) button_new_from_icon_name ("application-exit");
  gtk_widget_set_tooltip_text (GTK_WIDGET (button), _("Exit"));
  gtk_box_append (box, GTK_WIDGET (button));
  g_signal_connect_swapped (button, "clicked",
                            G_CALLBACK (g_application_quit),
                            G_APPLICATION (application));

  // Movements label
  label_movements = (GtkLabel *) gtk_label_new (NULL);
  gtk_grid_attach (grid, GTK_WIDGET (label_movements),
                   0, N_MAX_ROWS, N_MAX_COLUMNS, 1);

  // Show main window
  widget_show (GTK_WIDGET (window));

  // First game
  window_new_game ();

#if DEBUG
  fprintf (stderr, "window_activate: end\n");
#endif
}
