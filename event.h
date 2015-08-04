#ifndef EVENT_H
#define EVENT_H

typedef struct
{
    unsigned int id;
    unsigned long time;
    int group;
    char *title;
    char *description;
} Event;

typedef struct _EventList EventList;
typedef struct _EventNode EventNode;

EventList *event_list_new();
int event_list_is_empty(EventList *list);
void event_list_insert(EventList *list, Event *event);
void event_list_reset_iterator(EventList *list);
int event_list_has_next(EventList *list);
Event *event_list_get_next(EventList *list);
void event_list_delete(EventList *list);

#endif //EVENT_H
