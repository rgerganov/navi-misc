#include <glade/glade.h>
#include <gnome.h>

#ifndef XCHAT_GNOME_GUI_H
#define XCHAT_GNOME_GUI_H

typedef struct {
	GladeXML *xml;
	GnomeApp *main_window;
	GtkDialog *preferences_dialog;
	GnomeAbout *about;
} XChatGUI;

extern XChatGUI gui;

gboolean initialize_gui();
int xtext_get_stamp_str (time_t tim, char **ret);

#endif
