/*
	ptouch-gtk - Simple GTK+ UI to print labels on a Brother P-Touch
	
	Copyright (C) 2015 Dominic Radermacher <dominic.radermacher@gmail.com>

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License version 3 as
	published by the Free Software Foundation
	
	This program is distributed in the hope that it will be useful, but
	WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	See the GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software Foundation,
	Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include <gtk/gtk.h>

#define BUILDER_XML_FILE "data/ptouch.ui"

typedef struct
{
	GtkWidget	*window;
	GtkWidget	*statusbar;
	GtkWidget	*text_view;
	guint		statusbar_context_id;
} PTouchEditor;

/* prototypes */
void error_message(const gchar *message);
void on_window_destroy(GtkWidget *object, PTouchEditor *editor);
gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event,
	PTouchEditor *editor);
gboolean init_app(PTouchEditor *editor);

void error_message(const gchar *message)
{
	GtkWidget *dialog;
	g_warning(message);		/* log to terminal window */
	/* create an error message dialog and display modally to the user */
	dialog = gtk_message_dialog_new(NULL,
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, message);
	gtk_window_set_title(GTK_WINDOW(dialog), "Error!");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void on_window_destroy(GtkWidget *object, PTouchEditor *editor)
{
	gtk_main_quit();
}

gboolean on_window_delete_event(GtkWidget *widget, GdkEvent *event, PTouchEditor *editor)
{
	return FALSE;   /* propogate event */
}

void show_about(PTouchEditor *editor)
{
	static const gchar * const authors[] = {
		"Dominic Radermacher <dominic.radermacher@gmail.com>",
		NULL
	};
	static const gchar copyright[] = "Copyright \xc2\xa9 2015 Dominic Radermacher";
	static const gchar comments[] = "PTouch Print";

	gtk_show_about_dialog(GTK_WINDOW(editor->window), "authors", authors,
		"comments", comments, "copyright", copyright,
		"version", "0.1",
		"website", "http://mockmoon-cybernetics.ch/",
		"program-name", "ptouch-gtk",
		"logo-icon-name", GTK_STOCK_EDIT, NULL);
}

gboolean init_app(PTouchEditor *editor)
{
	GtkBuilder *builder;
	GError *err=NULL;      
	guint id;

	/* use GtkBuilder to build our interface from the XML file */
	builder = gtk_builder_new();
	if (gtk_builder_add_from_file(builder, BUILDER_XML_FILE, &err) == 0) {
		error_message(err->message);
		g_error_free(err);
		return FALSE;
	}
	/* get the widgets which will be referenced in callbacks */
	editor->window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
	editor->statusbar = GTK_WIDGET(gtk_builder_get_object(builder, "statusbar"));
	editor->text_view = GTK_WIDGET(gtk_builder_get_object(builder, "text_view"));
	gtk_builder_connect_signals(builder, editor);
	/* free memory used by GtkBuilder object */
	g_object_unref(G_OBJECT(builder));
	gtk_window_set_default_icon_name(GTK_STOCK_EDIT);
	/* setup and initialize our statusbar */
	id = gtk_statusbar_get_context_id(GTK_STATUSBAR(editor->statusbar),
		"PTouch Print GTK+");
	editor->statusbar_context_id = id;
	return TRUE;
}

int main(int argc, char *argv[])
{
	PTouchEditor *editor;

	editor = g_slice_new(PTouchEditor);
	gtk_init(&argc, &argv);
	if (init_app(editor) == FALSE) {
		return 1;	/* error loading UI */
	}
	gtk_widget_show(editor->window);
	gtk_main();
	g_slice_free(PTouchEditor, editor);
}
