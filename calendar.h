#ifndef CALENDAR_H
#define CALENDAR_H

#include <gtk/gtk.h>
/**
 * Returns the instance of the widget.
 * Creates new instance when called for the first time.
 */
GtkWidget *calendar_get_instance();

/**
 * Creates the widget and its contents.
 */
void calendar_init();

#endif
