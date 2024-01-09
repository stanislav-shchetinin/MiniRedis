//
// Created by stass on 08.01.2024.
//
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include "network.h"
#include "request.h"
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

void out_string(uint8_t* out, char* msg){
    uint32_t n = (uint32_t)strlen(msg);
    assert(n < MAX_MSG);
    memcpy(out, &n, HEADER_SIZE);
    strcpy(&out[HEADER_SIZE], msg);
}

static void do_set(struct command* cmd, uint8_t* out){
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
            out_string(out, "malloc error\n");
            return;
        }

        ent->key = key.key;
        ent->node.hcode = key.node.hcode;
        ent->val = cmd->value;
        hm_insert(&g_data.db, &ent->node);
    }
    out_string(out, "OK\n");
}

static void do_get(struct command* cmd, uint8_t* out){
    printf("Get. Key: %s\n", cmd->key);
    fflush(stdout);
    struct Entry key;
    key.key = cmd->key;
    key.node.hcode = str_hash((uint8_t*)key.key, strlen(key.key));
    struct HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (!node) {
        out_string(out, "Not found\n");
        return;
    }
    char* val = container_of(node, struct Entry, node)->val;
    out_string(out, val);
}

static void do_del(struct command* cmd, uint8_t* out){
    struct Entry key;
    key.key = cmd->key;
    key.node.hcode = str_hash((uint8_t*)key.key, strlen(key.key));

    struct HNode *node = hm_pop(&g_data.db, &key.node, &entry_eq);
    if (node) {
        free(container_of(node, struct Entry, node));
    }
    out_string(out, "OK\n");
}

static uint8_t* do_cmd(struct command* cmd){
    uint8_t* out = malloc(MAX_MSG + HEADER_SIZE + 1);
    if (strcmp(cmd->name, "set") == 0){
        if (cmd->key == NULL || cmd->value == NULL){
            out_string(out, "Invalid format: set <key> <value>\n");
        } else {
            do_set(cmd, out);
        }

    } else if (strcmp(cmd->name, "get") == 0){
        if (cmd->key == NULL || cmd->value != NULL){
            out_string(out, "Invalid format: get <key>\n");
        } else {
            do_get(cmd, out);
        }

    } else if (strcmp(cmd->name, "del") == 0){
        if (cmd->key == NULL || cmd->value != NULL){
            out_string(out, "Invalid format: del <key> <value>\n");
        } else {
            do_del(cmd, out);
        }

    } else {
        out_string(out, "Invalid command\n");
    }
    return out;
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

uint8_t* do_request(uint8_t *text) {
    struct command* cmd = parse(text);
    uint8_t* out = do_cmd(cmd);
    free(cmd);
    return out;
}