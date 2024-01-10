#include "hashmap.h"

void hm_help_resizing(struct HMap *hmap) {
    if (hmap->ht2.tab == NULL) {
        return;
    }

    size_t nwork = 0;
    while (nwork < RESIZING_WORK && hmap->ht2.size > 0) {
        struct HNode **from = &hmap->ht2.tab[hmap->resizing_pos];
        if (!*from) {
            hmap->resizing_pos++;
            continue;
        }

        h_insert(&hmap->ht1, h_detach(&hmap->ht2, from));
        nwork++;
    }

    if (hmap->ht2.size == 0) {
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

void hm_start_resizing(struct HMap *hmap) {
    assert(hmap->ht2.tab == NULL);
    hmap->ht2 = hmap->ht1;
    h_init(&hmap->ht1, (hmap->ht1.mask + 1) * 2);
    hmap->resizing_pos = 0;
}

void hm_insert(struct HMap *hmap, struct HNode *node) {
    if (!hmap->ht1.tab) {
        h_init(&hmap->ht1, 4);
    }
    h_insert(&hmap->ht1, node);

    if (!hmap->ht2.tab) {
        size_t load_factor = hmap->ht1.size / (hmap->ht1.mask + 1);
        if (load_factor >= MAX_LOAD) {
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

bool entry_eq(struct HNode *lhs, struct HNode *rhs) {
    struct Entry *le = container_of(lhs, struct Entry, node);
    struct Entry *re = container_of(rhs, struct Entry, node);
    return lhs->hcode == rhs->hcode && le->key == re->key;
}