#include "event.h"
#include <malloc.h>

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

EventList *event_list_new()
{
	EventList *list = malloc(sizeof(EventList));
	list->first = NULL;
	return list;
}

int event_list_is_empty(EventList *list)
{
	return list->first == NULL;
}

void event_list_insert(EventList *list, Event *event)
{
	if (event_list_is_empty(list))
	{
		list->first = malloc(sizeof(EventNode));
		list->first->next = NULL;
		list->last = list->first;
		list->iterator = list->first;
		list->first->event = event;
	}
	else
	{
		list->last->next = malloc(sizeof(EventNode));
		list->last = list->last->next;
		list->last->next = NULL;
		list->last->event = event;
	}
}

void event_list_reset_iterator(EventList *list)
{
	list->iterator = list->first;
}

int event_list_has_next(EventList *list)
{
	return list->iterator != NULL;
}

Event *event_list_get_next(EventList *list)
{
	if (list->iterator == NULL)
	{
		return NULL;
	}
	else
	{
		Event *event = list->iterator->event;
		list->iterator = list->iterator->next;
		return event;
	}
}

void event_list_delete(EventList *list)
{
	EventNode *node;
	EventNode *next;
	node = list->first;
	
	while(node->next != NULL)
	{
		next = node->next;
		//free(node->event);
		free(node);
		node = next;
	}
	free(node);
	free(list);
}
