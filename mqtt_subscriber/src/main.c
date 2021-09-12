#include <signal.h>
#include "mqtt_sub.h"
#include "linked_list.h"
#include "mqtt_db.h"
#include <unistd.h>

volatile sig_atomic_t deamonize = 1;
void term_proc(int sigterm)
{
        deamonize = 0;
}

int main(void)
{
    struct mosquitto *mosq = NULL;
    struct settings *settings = NULL;
    struct topic_node *head = NULL;
    int rc = 0;

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term_proc;
    sigaction(SIGTERM, &action, NULL);

    if (iniciate_config_read(&head, &settings) != 0)
            goto cleanup_3;

    if (connect_to_broker(&mosq, settings, head) != 0)
            goto cleanup_2;

    if (subscribe_topics(&mosq, head) != 0)
            goto cleanup_1;

    if (open_db() != 0)
            goto cleanup_2;
    
    while (deamonize) {
        if (mosquitto_loop(mosq, -1, 1) != MOSQ_ERR_SUCCESS) {
                fprintf(stderr, "Communications between the client and broker stopped\n");
                goto cleanup_2;
        }
    }

    cleanup_1:
            mosquitto_disconnect(mosq);
    cleanup_2:
            close_db();
            mosquitto_destroy(mosq);
            mosquitto_lib_cleanup();
    cleanup_3:
            if (head != NULL)
                    delete_list(head);
            if (settings != NULL)
                    free(settings);
            return 0;
}