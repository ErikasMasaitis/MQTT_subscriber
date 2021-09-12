#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stdio.h>
#include "mqtt_config.h"

extern void push_event(struct event_node **head, struct event_node *new);
extern void push_topic(struct topic_node **head, struct topic_node *new);
extern void print_list(struct topic_node *start);
extern void delete_list(struct topic_node *head);
extern struct topic_node *create_topic(char *name, int qos);
extern struct event_node *create_event(struct event_node *temp_event);
extern int count_topics(struct topic_node *start);
extern int count_events(struct event_node *curr_event);
static void delete_events_list(struct event_node *head);

#endif