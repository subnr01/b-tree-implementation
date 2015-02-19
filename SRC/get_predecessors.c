#include <stdio.h>
#include <string.h>
#include "def.h"

extern void pritnLL(struct node *pNode);

extern int iscommon(char *word);

extern FILE *fpbtree;

extern int check_word(char *word);

extern int strtolow(char *s);

extern int getpostings(POSTINGSPTR pptr);

extern PAGENO treesearch_page_buildLL(int PageNo, char *key, struct node *root);

struct node *head = NULL;

void search_build_list(char *key, int k, int flag);

void clearHeadPtr();

void build_pred_list(char *key, int k, FILE *pFILE);

int get_predecessors(char *key, int k, char *result[]) {
    search_build_list(key, k, FALSE);

    return 0;
}


void search_build_list(char *key, int k, int flag) {
    POSTINGSPTR pptr;

    /* Print an error message if strlen(key) > MAXWORDSIZE */
    if (strlen(key) > MAXWORDSIZE) {
        printf("ERROR in \"search\":  Length of key Exceeds Maximum Allowed\n");
        printf(" and key May Be Truncated\n");
    }
    if (iscommon(key)) {
        printf("\"%s\" is a common word - no searching is done\n", key);
        return;
    }
    if (check_word(key) == FALSE) {
        return;
    }
    /* turn to lower case, for uniformity */
    strtolow(key);
    //typedef struct node item;
    struct node *root = NULL;
    pptr = treesearch_page_buildLL(ROOT, key, root);
    if (pptr == NONEXISTENT) {
        printf("key \"%s\": not found\n", key);
    } else {
        if (flag) {
            getpostings(pptr);
        } else {
//            pritnLL(head);
            build_pred_list(key, k, fpbtree);
            printf("Found the key!\n");
        }
        clearHeadPtr();
    }
}

void build_pred_list(char *key, int k, FILE *fpb) {
    POSTINGSPTR PostOffset;
    NUMKEYS NumKeys;
    KEYLEN KeyLen;
    NUMBYTES NumBytes;
    int j;
    char Ch;
    PAGENO PtrToNxtLfPg, PtrToFinalRtgPg, PgNum;
    PAGENO i = head->value;
    printf("printing i:%ld\n", i);
    fseek(fpb, (long) (i - 1) * PAGESIZE, 0);
    fread(&Ch, sizeof(Ch), 1, fpb);
    if (feof(fpb) != 0)
        exit(0);
    printf("Leafstatus:%c\n", Ch);
    fread(&PgNum, sizeof(PgNum), 1, fpb);
    printf("Page#:%d\n", (int) PgNum);
    if (Ch == LeafSymbol) {
        fread(&PtrToNxtLfPg, sizeof(PtrToNxtLfPg), 1, fpb);
      //  printf("PtrtoNextLeafPage:%d\n", (int) PtrToNxtLfPg);
    }
    fread(&NumBytes, sizeof(NumBytes), 1, fpb);
    //printf("Number of bytes occcupied:%d\n", NumBytes);
    fread(&NumKeys, sizeof(NumKeys), 1, fpb);
    printf("Number of keys:%d\n", NumKeys);
    if (Ch == NonLeafSymbol) {
        fread(&PtrToFinalRtgPg, sizeof(PtrToFinalRtgPg), 1, fpb);
        printf("PtrToFinalRtgPg%d\n", (int) PtrToFinalRtgPg);
    }
    for (j = 0; j < NumKeys; j++) {
        if (Ch == NonLeafSymbol) {
            fread(&PgNum, sizeof(PgNum), 1, fpbtree);
            printf("NextPage:%d, ", (int) PgNum);
            fread(&KeyLen, sizeof(KeyLen), 1, fpbtree);
            fread(key, sizeof(char), KeyLen, fpbtree);
            (*(key + KeyLen)) = '\0';
            printf("key:%s, ", key);
            printf("length:%d\n", KeyLen);
        }
        if (Ch == LeafSymbol) {
            fread(&KeyLen, sizeof(KeyLen), 1, fpbtree);
            printf("Keylength:%d, ", KeyLen);
            fread(key, sizeof(char), KeyLen, fpbtree);
            (*(key + KeyLen)) = '\0';
            printf("key:%s, ", key);
            fread(&PostOffset, sizeof(PostOffset), 1, fpbtree);
            printf("Postings offset:%d\n", (int) PostOffset);
        }
    }
    printf("\n");

}

void clearHeadPtr() {
    head = NULL;

}

//todo NULL head ptr after finishing
