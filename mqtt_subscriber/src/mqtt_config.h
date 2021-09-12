#ifndef MQTT_CONFIG_H
#define MQTT_CONFIG_H

#include <string.h>
#include <stdlib.h>
#include <uci.h>

#define CONFIG_FILE "/etc/config/mqtt_subs"

struct event_node
{
    char topic[200];
    char type[10];
    char opt_value[200];
    int dec_operator;
    int str_operator;
    char user_email[150];
    char smtp_ip[30];
    char smtp_port[10];
    int credentials;
    int secure;
    char username[70];
    char password[70];
    char sender_email[200]; 
    struct event_node *next;   
};

struct topic_node 
{
    char name[200];
    int qos;
    struct event_node *head_event;
    struct topic_node *next;
};

struct settings 
{
        char remote_address[50];
        char remote_port[30];
        char username[50];
        char password[50];
        char psk[200];
        char identity[200];
        int ssl_enabled;
        char tls_type[10];
        char cert_file[200];
        char key_file[200];
        char ca_cert[200];
};

extern int iniciate_config_read(struct topic_node **alltopics, struct settings **settings);
static int save_values(struct settings **settings, struct topic_node **alltopics, struct uci_context *ctx, struct uci_section *s);
static int load_config(struct uci_package **p, struct uci_context **ctx, char *config_name);
static int set_settings(struct uci_context *ctx, struct uci_section *s, struct settings **settings);
static int set_topics(struct uci_context *ctx, struct uci_section *s, struct topic_node **topics);
static int set_events(struct uci_context *ctx, struct uci_section *s, struct topic_node **topics);
static int get_topics(struct uci_context *ctx, struct uci_package *p, struct topic_node **alltopics);
static int set_sender_settings(struct event_node *temp_event, char *sender_name);

#endif