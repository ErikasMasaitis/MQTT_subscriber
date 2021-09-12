#include "mqtt_mail.h"


static size_t payload_source(void *ptr, size_t size, size_t nmemb, void *userp)
{
  struct upload_status *upload_ctx = (struct upload_status *)userp;
  const char *data;

  size_t bytes_to_copy = upload_ctx->sizeleft < size*nmemb ? upload_ctx->sizeleft : size*nmemb;
  bytes_to_copy = upload_ctx->sizeleft < 10 ? upload_ctx->sizeleft : 10;

  if(size*nmemb < 1)
        return 0;

  if(upload_ctx->sizeleft) {
        memcpy((char*)ptr, (char*)upload_ctx->readptr, bytes_to_copy);
        upload_ctx->readptr+=bytes_to_copy;
        upload_ctx->sizeleft-=bytes_to_copy;
        return bytes_to_copy;
  }

  return 0;
}
/**
 * generate message template
 * @return: NULL if failed, filled template if okay
 */
static char *create_template(char *value, char *user_mail, char *sender_email, char *topic, char *opt_value)
{
    char payload_template[] =
    "To: %s\r\n"
    "From: %s\r\n"
    "Subject: MQTT Subscriber\r\n"
    "\r\n"
    "An event from topic '%s' was observed. Event value: %s, publisher value: %s."
    "\r\n\r\n";

    size_t payload_text_len = strlen(payload_template) +
                    strlen(user_mail) +
                    strlen(sender_email) +
                    strlen(topic) +
                    strlen(opt_value) + strlen(value)+1;

    char* payload_text = malloc(payload_text_len);

    if (payload_text == NULL)
            return payload_text;

    sprintf(payload_text, payload_template, user_mail, sender_email, topic, opt_value, value);

    return payload_text;
}
/**
 * set options and send message to mail
 * @return: 0 - success, 2 - allocation problems, 3 - sending letter failed
 */
extern int send_mail(char *value, char *user_email, char *username, char *password, char *sender_email,
 char *smtp_ip, char *smtp_port, int secure, char *opt_value, char *topic)
{
    CURL *curl;
    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    struct upload_status upload_ctx = { 0 };
    char *smtp_server_url;
    
    char *payload_text = create_template(value, user_email, sender_email, topic, opt_value);
    if (payload_text == NULL) {
            fprintf(stderr, "creating letter template failed\n");
            return 2;
    }
    
    smtp_server_url = (char *) malloc(sizeof(char) * (strlen("smtp://") + strlen(smtp_ip) + strlen(smtp_port))+5);
    if (smtp_server_url == NULL)
            return 2;
    sprintf(smtp_server_url, "smtp://%s:%s", smtp_ip, smtp_port);
    curl = curl_easy_init();
    if(curl) {
            upload_ctx.readptr = payload_text;
            upload_ctx.sizeleft = (long)strlen(payload_text);

            recipients = curl_slist_append(recipients, user_email);

            curl_easy_setopt(curl, CURLOPT_USERNAME, username);
            curl_easy_setopt(curl, CURLOPT_PASSWORD, password);
            curl_easy_setopt(curl, CURLOPT_URL, smtp_server_url);

            if(secure == 1) {
                    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
                    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
                    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);                    
            }

            curl_easy_setopt(curl, CURLOPT_MAIL_FROM, sender_email);
            curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
            curl_easy_setopt(curl, CURLOPT_READFUNCTION, payload_source);

            curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
            curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    
            res = curl_easy_perform(curl);

            if(res != CURLE_OK) {
                    fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                    return 3;
            }
            curl_global_cleanup();
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);
    }
    free(payload_text);
    free(smtp_server_url);
    return 0;
}