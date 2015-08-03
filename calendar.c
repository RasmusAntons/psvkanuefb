#include "calendar.h"
#include <time.h>
#include <assert.h>
#include <stdlib.h>

struct _CalEvent
{
	int group;
	time_t time;
	char *title;
	char *description;
};

struct _CalDay
{
	int year, month, day;
	int n_events;
	CalEvent *events;
};

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
	calendar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	
	//HEADER
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
	for (d = 0; d < 7; d++)
	{
		for (w = 0; w < 6; w++)
		{
			CalDay calday;
			calday.day = 7 * w + d;
			GtkWidget *day = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
			sprintf(datebuffer, "%02d", calday.day);
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
 * Calculates the number of days in a month for a specific year and month.
 * 
 * @param year the year
 * @param month the month 1:January, ... , 12:December
 */
int days_in_month(int year, int month)
{
	assert(month > 0 && month < 13);
	if (month == 4 || month == 6 || month == 9 || month == 11)
	  return 30;
	else if (month == 2)
	{
		if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0))
			return 29;
		else
			return 28;
	}
	else
		return 31;
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
