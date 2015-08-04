#include "efb.h"

GtkWidget *efb = NULL;

/**
 * Returns the instance of the widget.
 * Creates new instance when called for the first time.
 */
GtkWidget *efb_get_instance()
{
	if(!efb)
		efb_init();
	return efb;
}

/**
 * Creates the widget and its contents.
 */
void efb_init()
{
	efb = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	WebKitWebView *web = WEBKIT_WEB_VIEW(webkit_web_view_new());
	
	gtk_box_pack_start(GTK_BOX(efb), GTK_WIDGET(web), TRUE, TRUE, 0);
	webkit_web_view_load_uri(web, "http://rasmusantons.de/");
}
