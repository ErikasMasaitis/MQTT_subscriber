#ifndef MQTT_MAIL_H
#define MQTT_MAIL_H

#include <curl/curl.h>
#include <string.h>
#include "mqtt_config.h"

struct upload_status {
    const char *readptr;
    size_t sizeleft;
};

extern int send_mail(char *value, char *user_email, char *username, char *password, char *sender_email,
char *smtp_ip, char *smtp_port, int secure, char *opt_value, char *topic);
static char *create_template(char *value, char *user_mail, char *sender_email, char *topic, char *opt_value);

#endif