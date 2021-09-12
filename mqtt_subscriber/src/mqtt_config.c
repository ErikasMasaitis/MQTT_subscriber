#include "mqtt_config.h"
#include "linked_list.h"

/**
 * loop through uci sections
 * @return: 0 - success, other - something wrong
 */
extern int iniciate_config_read(struct topic_node **alltopics, struct settings **settings)
{
    struct uci_package *p = NULL;
    struct uci_element *i = NULL;
    struct uci_context *ctx = NULL;
    int tc = 0;

    if (load_config(&p, &ctx, CONFIG_FILE) != 0)
            return 1;

    if (get_topics(ctx, p, alltopics) != 0) //!< firstly we get topics, because uci sections can be in random order
            return 1;

    tc = count_topics(*alltopics); 
    if (tc == 0) {
            uci_free_context(ctx);
            return 1;
    }

    uci_foreach_element(&p->sections, i) {//!< then we can get events of topics and broker/subscriber settings
            struct uci_section *section = uci_to_section(i);
            if (save_values(settings, alltopics, ctx, section) != 0) 
                    return 1;
    }
    
    uci_free_context(ctx);
    return 0;
}
/**
 * set topics
 * @return: 0 - success, 1 - allocation problems
 */
static int get_topics(struct uci_context *ctx, struct uci_package *p, struct topic_node **alltopics)
{
    struct uci_element *i = NULL;
    uci_foreach_element(&p->sections, i) {
            struct uci_section *s = uci_to_section(i);
            if (strcmp(s->type, "mqtt_topic") == 0)
                    if (set_topics(ctx, s, alltopics) != 0)
                            return 1;
       
    } 
    return 0;
}
/**
 * check uci section type and set neccesary options
 * @return: 0 - success, other - something wrong
 */
static int save_values(struct settings **settings, struct topic_node **alltopics, struct uci_context *ctx, struct uci_section *s)
{
    int rc = 0;
    if (strcmp(s->type, "mqtt_sub") == 0)
            rc = set_settings(ctx, s, settings);

    if (strcmp(s->type, "mqtt_event") == 0)
            rc = set_events(ctx, s, alltopics);

    return rc;
}
/**
 * set MQTT broker connection settings
 * remote address and port are mandatory
 * other settings are optional
 * @return: 0 - success, 1 - allocation problems, 2 - can't find remote address or remote port
 */
static int set_settings(struct uci_context *ctx, struct uci_section *s, struct settings **settings)
{
    *settings = (struct settings *) calloc(1, sizeof(struct settings));
    if (*settings == NULL) {
            fprintf(stderr, "settings structure allocation failed\n");
            return 1;
    }

    const char *remote_address = uci_lookup_option_string(ctx, s, "remote_address");
    const char *remote_port = uci_lookup_option_string(ctx, s, "remote_port");
    const char *username = uci_lookup_option_string(ctx, s, "remote_username");
    const char *password = uci_lookup_option_string(ctx, s, "password");    
    const char *tls_enabled = uci_lookup_option_string(ctx, s, "tls");
    const char *tls_type = uci_lookup_option_string(ctx, s, "tls_type");
    const char *cafile = uci_lookup_option_string(ctx, s, "cafile");
    const char *certfile = uci_lookup_option_string(ctx, s, "certfile");
    const char *keyfile = uci_lookup_option_string(ctx, s, "keyfile");  
    const char *psk = uci_lookup_option_string(ctx, s, "psk");
    const char *identity = uci_lookup_option_string(ctx, s, "identity");  

    if (remote_address == NULL || remote_port == NULL) {
            fprintf(stderr, "can't find remote address or remote port");
            return 2;
    }
    
    strcpy((*settings)->remote_address, remote_address);
    strcpy((*settings)->remote_port, remote_port);

    if (username != NULL && password != NULL) {
            strcpy((*settings)->username, username);
            strcpy((*settings)->password, password);
    }
    
    if (tls_enabled == NULL || atoi(tls_enabled) != 1 || tls_type == NULL)
            return 0;

    (*settings)->ssl_enabled = atoi(tls_enabled);

    if (strcmp(tls_type, "cert") == 0) {
            if (cafile != NULL && certfile != NULL && keyfile != NULL) {
                    strcpy((*settings)->ca_cert, cafile);
                    strcpy((*settings)->cert_file, certfile);
                    strcpy((*settings)->key_file, keyfile);
            }
    }

    if (strcmp(tls_type, "psk") == 0) {
            if (psk != NULL && identity != NULL) {
                    strcpy((*settings)->psk, psk);
                    strcpy((*settings)->identity, identity);
            }
    }
    return 0;
}
/**
 * check uci section type and set neccesary options
 * @return: 0 - success, 1 - problems with setting sender settings
 */
static int set_events(struct uci_context *ctx, struct uci_section *s, struct topic_node **topics)
{
    struct uci_element *j;
    struct event_node temporary;
    struct event_node *temp_event = NULL;
    struct topic_node *point_to_first = NULL; //!< temporary node, helps us always start loop from first element
    int rc = 0; //!< return code;
    char sender_name[80];
    
    uci_foreach_element(&s->options, j) {
            struct uci_option *option = uci_to_option(j);
            
            if (strcmp(option->e.name, "topic") == 0) {
                    strcpy(temporary.topic, option->v.string);
            } else if (strcmp(option->e.name, "type") == 0) {
                    strcpy(temporary.type, option->v.string);
            } else if (strcmp(option->e.name, "value") == 0) {
                    strcpy(temporary.opt_value, option->v.string);
            } else if (strcmp(option->e.name, "dec_operator") == 0) {
                    temporary.dec_operator = atoi(option->v.string);
            } else if (strcmp(option->e.name, "str_operator") == 0) {
                    temporary.str_operator = atoi(option->v.string);
            } else if (strcmp(option->e.name, "user_email") == 0) {
                    strcpy(temporary.user_email, option->v.string);
            } else if (strcmp(option->e.name, "sender") == 0) {
                    strcpy(sender_name, option->v.string);
                    rc = set_sender_settings(&temporary, sender_name);
            }
    }

    if (rc != 0)
            return 1;

    point_to_first = *topics;
    while(point_to_first != NULL)
    {
            if (strcmp(point_to_first->name, temporary.topic) == 0) {
                    temp_event = create_event(&temporary);
                    if (temp_event != NULL)
                            push_event(&point_to_first->head_event, temp_event);
            }
            point_to_first = point_to_first->next;
    }
}
/**
 * set mail sender options
 * @return: 0 - success, 1 - problems with config loading
 */
static int set_sender_settings(struct event_node *temp_event, char *sender_name)
{
    struct uci_package *p = NULL;
    struct uci_element *i, *j;
    struct uci_context *ctx = NULL;

    if (load_config(&p, &ctx, "/etc/config/user_groups") != 0)
            return 1;

    uci_foreach_element(&p->sections, i) {
            struct uci_section *s = uci_to_section(i);

            uci_foreach_element(&s->options, j) {
                    struct uci_option *option = uci_to_option(j);

                    if (strcmp(option->e.name, "name") == 0) {
                            if (strcmp(option->v.string, sender_name) != 0) //!< checking if this uci section is the one selected in event menu 
                                    break;
                    }

                    if (strcmp(option->e.name, "smtp_ip") == 0) {
                            strcpy(temp_event->smtp_ip, option->v.string);
                    } else if (strcmp(option->e.name, "smtp_port") == 0) {
                            strcpy(temp_event->smtp_port, option->v.string);
                    } else if (strcmp(option->e.name, "credentials") == 0) {
                            temp_event->credentials = atoi(option->v.string);
                    } else if (strcmp(option->e.name, "secure_conn") == 0) {
                            temp_event->secure = atoi(option->v.string);
                    } else if (strcmp(option->e.name, "username") == 0) {
                            strcpy(temp_event->username, option->v.string);
                    } else if (strcmp(option->e.name, "password") == 0) {
                            strcpy(temp_event->password, option->v.string);
                    } else if (strcmp(option->e.name, "senderemail") == 0) {
                            strcpy(temp_event->sender_email, option->v.string);
                    }
            }
    }

    uci_free_context(ctx);
    return 0;
}
/**
 *  push user defined topics to linked list. 
 *  if adding elements to linked list fails dont stop the program.
 *  @return: 0 - success, 1 - allocation problems
 */
static int set_topics(struct uci_context *ctx, struct uci_section *s, struct topic_node **topics)
{
    struct uci_element *j;
    struct topic_node *temp_topic;
    char name[200];
    int qos;

    uci_foreach_element(&s->options, j) {
            struct uci_option *option = uci_to_option(j);
            if (strcmp(option->e.name, "topic") == 0) {
                strcpy(name, option->v.string);
            }  else if (strcmp(option->e.name, "qos") == 0) {
                qos = atoi(option->v.string);
            }
    }
    temp_topic = create_topic(name, qos);
    if (temp_topic == NULL)
            return 1;
    
    push_topic(topics, temp_topic);

    return 0;
}

/**
 * load config from file
 * @return: 0 - success, 1 - allocation problems, 2 - uci load failed (probably bad config name)
 */
static int load_config(struct uci_package **p, struct uci_context **ctx, char *config_name)
{
    *ctx = uci_alloc_context();
    if (*ctx == NULL) {
            fprintf(stderr, "uci context memory allocation failed");
            return 1;
    }
    		
    if (UCI_OK != uci_load(*ctx, config_name, p)) {
            fprintf(stderr, "uci load failed");
            uci_free_context(*ctx);
            return 2;
    }

    return 0;
}