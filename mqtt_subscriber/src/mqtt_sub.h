#ifndef MQTT_SUB_H
#define MQTT_SUB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mosquitto.h> 
#include "mqtt_config.h"


extern int connect_to_broker(struct mosquitto **mosq, struct settings *settings, struct topic_node *topics);
extern int subscribe_topics(struct mosquitto **mosq, struct topic_node *topics);
static int set_broker_settings(struct mosquitto **mosq, struct settings *settings);
static void on_message_cb(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg);
static void on_connect_cb(struct mosquitto *mosq, void *obj, int rc);

#endif