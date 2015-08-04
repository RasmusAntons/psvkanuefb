#include "event.h"
#include <malloc.h>
#include <sqlite3.h>
#include <assert.h>
#include <unistd.h>

sqlite3 *db;
sqlite3_stmt *stmt;
char *zErrMsg = NULL;
int rc;
char *sql;

struct _EventList
{
	EventNode *first;
	EventNode *iterator;
	EventNode *last;
};

struct _EventNode
{
	Event *event;
	EventNode *next;
};

/**
 * Creates a new EventList.
 * The list must be deleted with event_list_delete().
 */
EventList *event_list_new()
{
	EventList *list = malloc(sizeof(EventList));
	list->first = NULL;
	return list;
}

/**
 * Checks if a list is empty.
 *
 * @return 1 if the list is empty, 0 if not
 */
int event_list_is_empty(EventList *list)
{
	return list->first == NULL;
}

/**
 * Inserts an Event into a list.
 */
void event_list_insert(EventList *list, Event *event)
{
	if (event_list_is_empty(list)) {
		list->first = malloc(sizeof(EventNode));
		list->first->next = NULL;
		list->last = list->first;
		list->iterator = list->first;
		list->first->event = event;
	} else {
		list->last->next = malloc(sizeof(EventNode));
		list->last = list->last->next;
		list->last->next = NULL;
		list->last->event = event;
	}
}

/**
 * Sets the inerator to the first element of a list.
 */
void event_list_reset_iterator(EventList *list)
{
	list->iterator = list->first;
}

/**
 * Checks if the iterator of a list has a next element to read.
 */
int event_list_has_next(EventList *list)
{
	return list->iterator != NULL;
}

/**
 * Returns the next Event for the iterator of a list
 *
 * @return The next Event or NULL if there is no next Element;
 */
Event *event_list_get_next(EventList *list)
{
	if (event_list_has_next(list)) {
		Event *event = list->iterator->event;
		list->iterator = list->iterator->next;
		return event;
	} else {
		return NULL;
	}
}

/**
 * Deletes the complete EventList and all contained Events.
 * TODO: actually delete the Events.
 */
void event_list_delete(EventList *list)
{
	EventNode *node;
	EventNode *next;
	node = list->first;
	
	while(node->next != NULL) {
		next = node->next;
		//free(node->event);
		free(node);
		node = next;
	}
	free(node);
	free(list);
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

/**
 * Creates a new database with empty table, database is left open.
 *
 * @return 1 if successful, 0 if not
 */
int db_create()
{
	rc = sqlite3_open("calendar.db", &db);
	sql = "CREATE TABLE events ( " \
	      "id INTEGER NOT NULL PRIMARY KEY AUTOINCREMENT UNIQUE, " \
	      "time INTEGER NOT NULL, " \
	      "title TEXT, " \
	      "desc TEXT )";
	rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	} else {
		printf("Empty calendar created\n");
	}
	return 1;
}

/**
 * Opens the Database, or creates new database if no file exists.
 *
 * @return 1 if successful, 0 if not
 */
int db_open()
{
	if (access("calendar.db", F_OK) == -1) {
		return db_create();
	} else {
		rc = sqlite3_open("calendar.db", &db);
		return rc == SQLITE_OK;
	}
}

/**
 * Debugging function that will be deleted
 */
void db_read_all()
{
	sqlite3_prepare_v2(db, "SELECT * FROM events", -1, &stmt, NULL);
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
		printf("%d:Event %s at %lld: %s\n",sqlite3_column_int(stmt, 0), sqlite3_column_text(stmt, 2), sqlite3_column_int64(stmt, 1), sqlite3_column_text(stmt, 3));
	}
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL Error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	} else {
		printf("SQLed successfully\n");
	}
}

/**
 * Returns an EventList containing all events in the given time interval.
 * The list must be deleted with event_list_delete().
 *
 * @param start start of the interval (formated as year * 10^8 + month * 10^6 + day * 10^4 + hour * 10^2 + minute)
 * @param end end of the intervar (formated same as start)
 * @return the resulting EventList
 */
EventList *db_read_interval(int64_t start, int64_t end)
{
	return NULL;
}

/**
 * Inserts an Event into the database
 *
 * @param event the Event to be inserted
 * @return 1 if successful, 0 if not
 */
int db_write_event(Event *event)
{
	sqlite3_prepare_v2(db, "INSERT INTO events (time, title, desc) VALUES (?1, ?2, ?3)", -1, &stmt, NULL);
	sqlite3_bind_int64(stmt, 1, event->time);
	sqlite3_bind_text(stmt, 2, event->title, -1, SQLITE_STATIC);
	sqlite3_bind_text(stmt, 3, event->desc, -1, SQLITE_STATIC);
	rc = sqlite3_step(stmt);
	if (rc != SQLITE_DONE) {
		fprintf(stderr, "SQL error(%d): %s\n", rc, sqlite3_errmsg(db));
		return 0;
	}
	sqlite3_finalize(stmt);
	return 1;
}