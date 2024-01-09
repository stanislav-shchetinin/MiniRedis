//
// Created by stass on 08.01.2024.
//
#include <malloc.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include "request.h"

struct HNode {
    struct HNode *next;
    uint64_t hcode;
};

struct HTab {
    struct HNode **tab;
    size_t mask;
    size_t size;
};

static void h_init(struct HTab *htab, size_t n) {
    assert(n > 0 && ((n - 1) & n) == 0);
    htab->tab = (struct HNode **)calloc(sizeof(struct HNode *), n);
    htab->mask = n - 1;
    htab->size = 0;
}

static void h_insert(struct HTab *htab, struct HNode *node) {
    size_t pos = node->hcode & htab->mask;
    struct HNode *next = htab->tab[pos];
    node->next = next;
    htab->tab[pos] = node;
    htab->size++;
}

static struct HNode **h_lookup(
        struct HTab *htab, struct HNode *key, bool (*cmp)(struct HNode *, struct HNode *))
{
    if (!htab->tab) {
        return NULL;
    }

    size_t pos = key->hcode & htab->mask;
    struct HNode **from = &htab->tab[pos];
    while (*from) {
        if (cmp(*from, key)) {
            return from;
        }
        from = &(*from)->next;
    }
    return NULL;
}

static struct HNode *h_detach(struct HTab *htab, struct HNode **from) {
    struct HNode *node = *from;
    *from = (*from)->next;
    htab->size--;
    return node;
}

struct HMap {
    struct HTab ht1;
    struct HTab ht2;
    size_t resizing_pos;
};

const size_t k_resizing_work = 128;

static void hm_help_resizing(struct HMap *hmap) {
    if (hmap->ht2.tab == NULL) {
        return;
    }

    size_t nwork = 0;
    while (nwork < k_resizing_work && hmap->ht2.size > 0) {
        // scan for nodes from ht2 and move them to ht1
        struct HNode **from = &hmap->ht2.tab[hmap->resizing_pos];
        if (!*from) {
            hmap->resizing_pos++;
            continue;
        }

        h_insert(&hmap->ht1, h_detach(&hmap->ht2, from));
        nwork++;
    }

    if (hmap->ht2.size == 0) {
        // done
        free(hmap->ht2.tab);
        hmap->ht2.tab = NULL;
        hmap->ht2.size = 0;
        hmap->ht2.mask = 0;
    }
}

struct HNode *hm_lookup(
        struct HMap *hmap, struct HNode *key, bool (*cmp)(struct HNode *, struct HNode *))
{
    hm_help_resizing(hmap);
    struct HNode **from = h_lookup(&hmap->ht1, key, cmp);
    if (!from) {
        from = h_lookup(&hmap->ht2, key, cmp);
    }
    return from ? *from : NULL;
}

static void hm_start_resizing(struct HMap *hmap) {
    assert(hmap->ht2.tab == NULL);
    // create a bigger hashtable and swap them
    hmap->ht2 = hmap->ht1;
    h_init(&hmap->ht1, (hmap->ht1.mask + 1) * 2);
    hmap->resizing_pos = 0;
}

const size_t k_max_load_factor = 8;

void hm_insert(struct HMap *hmap, struct HNode *node) {
    if (!hmap->ht1.tab) {
        h_init(&hmap->ht1, 4);
    }
    h_insert(&hmap->ht1, node);

    if (!hmap->ht2.tab) {
        // check whether we need to resize
        size_t load_factor = hmap->ht1.size / (hmap->ht1.mask + 1);
        if (load_factor >= k_max_load_factor) {
            hm_start_resizing(hmap);
        }
    }
    hm_help_resizing(hmap);
}

struct HNode *hm_pop(
        struct HMap *hmap, struct HNode *key, bool (*cmp)(struct HNode *, struct HNode *))
{
    hm_help_resizing(hmap);
    struct HNode **from = h_lookup(&hmap->ht1, key, cmp);
    if (from) {
        return h_detach(&hmap->ht1, from);
    }
    from = h_lookup(&hmap->ht2, key, cmp);
    if (from) {
        return h_detach(&hmap->ht2, from);
    }
    return NULL;
}

struct Entry {
    struct HNode node;
    char* key;
    char* val;
};

static struct {
    struct HMap db;
} g_data;

#define container_of(ptr, type, member) ({                  \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type, member) );})

static bool entry_eq(struct HNode *lhs, struct HNode *rhs) {
    struct Entry *le = container_of(lhs, struct Entry, node);
    struct Entry *re = container_of(rhs, struct Entry, node);
    return lhs->hcode == rhs->hcode && le->key == re->key;  //!!!!!!!!!!!!!!!!!!!
}

struct command {
    char* name;
    char* key;
    char* value;
};

static uint64_t str_hash(const uint8_t *data, size_t len) {
    uint32_t h = 0x811C9DC5;
    for (size_t i = 0; i < len; i++) {
        h = (h + data[i]) * 0x01000193;
    }
    return h;
}


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