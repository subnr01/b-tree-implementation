/*
    Applies function "f" to every key in page "p"
 */
#include "def.h"

typedef void (*RecordVisitor)(struct KeyRecord *);

int CompareKeys(char *Key, char *Word);

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
    struct KeyRecord *keyptr = (struct KeyRecord *) malloc(sizeof(struct KeyRecord *));
    if (p != NULL) {
        keyptr = p->KeyListPtr;
    }
    int no_of_elements_removed = 0;
    while (k > 0 && keyptr != NULL) {
        if(CompareKeys(keyptr->StoredKey, key) == 2) {
            printf("%s\n", keyptr->StoredKey);
            k--;
            no_of_elements_removed++;
        }
        keyptr = keyptr->Next;
    }

    fflush(stdout);
    return no_of_elements_removed;
}