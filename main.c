#include <gtk/gtk.h>
#include <stdlib.h>
#include "efb.h"
#include "calendar.h"

/**
 * Gets calles if any key is pressed on the main window.
 * Sets fullscreen if pressed key is F10 or F11
 */
void on_key_pressed(GtkWidget *window, GdkEventKey *event, gpointer *user_data)
{
	if (event->keyval == 0xffc7) // F10
		gtk_window_unfullscreen(GTK_WINDOW(window));
	if (event->keyval == 0xffc8) // F11
			gtk_window_fullscreen(GTK_WINDOW(window));
}

/**
 * Gets called when the Application is started, creates the main window.
 */
void activate(GtkApplication *app, gpointer user_data)
{
	GtkWidget *window;
	
	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW(window), "PSV Kanu EFB");
	gtk_window_set_default_size(GTK_WINDOW(window), 600, 300);
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(on_key_pressed), NULL);
	
	GtkWidget *box_main;
	box_main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), box_main);
	
	GtkWidget *notebook_main;
	notebook_main = gtk_notebook_new();
	gtk_box_pack_start(GTK_BOX(box_main), notebook_main, TRUE, TRUE, 0);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook_main), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook_main));
	
	//KALENDER
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_main), calendar_get_instance(), gtk_label_new("Kalender"));
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook_main), calendar_get_instance(), TRUE);
	
	//FAHRTENBUCH
	gtk_notebook_append_page(GTK_NOTEBOOK(notebook_main), efb_get_instance(), gtk_label_new("Fahrtenbuch"));
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook_main), efb_get_instance(), TRUE);
	
	gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{
	GtkApplication *app;
	int status;
	
	app = gtk_application_new("de.rasmusantons.psvkanuefb", G_APPLICATION_FLAGS_NONE);
	g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
	status = g_application_run(G_APPLICATION(app), argc, argv);
	g_object_unref(app);
	
	return status;
}
