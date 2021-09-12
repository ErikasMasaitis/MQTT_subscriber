#include "mqtt_sub.h"
#include "mqtt_db.h"
#include "mqtt_events.h"

/**
 * set connection settings and connect to broker
 * @return: 0 - success, 1 - allocation problems, 2 - settings failed to apply, 3 - could'nt connect to broker
 */
extern int connect_to_broker(struct mosquitto **mosq, struct settings *settings, struct topic_node *topics)
{
    int rc;
    mosquitto_lib_init();
    *mosq = mosquitto_new(NULL, true, topics);

    if (*mosq == NULL) {
            fprintf(stderr, "Can't create mosquitto broker\n");
            return 1;
    }

    if (set_broker_settings(mosq, settings) != 0) {
            fprintf(stderr, "Could not set broker settings\n");
            return 2;
    }
    
    rc = mosquitto_connect(*mosq, settings->remote_address, atoi(settings->remote_port), 60);

    if (rc != 0) {
            fprintf(stdout, "Could not connect to broker, return code: %d \n", rc);
            return 3;
    }

    return 0;
}

/**
 * subscribe to given topics
 * @return: 0 - success, 1 - can't subscribe, terminate.
 */
extern int subscribe_topics(struct mosquitto **mosq, struct topic_node *topics)
{
    while(topics != NULL) {
            if (mosquitto_subscribe(*mosq, NULL, topics->name, topics->qos) != MOSQ_ERR_SUCCESS) {
                    fprintf(stderr, "Subscribing to topic '%s' failed\n", topics->name);
                    return 1;
            }
            topics = topics->next;
    }

    return 0;
}

/**
 * set broker settings
 * @return: 0 - success, 1 - can't set credentials or ssl
 */
static int set_broker_settings(struct mosquitto **mosq, struct settings *settings)
{
    int rc = 0;

    mosquitto_connect_callback_set(*mosq, on_connect_cb);
    mosquitto_message_callback_set(*mosq, on_message_cb);

    if (strlen(settings->username) != 0 || strlen(settings->password) != 0)
            rc = mosquitto_username_pw_set(*mosq, settings->username, settings->password);

    if (rc != MOSQ_ERR_SUCCESS)
                return 1;

    if (settings->ssl_enabled = 1 && strcmp(settings->tls_type, "cert") == 0) {
            if (strlen(settings->ca_cert) != 0 && strlen(settings->key_file) != 0 && strlen(settings->cert_file) != 0) {
                    rc = mosquitto_tls_set(*mosq, settings->ca_cert, NULL, settings->cert_file, settings->key_file, NULL);
            }
    }
    
    if (settings->ssl_enabled = 1 && strcmp(settings->tls_type, "psk") == 0) {
            if (strlen(settings->psk) != 0 && strlen(settings->identity) != 0) {
                    rc = mosquitto_tls_psk_set(*mosq, settings->psk, settings->identity, NULL);
            }
    }

    if (rc != MOSQ_ERR_SUCCESS)
            return 1;

    return 0;
}

static void on_connect_cb(struct mosquitto *mosq, void *obj, int rc) 
{
    if(rc) {
            fprintf(stderr, "Error with result code: %d\n", rc);
            exit(1);
    }
}

static void on_message_cb(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg)
{
    struct topic_node *topics = (struct topic_node *)obj;
    
    if (msg->payloadlen == 0) {
            fprintf(stderr, "message can't be null\n");
            return ;
    }
    
    if (add_message_to_db(msg->topic, msg->payload) != 0) {
            fprintf(stderr, "adding message to database failed\n");
            return ;
    }
    if (process_events(msg->topic, msg->payload, topics) != 0) {
            fprintf(stderr, "message was not sent to email!\n");
            return ;
    }
    
    
}