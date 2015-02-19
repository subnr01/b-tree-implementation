#include <stdio.h>
#include <string.h>
#include "def.h"

extern void printLL(struct node *pNode);

extern int iscommon(char *word);

extern struct PageHdr *FetchPage(PAGENO Page);

extern FILE *fpbtree;

extern int check_word(char *word);

extern int strtolow(char *s);

extern int getpostings(POSTINGSPTR pptr);

extern PAGENO treesearch_page_buildLL(int PageNo, char *key, struct node *root);

extern void createstackfromLL(struct node *pNode);

struct node *head = NULL;

void search_build_list(char *key, int k, int flag);

void clearHeadPtr();

void build_pred_list(char *key, int k, FILE *pFILE);

void iterate_stack(FILE *fpb, char *key);

struct stacknode {
    PAGENO info;
    struct stacknode *ptr;
} *top, *top1, *temp;

PAGENO topelement();

void push(PAGENO data);

PAGENO pop();

int empty();

void display();

void destroy();

void stack_count();

void create();

int count = 0;

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
            //printLL(head);

            createstackfromLL(head);
            display();
            //build_pred_list(key, k, fpbtree);
            iterate_stack(fpbtree, key);
            printf("Found the key!\n");
        }
        clearHeadPtr();
    }
}

void iterate_stack(FILE *fpb, char *key) {
    char Ch;

    POSTINGSPTR PostOffset;
    PAGENO PgNum, PtrToNxtLfPg, PtrToFinalRtgPg;
    NUMKEYS NumKeys;
    KEYLEN KeyLen;
    NUMBYTES NumBytes;
    while (!empty()) {
        PAGENO curr = pop();
        printf("accessing from stack:%ld\n", curr);
        fseek(fpbtree, (long) (curr - 1) * PAGESIZE, 0);
        fread(&Ch, sizeof(Ch), 1, fpb);
        if (feof(fpbtree) != 0)
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
        //struct PageHdr *PagePtr = FetchPage(curr);
        if (Ch == NonLeafSymbol) {
            fread(&PtrToFinalRtgPg, sizeof(PtrToFinalRtgPg), 1, fpb);
            printf("PtrToFinalRtgPg%d\n", (int) PtrToFinalRtgPg);
        }
        int j = 0;
        for (; j < NumKeys; j++) {
            if (Ch == NonLeafSymbol) {
                fread(&PgNum, sizeof(PgNum), 1, fpb);
                printf("NextPage:%d, ", (int) PgNum);
                fread(&KeyLen, sizeof(KeyLen), 1, fpb);
                fread(key, sizeof(char), KeyLen, fpb);
                (*(key + KeyLen)) = '\0';
                printf("key:%s, ", key);
                printf("length:%d\n", KeyLen);
            }
            if (Ch == LeafSymbol) {
                fread(&KeyLen, sizeof(KeyLen), 1, fpb);
                printf("Keylength:%d, ", KeyLen);
                fflush(stdout);
                fread(key, sizeof(char), KeyLen, fpb);
                (*(key + KeyLen)) = '\0';
                printf("key:%s, ", key);
                fread(&PostOffset, sizeof(PostOffset), 1, fpb);
                printf("Postings offset:%d\n", (int) PostOffset);
            }
        }
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

/* Create empty stack */
void create() {
    top = NULL;
}

/* Count stack elements */
void stack_count() {
    printf("\n No. of elements in stack : %d", count);
}

/* Push data into stack */
void push(PAGENO data) {
    if (top == NULL) {
        top = (struct stacknode *) malloc(1 * sizeof(struct stacknode));
        top->ptr = NULL;
        top->info = data;
    }
    else {
        temp = (struct stacknode *) malloc(1 * sizeof(struct stacknode));
        temp->ptr = top;
        temp->info = data;
        top = temp;
    }
    count++;
}

/* Pop Operation on stack */
PAGENO pop() {
    top1 = top;

    if (top1 == NULL) {
        printf("\n Error : Trying to pop from empty stack");
        return -1;
    }
    else
        top1 = top1->ptr;
    printf("\n Popped value : %ld", top->info);
    PAGENO saved = top->info;
    //free(top);
    top = top1;
    count--;
    return saved;
}

/* Return top element */
PAGENO topelement() {
    return (top->info);
}

int empty() {
    if (top == NULL)
    {
        printf("\n Stack is empty");
        return 1;
    }
    else
    {
        printf("\n Stack is not empty with %d elements", count);
        return 0;
    }
}

void display() {
    top1 = top;

    if (top1 == NULL) {
        printf("Stack is empty");
        return;
    }

    while (top1 != NULL) {
        printf("%ld ", top1->info);
        top1 = top1->ptr;
    }
}