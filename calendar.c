#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include "calendar.h"
#include "event.h"

typedef struct
{
	GtkWidget *label,
	          *add_button,
	          *event_box;
} Day;

static const char *MONTHS[] = {"Januar", "Februar", "März", "April", "Mai", "Juni", "Juli", "August", "September", "Oktober", "November", "Dezember"};
void change_current_month(GtkWidget *widget, gpointer *data);
void reset_current_month(GtkWidget *widget, gpointer *data);
int populate_events();

GtkWidget *calendar = NULL;
GtkWidget *header = NULL;
GtkWidget *daygrid = NULL;
Day *days[42];

int current_year = 2015;
int current_month = 8;
int first, last;

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
	event.time = 201508051500LL;
	event.title = "AuD2";
	event.desc = "Für Aud2 lernen";
	db_write_event(&event);
	*/
	calendar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	//HEADER BAR
	header = gtk_header_bar_new();
	gtk_header_bar_set_title(GTK_HEADER_BAR(header), "August");
	gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header), "2015");
	
	GtkWidget *button_previous_month = gtk_button_new_with_label("früher");
	g_signal_connect(button_previous_month, "clicked", G_CALLBACK(change_current_month), (gpointer) -1);
	GtkWidget *button_now = gtk_button_new_with_label("heute");
	g_signal_connect(button_now, "clicked", G_CALLBACK(reset_current_month), NULL);
	GtkWidget *button_next_month = gtk_button_new_with_label("später");
	g_signal_connect(button_next_month, "clicked", G_CALLBACK(change_current_month), (gpointer) 1);
	
	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button_previous_month);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), button_now);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(header), button_next_month);
	gtk_box_pack_start(GTK_BOX(calendar), header, FALSE, FALSE, 0);
	
	//DAYGRID
	daygrid = gtk_grid_new();
	gtk_grid_set_row_homogeneous(GTK_GRID(daygrid), TRUE);
	gtk_grid_set_column_homogeneous(GTK_GRID(daygrid), TRUE);
	int d, w;
	for (d = 0; d < 7; d++) {
		for (w = 0; w < 6; w++) {
			Day *day = malloc(sizeof(Day));
			day->label = gtk_label_new("0");
			day->add_button = gtk_button_new_with_label("+");
			day->event_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
			GtkWidget *day_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
			GtkWidget *day_vbox_left = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

			gtk_box_pack_start(GTK_BOX(day_vbox_left), day->label, FALSE, FALSE, 0);
			gtk_box_pack_start(GTK_BOX(day_vbox_left), day->add_button, FALSE, FALSE, 0);
			gtk_box_pack_start(GTK_BOX(day_hbox), day_vbox_left, FALSE, FALSE, 0);
			gtk_box_pack_start(GTK_BOX(day_hbox), day->event_box, TRUE, TRUE, 0);
			gtk_widget_set_hexpand(day_hbox, TRUE);
			gtk_widget_set_vexpand(day_hbox, TRUE);
			gtk_grid_attach(GTK_GRID(daygrid), day_hbox, d, w, 1, 1);

			days[7 * w + d] = day;
		}
	}
	gtk_box_pack_start(GTK_BOX(calendar), daygrid, TRUE, TRUE, 0);

	reset_current_month(NULL, NULL);
}

/**
 * Gets called to remove an event (only the GtkWidget) from a given day;
 *
 * @param widget the widget to remove
 * @param data the number of the day(in the days array), as an int, not a gpointer
 */
void remove_gtk_event(GtkWidget *widget, gpointer data)
{
	gtk_container_remove(GTK_CONTAINER(days[(int) data]->event_box), widget);
}

/**
 * Gets called to change the current month.
 */
void change_current_month(GtkWidget *widget, gpointer *data)
{
	current_month += (int) data;
	if (current_month < 1) {
		current_month = 12;
		current_year--;
	} else if (current_month > 12) {
		current_month = 1;
		current_year++;
	}
	gtk_header_bar_set_title(GTK_HEADER_BAR(header), MONTHS[current_month - 1]);
	char buffer[5];
	snprintf(buffer, 5, "%d", current_year);
	gtk_header_bar_set_subtitle(GTK_HEADER_BAR(header), buffer);

	first = first_day_of_month(current_year, current_month);
	last = first + days_in_month(current_year, current_month);
	int i;
	for (i = 0; i < 42; i++) // remove all old events
		gtk_container_foreach(GTK_CONTAINER(days[i]->event_box), remove_gtk_event, (gpointer) i);
	for (i = 0; i < first; i++) // these days don't exist, maybe hide the add-button?
		gtk_label_set_text(GTK_LABEL(days[i]->label), "");
	for (i = first; i < last; i ++) { // print the number of the actual day into the box
		snprintf(buffer, 5, "%d", i - first + 1);
		gtk_label_set_text(GTK_LABEL(days[i]->label), buffer);
	}
	for (i = last; i < 42; i++) // these days also don't exist
		gtk_label_set_text(GTK_LABEL(days[i]->label), "");

	populate_events();
}

/**
 * Reads all events for the current month from the database and puts GtkWidgets in the depending day.
 *
 * @return 1 if successful, 0 if not (actually always 1, but I will fix that)
 */
int populate_events()
{
	EventList *events = db_read_interval(
			INT64_C(current_year * 100000000LL + current_month * 1000000L),
			INT64_C(current_year * 100000000LL + current_month * 1000000L + 312359L)
	);

	while (event_list_has_next(events)) {
		Event *event = event_list_get_next(events);
		print_event(event);
		int date = (event->time / 10000) % 100;
		GtkWidget *label = gtk_label_new(event->title);
		gtk_box_pack_start(GTK_BOX(days[first + date - 1]->event_box), label, TRUE, TRUE, 0);
		gtk_widget_show_all(days[first + date - 1]->event_box);
	}

	event_list_delete(events);
	return 1;
}

/**
 * Resets the current month to the actual month
 */
void reset_current_month(GtkWidget *widget, gpointer *data)
{
	time_t rawtime;
	struct tm *timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	current_year = timeinfo->tm_year + 1900;
	current_month = timeinfo->tm_mon + 1;
	change_current_month(widget, (gpointer) 0);
}

/**
 * Calculates the number of days in a month for a specific year and month.
 *
 * @param year the year
 * @param month the month 1:January, ... , 12:December
 */
int days_in_month(int year, int month)
{
	assert(month > 0 && month < 13);
	if (month == 4 || month == 6 || month == 9 || month == 11) {
		return 30;
	} else if (month == 2) {
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
			return 29;
		else
			return 28;
	} else {
		return 31;
	}
}

/**
 * Calculates the first weekday of the year for a specific year.
 * 0:Monday, ... ,6:Sunday
 *
 * @param year the year
 */
int first_day_of_year(int year)
{
	return (int) (((year - 1) * 365L + (year - 1) / 4 - (year - 1) / 100 + (year - 1) / 400) % 7);
}

/**
 * Calculates the first weekday of the month for a specific year and month.
 * 0:Monday, ... ,6:Sunday
 *
 * @param year the year
 * @param month the month 1:January, ... , 12:December
 */
int first_day_of_month(int year, int month)
{
	int offset = 0;
	int m;
	for (m = 1; m < month; m++)
		offset += days_in_month(year, m);
	return (first_day_of_year(year) + offset) % 7;
}
