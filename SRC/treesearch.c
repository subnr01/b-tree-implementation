/*
    searches the subtree starting at the node stored at the page
    with number
        PageNo
    for the
        key.
    It returns the offset from the postings file, or ERROR, if not found
 */

#include "def.h"

extern POSTINGSPTR searchLeaf(struct PageHdr *PagePtr, char *key, int printFlag);

extern int FreePage(struct PageHdr *PagePtr);

extern PAGENO FindPageNumOfChild(struct PageHdr *PagePtr,
        struct KeyRecord *KeyListTraverser, char *Key,
        NUMKEYS NumKeys);

extern struct PageHdr *FetchPage(PAGENO Page);

extern struct node *head;

void printLL(struct node *pNode);

void createstackfromLL(struct node *pNode);

extern struct stacknode *top, *top1, *temp;

extern void push(PAGENO data);

extern void display();

/**
* recursive call to find the page in which the key should reside
* and return the page number (guaranteed to be a leaf page).
*/
PAGENO treesearch_page(PAGENO PageNo, char *key) { // ROOT, word
    PAGENO result;
    struct PageHdr *PagePtr = FetchPage(PageNo);
    if (PagePtr != NULL) {
        printf("\ntree search - pgnum:%ld", PagePtr->PgNum);
    }
    if (IsLeaf(PagePtr)) { /* found leaf */
        result = PageNo;
    } else if ((IsNonLeaf(PagePtr)) && (PagePtr->NumKeys == 0)) {
        /* keys, if any, will be stored in Page# 2
           THESE PIECE OF CODE SHOULD GO soon! **/
        result = treesearch_page(FIRSTLEAFPG, key);
    } else if ((IsNonLeaf(PagePtr)) && (PagePtr->NumKeys > 0)) {
        PAGENO ChildPage = FindPageNumOfChild(PagePtr, PagePtr->KeyListPtr, key,
                PagePtr->NumKeys);
        result = treesearch_page(ChildPage, key);
    } else {
        assert(0 && "this should never happen");
    }
    FreePage(PagePtr);
    return result;
}

/**
* find the posting pointer to which the key should reside, given the
* starting page number to look at.
*
* to search the whole tree, pass in ROOT as the page number.
*/
POSTINGSPTR treesearch(PAGENO PageNo, char *key) {
    /* recursive call to find page number */
    const PAGENO page = treesearch_page(PageNo, key);
    /* from page number we traverse the leaf page */
    struct PageHdr *PagePtr = FetchPage(page);
    POSTINGSPTR result = searchLeaf(PagePtr, key, 1);
    FreePage(PagePtr);
    return result;
}

struct PageHdr *treesearch_pageHdr(PAGENO PageNo, char *key) {
    /* recursive call to find page number */
    const PAGENO page = treesearch_page(PageNo, key);
    /* from page number we traverse the leaf page */
    struct PageHdr *PagePtr = FetchPage(page);
    //FreePage(PagePtr);
    return PagePtr;
}


PAGENO treesearch_page_buildLL(int PageNo, char *key, struct node *root) { // ROOT, word
    PAGENO result;
    struct PageHdr *PagePtr = FetchPage(PageNo);
    if (PagePtr != NULL) {
        root = (struct node *) malloc(sizeof(struct node *));
        root->value = PagePtr->PgNum;
        root->next = head;
        head = root;
    }
    if (IsLeaf(PagePtr)) { /* found leaf */
        result = PageNo;
        head = root;
    } else if ((IsNonLeaf(PagePtr)) && (PagePtr->NumKeys == 0)) {
        /* keys, if any, will be stored in Page# 2
           THESE PIECE OF CODE SHOULD GO soon! **/
        result = treesearch_page_buildLL(FIRSTLEAFPG, key, root);
    } else if ((IsNonLeaf(PagePtr)) && (PagePtr->NumKeys > 0)) {
        PAGENO ChildPage = FindPageNumOfChild(PagePtr, PagePtr->KeyListPtr, key,
                PagePtr->NumKeys);
        result = treesearch_page_buildLL(ChildPage, key, root);
    } else {
        assert(0 && "this should never happen");
    }
    FreePage(PagePtr);
    return result;
}

void printLL(struct node *pNode) {
    /*while (pNode) {
        printf("List of pages--:%ld\n", pNode->value);
        pNode = pNode->next;
    }*/
    //createstackfromLL(pNode);
   // display();

}

void printLLString(struct nodeString *pNode) {
    while (pNode) {
        printf("List of keys--:%s\n", pNode->value);
        pNode = pNode->next;
    }
}

void createstackfromLL(struct node *pNode) {
    if (pNode) {
        createstackfromLL(pNode->next);
        push(pNode->value);
    }
}