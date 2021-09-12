#include "mqtt_events.h"
#include "mqtt_mail.h"

/**
 * decimal operators: shell script operators
 * @return: 0 - success, 1 - failure
 */
static int process_dec_val(char *value, int dec_operator, char *event_value)
{
    int sender_value = atoi(value);
    int event_value_converted = atoi(event_value);
    switch(dec_operator) {
        case lt:
                if(sender_value < event_value_converted)
                        return 0;
                break;
        case gt:
                if(sender_value > event_value_converted)
                        return 0;        
                break;
        case le:
                if(sender_value <= event_value_converted)
                        return 0;        
                break;
        case ge:
                if(sender_value >= event_value_converted)
                        return 0;           
                break;
        case eq:
                if(sender_value == event_value_converted)
                        return 0;           
                break;
        case ne:
                if(sender_value != event_value_converted)
                        return 0;           
                break;
        default:
                fprintf(stderr, "decimal operator unknown\n");
    }
    return 1;
}
/**
 * string operators:
 * 0 - Equal
 * 1 - Not equal
 * @return: 0 - success, 1 - failure
 */
static int process_str_val(char *value, int dec_operator, char *event_value)
{
    switch(dec_operator) {
        case 0:
                if(strcmp(value, event_value) == 0)
                        return 0;
                break;
        case 1:
                if(strcmp(value, event_value) != 0)
                        return 0;        
                break;
        default:
                fprintf(stderr, "string operator unknown\n");
    }
    return 1;
}

/**
 * check json value type
 * https://stackoverflow.com/questions/2279379/how-to-convert-integer-to-char-in-c
 * @return: value of json object
 */
static char *get_value_from_jobj(struct json_object *val, int *decider)
{
    enum json_type type;
    char *output = (char *) malloc(sizeof(char) * 200);
    if(output == NULL)
            return NULL;
    
    type = json_object_get_type(val);
    switch (type) {
            case json_type_null:
                    output = NULL;
                    break;
            case json_type_double:
                    sprintf(output, "%f", json_object_get_double(val));
                    *decider = 1;
                    break;
            case json_type_int:
                    sprintf(output, "%d", json_object_get_int(val));
                     *decider = 1;
                    break;
            case json_type_string:
                    sprintf(output, "%s", json_object_get_string(val));
                     *decider = 2;
                    break;
            default:
                    output = NULL;
    }
    return output;
}
/**
 * check if 
 * @return: 0 - success, 1 - json problems, other - mail problems (see mqtt_mail.c)
 */
extern int process_events(char *topic, char *payload, struct topic_node *topics)
{
    struct json_object *jobj = NULL;
    char *msg_value = NULL;
    int rc = 1;
    int decider = 0; //!< tells if json value is number or string

    jobj = json_tokener_parse(payload);
    if (jobj == NULL) {
            fprintf(stderr, "event value has to be in JSON format\n");
            return 1;
    }
    while (topics != NULL) {
            if (strcmp(topic, topics->name) != 0) {
                    topics = topics->next;
                    continue;
            }
            
            json_object_object_foreach(jobj, key, val) {
                    if (key == NULL || val == NULL)
                            continue;
                    
                    msg_value = get_value_from_jobj(val, &decider);
                    if (msg_value == NULL) {
                            fprintf(stderr, "JSON value parsing failed\n");
                            return 1;
                    }

                    while (topics->head_event != NULL) {
                            rc = 1;
                            if (strcmp(topics->head_event->type, "decimal") == 0 && decider == 1) {
                                    rc = process_dec_val(msg_value, topics->head_event->dec_operator, topics->head_event->opt_value);
                            } else if (strcmp(topics->head_event->type, "string") == 0 && decider == 2) {
                                    rc = process_str_val(msg_value, topics->head_event->str_operator, topics->head_event->opt_value);
                            }

                            if (rc == 0) {
                                    send_mail(msg_value, topics->head_event->user_email, topics->head_event->username, topics->head_event->password,
                                    topics->head_event->sender_email, topics->head_event->smtp_ip, topics->head_event->smtp_port, topics->head_event->secure,
                                    topics->head_event->opt_value, topics->head_event->topic); //!< this or Linked List search function?
                            }
                                    
                            topics->head_event = topics->head_event->next;
                    }
                    free(msg_value);
            }
            topics = topics->next;
    }
    json_object_put(jobj);

    return 0;
}