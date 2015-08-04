#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

typedef struct
{
    int id;
    int64_t time;
    int group;
    char *title;
    char *desc;
} Event;

typedef struct _EventList EventList;
typedef struct _EventNode EventNode;

EventList *event_list_new();
int event_list_is_empty(EventList *list);
void event_list_insert(EventList *list, Event *event);
void event_list_reset_iterator(EventList *list);
int event_list_has_next(EventList *list);
Event *event_list_get_next(EventList *list);
void event_delete(Event *event);
void event_list_delete(EventList *list);

int db_create();
int db_open();
void db_read_all();
void print_event(Event *event);
EventList *db_read_interval(int64_t start, int64_t end);
int db_write_event(Event *event);

#endif //EVENT_H
