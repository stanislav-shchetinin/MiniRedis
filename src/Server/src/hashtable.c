//
// Created by stass on 09.01.2024.
//

#include "hashtable.h"

void h_init(struct HTab *htab, size_t n) {
    assert(n > 0 && ((n - 1) & n) == 0);
    htab->tab = (struct HNode **)calloc(sizeof(struct HNode *), n);
    htab->mask = n - 1;
    htab->size = 0;
}

void h_insert(struct HTab *htab, struct HNode *node) {
    size_t pos = node->hcode & htab->mask;
    struct HNode *next = htab->tab[pos];
    node->next = next;
    htab->tab[pos] = node;
    htab->size++;
}

struct HNode **h_lookup(
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

struct HNode *h_detach(struct HTab *htab, struct HNode **from) {
    struct HNode *node = *from;
    *from = (*from)->next;
    htab->size--;
    return node;
}