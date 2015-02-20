#include <stdio.h>
#include <string.h>
#include "def.h"


extern int iscommon(char *word);

extern int CompareKeys(char *Key, char *Word);

extern struct PageHdr *FetchPage(PAGENO Page);

extern FILE *fpbtree;

extern int check_word(char *word);

extern int strtolow(char *s);

extern int getpostings(POSTINGSPTR pptr);

extern PAGENO treesearch_page_buildLL(int PageNo, char *key, struct node *root);

extern void createstackfromLL(struct node *pNode);

struct node *head = NULL;

int search_build_list(char *key, int k, int flag);

void clearHeadPtr();

int iterate_stack(FILE *fpb, char *key, char *targetKey, int k);

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

extern char **create_string_array(size_t n);

int get_predecessors(char *key, int k, char *result[]) {
    if (k < 0) {
        printf("k should be positive not %d", k);
        return -1;
    }

    if (strlen(key) > MAXWORDSIZE) {
        printf("ERROR in \"get_predecessors\":  Length of Word Exceeds Maximum Allowed\n");
        printf(" and word May Be Truncated\n");
        return -1;
    }

    if (check_word(key) == FALSE) {
        return -1;
    }
    strtolow(key);

    int r = search_build_list(key, k, FALSE);
    return r;
}


int search_build_list(char *key, int k, int flag) {
    POSTINGSPTR pptr;

    struct node *root = NULL;
    pptr = treesearch_page_buildLL(ROOT, key, root);
    int number_of_predecessors;

    if (pptr == NONEXISTENT) {
        printf("key \"%s\": not found\n", key);
    } else {
        if (flag) {
            getpostings(pptr);
        } else {
            createstackfromLL(head);
            char temp[MAXWORDSIZE];
            number_of_predecessors = iterate_stack(fpbtree, temp, key, k);
        }
        clearHeadPtr();
    }
    return number_of_predecessors;
}

int iterate_stack(FILE *fpb, char *key, char *targetKey, int k) {
    char Ch;
    char **global_arr = create_string_array(k);
    int global_ptr = 0;
    POSTINGSPTR PostOffset;
    PAGENO PgNum, PtrToNxtLfPg, PtrToFinalRtgPg;
    NUMKEYS NumKeys;
    KEYLEN KeyLen;
    NUMBYTES NumBytes;
    int homePage, isHome = 0, number_of_predecessors = 0;
    while (!empty() && k > 0) {
        PAGENO curr = pop();
        if (!isHome) {
            isHome = 1;
            homePage = curr;
        }
        fseek(fpbtree, (long) (curr - 1) * PAGESIZE, 0);
        fread(&Ch, sizeof(Ch), 1, fpb);
        if (feof(fpbtree) != 0)
            exit(0);
        fread(&PgNum, sizeof(PgNum), 1, fpb);
        if (Ch == LeafSymbol) {
            fread(&PtrToNxtLfPg, sizeof(PtrToNxtLfPg), 1, fpb);
        }
        fread(&NumBytes, sizeof(NumBytes), 1, fpb);
        fread(&NumKeys, sizeof(NumKeys), 1, fpb);
        if (Ch == NonLeafSymbol) {
            fread(&PtrToFinalRtgPg, sizeof(PtrToFinalRtgPg), 1, fpb);
        }
        char values[NumKeys][MAXWORDSIZE];

        int j = 0, hits = 0;
        for (; j < NumKeys; j++) {
            if (Ch == NonLeafSymbol) {
                fread(&PgNum, sizeof(PgNum), 1, fpb);
                fread(&KeyLen, sizeof(KeyLen), 1, fpb);
                fread(key, sizeof(char), KeyLen, fpb);
                (*(key + KeyLen)) = '\0';
                int Result = CompareKeys(key, targetKey);
                if (Result == 1) {
                    push(PgNum);
                }
            }
            if (Ch == LeafSymbol) {
                fread(&KeyLen, sizeof(KeyLen), 1, fpb);
                fflush(stdout);
                fread(key, sizeof(char), KeyLen, fpb);
                (*(key + KeyLen)) = '\0';
                if (CompareKeys(key, targetKey) == 1) {
                    strcpy(values[j], key);
                    hits++;
                }
                fread(&PostOffset, sizeof(PostOffset), 1, fpb);
            }
        }
        if (Ch == NonLeafSymbol) {
            if (homePage != PtrToFinalRtgPg) {
                push(PtrToFinalRtgPg);
            }
        }
        int p = hits - 1;

        for (; p >= 0 && k > 0; p--) {
            number_of_predecessors++;
            strcpy(global_arr[global_ptr], values[p]);
            global_ptr++;
            --k;
        }
    }
    int g = global_ptr - 1;
    printf("found %d predecessors:\n", number_of_predecessors);
    for (; g >= 0; g--) {
        printf("%s\n", global_arr[g]);
    }
    fflush(stdout);
    return number_of_predecessors;
}

void clearHeadPtr() {
    head = NULL;

}

/* Create empty stack */
void create() {
    top = NULL;
}

/* Count stack elements */
void stack_count() {
    //////printf("\n No. of elements in stack : %d", count);
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
        //////printf("\n Error : Trying to pop from empty stack");
        return -1;
    }
    else
        top1 = top1->ptr;
    //////printf("\n Popped value : %ld", top->info);
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
    if (top == NULL) {
        //////printf("\n Stack is empty");
        return 1;
    }
    else {
        //////printf("\n Stack is not empty with %d elements", count);
        return 0;
    }
}

void display() {
    top1 = top;

    if (top1 == NULL) {
        ////printf("Stack is empty");
        return;
    }
    //printf("\nStack data:\n");
    while (top1 != NULL) {
        //printf("%ld ", top1->info);
        top1 = top1->ptr;
    }
    //printf("\n");
}