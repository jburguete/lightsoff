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
 * \file interface.h
 * \brief Header file of the lights off game interface.
 * \author Javier Burguete Tolosa.
 * \copyright Copyright 2016-2021, Javier Burguete Tolosa.
 */
#ifndef INTERFACE__H
#define INTERFACE__H 1

extern GtkApplication *application;
extern GtkWindow *window;

void window_destroy ();
void window_activate (GtkApplication * application);

#if GTK4

#define button_new_from_icon_name gtk_button_new_from_icon_name
#define image_new_from_icon_name gtk_image_new_from_icon_name
#define widget_show gtk_widget_show

#else

#define gtk_button_set_child(widget, child) \
	(gtk_container_add(GTK_CONTAINER(widget), child))
#define gtk_button_get_child(widget) \
	((GtkWidget *)gtk_container_get_children(widget)->data)
#define gtk_window_set_child(widget, child) \
	(gtk_container_add(GTK_CONTAINER(widget), child))
#define gtk_window_destroy gtk_widget_destroy
#define gtk_grid_remove(widget, child) (gtk_widget_destroy(child))
#define button_new_from_icon_name(widget) \
	(gtk_button_new_from_icon_name(widget, GTK_ICON_SIZE_BUTTON))
#define image_new_from_icon_name(widget) \
	(gtk_image_new_from_icon_name(widget, GTK_ICON_SIZE_BUTTON))
#define gtk_box_append(widget, child) \
	(gtk_box_pack_start(widget, child, 0, 0, 0))
#define gtk_check_button_get_active(widget) \
	(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)))
#define gtk_check_button_set_active(widget, active) \
	(gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), active))
#define gtk_image_get_paintable gtk_image_get_pixbuf
#define widget_show gtk_widget_show_all

#endif

#endif
