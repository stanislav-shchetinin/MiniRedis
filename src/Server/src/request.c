//
// Created by stass on 08.01.2024.
//
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include "request.h"
#include "hashtable.h"
#include "hashmap.h"
#include "util.h"

static struct {
    struct HMap db;
} g_data;

struct command {
    char* name;
    char* key;
    char* value;
};

static void do_set(struct command* cmd){
    printf("Set. Key: %s, Value: %s\n", cmd->key, cmd->value);
    fflush(stdout);

    struct Entry key;
    key.key = cmd->key;
    key.node.hcode = str_hash((uint8_t*)key.key, strlen(key.key));

    struct HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (node) {
        container_of(node, struct Entry, node)->val = cmd->value;
    } else {
        struct Entry* ent = malloc(sizeof(struct Entry));

        if (!ent){
            perror("malloc");
            return;
        }

        ent->key = key.key;
        ent->node.hcode = key.node.hcode;
        ent->val = cmd->value;
        hm_insert(&g_data.db, &ent->node);
    }
}

static void do_get(struct command* cmd){
    printf("Get. Key: %s\n", cmd->key);
    fflush(stdout);
    struct Entry key;
    key.key = cmd->key;
    key.node.hcode = str_hash((uint8_t*)key.key, strlen(key.key));
    struct HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (!node) {
        printf("Not found\n");
        fflush(stdout);
        return;
    }
    char* val = container_of(node, struct Entry, node)->val;
    printf("Val: %s\n", val);
}

static void do_del(struct command* cmd){
    printf("Del. Key: %s\n", cmd->key);
    fflush(stdout);

    struct Entry key;
    key.key = cmd->key;
    key.node.hcode = str_hash((uint8_t*)key.key, strlen(key.key));

    struct HNode *node = hm_pop(&g_data.db, &key.node, &entry_eq);
    if (node) {
        free(container_of(node, struct Entry, node));
    }

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