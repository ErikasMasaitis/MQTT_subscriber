#include "linked_list.h"
/**
 * add element to linked list topic_node
 */
extern void push_topic(struct topic_node **head, struct topic_node *new)
{
    if (*head != NULL) {
            new->next = *head;
    }
    *head = new;
}
/**
 * add element to linked list event_node
 */
extern void push_event(struct event_node **head, struct event_node *new)
{
    if (*head != NULL) {
            new->next = *head;
    }
    *head = new;
}
/**
 * fully delete linked list
 */
extern void delete_list(struct topic_node *head)
{
    struct topic_node *temp = head;
    struct topic_node *next;
    while (temp != NULL) {
            next = temp->next;
            delete_events_list(temp->head_event);
            free(temp);
            temp = next;
    }
}
/**
 * delete events linked list
 */
static void delete_events_list(struct event_node *head)
{
    struct event_node *temp = head;
    struct event_node *next;
    while (temp != NULL) {
            next = temp->next;
            free(temp);
            temp = next;
    }
}
/**
 *  copy name and qos to new struct, ready to be pushed to linked list
 */
extern struct topic_node *create_topic(char *name, int qos)
{
    struct topic_node *topic;
    topic = (struct topic_node *) calloc(1, sizeof(struct topic_node));
    if (topic == NULL)
            return topic;
    
    strcpy(topic->name, name);
    topic->qos = qos;
    topic->next = NULL;

    return topic;
}
/**
 * copy contents of temp_event to new struct, ready to be pushed to linked list
 */
extern struct event_node *create_event(struct event_node *temp_event)
{
    struct event_node *event = NULL;
    event = (struct event_node *) malloc(sizeof(struct event_node));

    if (event == NULL)
            return event;

    strcpy(event->topic, temp_event->topic);
    strcpy(event->type, temp_event->type);
    strcpy(event->opt_value, temp_event->opt_value);
    event->dec_operator = temp_event->dec_operator;
    event->str_operator = temp_event->str_operator;
    strcpy(event->user_email, temp_event->user_email);
    strcpy(event->smtp_ip, temp_event->smtp_ip);
    event->credentials = temp_event->credentials;
    event->secure = temp_event->secure;
    strcpy(event->smtp_port, temp_event->smtp_port);
    strcpy(event->username, temp_event->username);
    strcpy(event->password, temp_event->password);
    strcpy(event->sender_email, temp_event->sender_email);
    event->next = NULL;

    return event;
}
/**
 * prints topics and its events
 */
extern void print_list(struct topic_node *start)
{
    struct topic_node *curr = start;
    while (curr != NULL) {
            struct event_node *curr_event = curr->head_event;
            printf("Name of topic - %s\n", curr->name);
            if (curr_event != NULL) {
                    while (curr_event != NULL) {
                            printf("Name event - %s\n", curr_event->topic);
                            curr_event = curr_event->next; 
                    }
            }
            curr = curr->next;
    }
}
/**
 * @return: count of topics
 */
extern int count_topics(struct topic_node *start)
{
    struct topic_node *curr = start;
    int count = 0;

    while (curr != NULL) {
            count++;
            curr = curr->next;
    }

    return count;
}
/**
 * @return: count of events
 */
extern int count_events(struct event_node *curr_event)
{
    int count = 0;

    while (curr_event != NULL) {
            count++;
            curr_event = curr_event->next;  
    }

    return count;
}