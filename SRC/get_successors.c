#include "def.h"

extern int check_word(char *word);

extern int strtolow(char *s);

extern struct PageHdr *treesearch_pageHdr(PAGENO PageNo, char *key);

extern POSTINGSPTR searchLeaf(struct PageHdr *PagePtr, char *key, int prinFlag);

extern struct PageHdr *FetchPage(PAGENO Page);

extern int FreePage(struct PageHdr *PagePtr);

extern int printLeaf(struct PageHdr *p, int initialLeaf, char *key, int k);

int get_successors(char *key, int k, char *result[]) {

    /* Print an error message if strlen(key) > MAXWORDSIZE */

    if (k < 0) {
        printf("k should be positive not %d", k);
        return -1;
    }

    if (strlen(key) > MAXWORDSIZE) {
        printf("ERROR in \"get_successors\":  Length of Word Exceeds Maximum Allowed\n");
        printf(" and word May Be Truncated\n");
        return -1;
    }

    if (check_word(key) == FALSE) {
        return -1;
    }

    /* turn to lower case, for uniformity */
    strtolow(key);

    struct PageHdr *PagePtr = treesearch_pageHdr(ROOT, key);
    POSTINGSPTR resultPage = searchLeaf(PagePtr, key, 0);
    if (resultPage == NONEXISTENT) {
        printf("key \"%s\": not found\n", key);
        return NONEXISTENT;
    }

    int total = k;
    int retrieved = printLeaf(PagePtr, 1, key, total);
    total -= retrieved;
//    printf("\ninitial ret:%d", retrieved);
// todo printing found 26 successors and then the keys!
    while (total > 0 && (PagePtr->PgNumOfNxtLfPg) != NULLPAGENO) {
        struct PageHdr *temp = PagePtr;
        PagePtr = FetchPage(temp->PgNumOfNxtLfPg);
        FreePage(temp);
        retrieved = printLeaf(PagePtr, 0, key, total);
        total -= retrieved;
        //printf("\ninside ret:%d", retrieved);

    }

    return 0;
}
