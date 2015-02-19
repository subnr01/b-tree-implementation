/*
    Applies function "f" to every key in page "p"
 */
#include "def.h"

typedef void (*RecordVisitor)(struct KeyRecord *);

void scanLeaf(struct PageHdr *p, RecordVisitor f) {
    struct KeyRecord *keyptr;

    /* traverse all keys in this page */
    keyptr = p->KeyListPtr;
    while (keyptr != NULL) {
        f(keyptr);
        keyptr = keyptr->Next;
    }
}


int printLeaf(struct PageHdr *p, int initialFlag, char *key, int k) {
    struct KeyRecord *keyptr;
    keyptr = p->KeyListPtr;
    if (initialFlag) {
        while (keyptr != NULL) {
            keyptr = keyptr->Next;
            if (strcmp(key, keyptr->StoredKey) == 0) {
                keyptr = keyptr->Next;
                break;
            }
        }
    }

    int no_of_elements_removed = 0;
    while (k > 0 && keyptr != NULL) {
        printf("%s\n", keyptr->StoredKey);
        keyptr = keyptr->Next;
        k--;
        no_of_elements_removed++;
    }

    fflush(stdout);
    return no_of_elements_removed;
}