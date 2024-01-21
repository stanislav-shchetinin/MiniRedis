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

#define SWAP(type, a, b) do { \
    type temp = a; \
    a = b; \
    b = temp; \
} while (0)

static void do_set(struct command* cmd, uint8_t* out){
    struct Entry key;
    SWAP(char*, key.key, cmd->key);
    //strcpy(key.key, cmd->key);
    key.node.next = NULL;
    key.node.hcode = str_hash((uint8_t*)key.key, strlen(key.key));

    struct HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (node) {
        //strcpy(container_of(node, struct Entry, node)->val, cmd->value);
        SWAP(char*, container_of(node, struct Entry, node)->val, cmd->value);
    } else {
        struct Entry* ent = calloc(sizeof(struct Entry), 1);

        SWAP(char*, ent->key, key.key);
        ent->node.hcode = key.node.hcode;
        //strcpy(ent.val, cmd->value);
        SWAP(char*, ent->val, cmd->value);
        hm_insert(&g_data.db, &ent->node);
    }

    out_string(out, "1\n");
}

static void do_get(struct command* cmd, uint8_t* out){
    struct Entry key;
    //strcpy(key.key, cmd->key);
    SWAP(char*, key.key, cmd->key);
    key.node.next = NULL;
    key.node.hcode = str_hash((uint8_t*)key.key, strlen(key.key));
    struct HNode *node = hm_lookup(&g_data.db, &key.node, &entry_eq);
    if (!node) {
        out_string(out, "(err) Not found\n");
        return;
    }
    char* val = container_of(node, struct Entry, node)->val;
    out_string(out, val);
}

static void do_del(struct command* cmd, uint8_t* out){
    struct Entry key;
    //strcpy(key.key, cmd->key);
    SWAP(char*, key.key, cmd->key);
    key.node.hcode = str_hash((uint8_t*)key.key, strlen(key.key));

    struct HNode *node = hm_pop(&g_data.db, &key.node, &entry_eq);
    if (node) {
        free(container_of(node, struct Entry, node));
        out_string(out, "1\n");
        return;
    }
    out_string(out, "0\n");
}

static void h_scan(struct HTab *tab, uint8_t* out) {
    if (tab->size == 0) {
        return;
    }
    char msg[MAX_MSG + 1];
    size_t pos = 0;
    for (size_t i = 0; i < tab->mask + 1; ++i) {
        struct HNode *node = tab->tab[i];
        while (node) {
            char* key = container_of(node, struct Entry, node)->key;
            size_t len = strlen(key);
            strcpy(&msg[pos], key);
            strcpy(&msg[pos + len + 1], "\n");
            pos += len + 2;
            node = node->next;
        }
    }
    out_string(out, msg);
}

static void do_keys(uint8_t* out){
    h_scan(&g_data.db.ht1, out);
    h_scan(&g_data.db.ht2, out);
}

static uint8_t* do_cmd(struct command* cmd){
    uint8_t* out = malloc(MAX_MSG + HEADER_SIZE + 1);
    if (strcmp(cmd->name, "set") == 0){
        if (*cmd->key == 0 || *cmd->value == 0){
            out_string(out, "(err) Invalid format: set <key> <value>\n");
        } else {
            do_set(cmd, out);
        }

    } else if (strcmp(cmd->name, "get") == 0){
        if (*cmd->key == 0 || *cmd->value != 0){
            out_string(out, "(err) Invalid format: get <key>\n");
        } else {
            do_get(cmd, out);
        }

    } else if (strcmp(cmd->name, "del") == 0){
        if (*cmd->key == 0 || *cmd->value != 0){
            out_string(out, "(err) Invalid format: del <key> <value>\n");
        } else {
            do_del(cmd, out);
        }

    } else if (strcmp(cmd->name, "keys") == 0) {
        do_keys(out);
    } else {
        out_string(out, "(err) Invalid command\n");
    }
    return out;
}

static struct command* parse(uint8_t *text){
    struct command* cmd = calloc(sizeof (struct command), 1);
    if (!cmd){
        perror("malloc");
        return NULL;
    }
    cmd->name = calloc(sizeof(char), HEADER_SIZE + MAX_MSG);
    cmd->value = calloc(sizeof(char), HEADER_SIZE + MAX_MSG);
    cmd->key = calloc(sizeof(char), HEADER_SIZE + MAX_MSG);
    char* token = NULL;
    if ((token = strsep(&text, " "))){
        strcpy(cmd->name, token);
    }
    if ((token = strsep(&text, " "))){
        strcpy(cmd->key, token);
    }
    if ((token = strsep(&text, " "))){
        strcpy(cmd->value, token);
    }
    return cmd;
}

uint8_t* do_request(uint8_t *text) {
    struct command* cmd = parse(text);
    uint8_t* out = do_cmd(cmd);
    free(cmd);
    return out;
}