#include "calendar.h"
#include "event.h"

GtkWidget *calendar = NULL;
GtkWidget *header = NULL;
GtkWidget *daygrid = NULL;

int current_year = 2015;
int current_month = 7;

/**
 * Returns the instance of the widget.
 * Creates new instance when called for the first time.
 */
GtkWidget *calendar_get_instance()
{
	if(!calendar)
		calendar_init();
	return calendar;
}

/**
 * Creates the widget and its contents.
 */
void calendar_init()
{
	db_open();
	/*
	Event event;
	event.time = 201509051500LL;
	event.title = "AuD2";
	event.desc = "Für Aud2 lernen";
	db_write_event(&event);
	*/
	db_read_all();
	calendar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	//HEADER BAR
	header = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(header), "August");
	gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header), "2015");
	
	GtkWidget *button_previous_month = gtk_button_new_with_label("früher");
	GtkWidget *button_next_month = gtk_button_new_with_label("später");
	
	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button_previous_month);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(header), button_next_month);
	gtk_box_pack_start(GTK_BOX(calendar), header, FALSE, FALSE, 10);
	
	//DAYGRID
	daygrid = gtk_grid_new();
	int d, w;
	char datebuffer[8];
	for (d = 0; d < 7; d++) {
		for (w = 0; w < 6; w++) {
			GtkWidget *day = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
			sprintf(datebuffer, "%02d", 7 * w + d);
			GtkWidget *day_date = gtk_label_new(datebuffer);
			gtk_box_pack_start(GTK_BOX(day), day_date, FALSE, FALSE, 0);
			//GtkWidget *day = gtk_button_new_with_label("day");
			gtk_widget_set_hexpand(day, TRUE);
			gtk_widget_set_vexpand(day, TRUE);
			gtk_grid_attach(GTK_GRID(daygrid), day, d, w, 1, 1);
		}
	}
	gtk_box_pack_start(GTK_BOX(calendar), daygrid, TRUE, TRUE, 0);
}

/**
 * callback function for debugging
 * prints stuff out
 */
static int callback(void *data, int argc, char **argv, char **azColName){
	int i;
	fprintf(stderr, "%s: ", (const char*)data);
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}
