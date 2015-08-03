#ifndef EFB_H
#define EFB_H

#include <gtk/gtk.h>
#include <webkit2/webkit2.h>

/**
 * Returns the instance of the widget.
 * Creates new instance when called for the first time.
 */
GtkWidget *efb_get_instance();

/**
 * Creates the widget and its contents.
 */
void efb_init();

#endif
