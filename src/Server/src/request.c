//
// Created by stass on 08.01.2024.
//
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include "request.h"

struct command {
    char* name;
    char* key;
    char* value;
};

static void do_set(struct command* cmd){
    printf("Set. Key: %s, Value: %s\n", cmd->key, cmd->value);
    fflush(stdout);
}

static void do_get(struct command* cmd){
    printf("Get. Key: %s\n", cmd->key);
    fflush(stdout);
}

static void do_del(struct command* cmd){
    printf("Del. Key: %s\n", cmd->key);
    fflush(stdout);
}

static bool do_cmd(struct command* cmd){
    if (strcmp(cmd->name, "set") == 0){
        if (cmd->key == NULL || cmd->value == NULL){
            fprintf(stderr, "Invalid format: set <key> <value>\n");
            return false;
        }
        do_set(cmd);

    } else if (strcmp(cmd->name, "get") == 0){
        if (cmd->key == NULL || cmd->value != NULL){
            fprintf(stderr, "Invalid format: get <key>\n");
            return false;
        }
        do_get(cmd);

    } else if (strcmp(cmd->name, "del") == 0){
        if (cmd->key == NULL || cmd->value != NULL){
            fprintf(stderr, "Invalid format: del <key> <value>\n");
            return false;
        }
        do_del(cmd);

    } else {
        fprintf(stderr, "Invalid command\n");
        return false;
    }
    return true;
}

static struct command* parse(uint8_t *text){
    struct command* cmd = malloc(sizeof(struct command));
    if (!cmd){
        perror("malloc");
        return NULL;
    }
    cmd->name = cmd->value = cmd->key = NULL;
    char* token = NULL;
    if ((token = strsep(&text, " "))){
        cmd->name = token;
    }
    if ((token = strsep(&text, " "))){
        cmd->key = token;
    }
    if ((token = strsep(&text, " "))){
        cmd->value = token;
    }
    return cmd;
}

void do_request(uint8_t *text) {
    struct command* cmd = parse(text);
    do_cmd(cmd);
    free(cmd);
}