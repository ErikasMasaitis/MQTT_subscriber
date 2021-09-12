#ifndef MQTT_DB_H
#define MQTT_DB_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sqlite3.h>

#define DB_FILE "etc/sub_messages.db"
#define DB_TABLE "messages"

static sqlite3 *db;

static int create_table_if_not_exist();
static int create_table();
extern int open_db();
static char *time_stamp();
extern int add_message_to_db(char *topic, char *payload);

#endif