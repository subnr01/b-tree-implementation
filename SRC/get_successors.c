#include "def.h"

extern int check_word(char *word);

extern void printPage(PAGENO i, FILE *fpb);

extern int strtolow(char *s);

extern int CompareKeys(char *Key, char *Word);

extern POSTINGSPTR searchLeaf(struct PageHdr *PagePtr, char *key, int prinFlag);

extern struct PageHdr *FetchPage(PAGENO Page);

extern int FreePage(struct PageHdr *PagePtr);

extern int printLeaf(struct PageHdr *p, char *key, int k);

extern PAGENO treesearch_page(PAGENO PageNo, char *key);

extern char **create_string_array(size_t n);

extern long int ffsize(FILE *fp);

extern FILE *fpbtree;
char **global_arr_succ;
int global_succ_count = 0;

int get_successors(char *targetKey, int k, char *result[]) {
    /* Print an error message if strlen(key) > MAXWORDSIZE */
    POSTINGSPTR PostOffset;
    NUMKEYS NumKeys;
    KEYLEN KeyLen;
    NUMBYTES NumBytes;
    PAGENO PtrToNxtLfPg, PgNum;
    int j;
    char key[MAXWORDSIZE], Ch;
    if (k < 0) {
        printf("k should be positive not %d", k);
        return -1;
    }
    if (targetKey == NULL) {
        printf("ERROR key cannot be NULL\n");
        return -1;
    }
    if (strlen(targetKey) > MAXWORDSIZE) {
        printf("ERROR in \"get_successors\":  Length of Word Exceeds Maximum Allowed\n");
        printf(" and word May Be Truncated\n");
        return -1;
    }

    if (check_word(targetKey) == FALSE) {
        printf("ERROR key doesn't contain only alphanumerics\n");
        return -1;
    }


    /* turn to lower case, for uniformity */
    strtolow(targetKey);
    global_arr_succ = create_string_array(k);

    PAGENO pgno = treesearch_page(ROOT, targetKey);

    if (pgno < 1) {
        printf("ERROR page numbers start from 1 and on\n");
        return -1;
    }
    struct PageHdr *PagePtr = FetchPage(pgno);

    if (PagePtr == NULL) {
        printf("ERROR PagePtr is NULL\n");
        return -1;
    }
    if (fpbtree == NULL) {
        printf("ERROR fbp is NULL\n");
        return -1;
    }
    if (ffsize(fpbtree) <= (pgno - 1) * PAGESIZE) { /* illegal page number */
        printf("ERROR page numbers start from 1 and not exceed the last one \n");
        return -1;
    }

    fseek(fpbtree, (long) (pgno - 1) * PAGESIZE, 0);
    fread(&Ch, sizeof(Ch), 1, fpbtree);
    fread(&PgNum, sizeof(PgNum), 1, fpbtree);
    if (Ch == LeafSymbol) {
        fread(&PtrToNxtLfPg, sizeof(PtrToNxtLfPg), 1, fpbtree);
    }
    fread(&NumBytes, sizeof(NumBytes), 1, fpbtree);
    fread(&NumKeys, sizeof(NumKeys), 1, fpbtree);
    for (j = 0; j < NumKeys; j++) {
        if (Ch == LeafSymbol) {
            fread(&KeyLen, sizeof(KeyLen), 1, fpbtree);
            fread(key, sizeof(char), KeyLen, fpbtree);
            (*(key + KeyLen)) = '\0';
            if (CompareKeys(key, targetKey) == 2 && k > 0) {
                k--;
                strcpy(global_arr_succ[global_succ_count], key);
                fflush(stdout);
                global_succ_count++;
            }
            fread(&PostOffset, sizeof(PostOffset), 1, fpbtree);
        }
    }

    int total = k, retrieved = 0;
    while (total > 0 && (PagePtr->PgNumOfNxtLfPg) != NULLPAGENO) {
        struct PageHdr *temp = PagePtr;
        PagePtr = FetchPage(temp->PgNumOfNxtLfPg);
        FreePage(temp);
        retrieved = printLeaf(PagePtr, targetKey, total);
        total -= retrieved;
    }

    int g = 0;
    int f = (global_succ_count - 1);
    printf("found %d successors:\n", f + 1);
    for (; g <= f; g++) {
        printf("%s\n", global_arr_succ[g]);
    }
    free(global_arr_succ);
    global_succ_count = 0;

    return f;
}
