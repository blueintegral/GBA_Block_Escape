// LinkedList.h
#include <stdio.h>
//#include "CMemLeak.h"
#define bool int
#define false 0
#define true 1

typedef void *PADT;
typedef void (* ADTshow)(const PADT);
typedef void (* ADTfree)(const PADT);
typedef int (* ADTcomp)(const PADT, const PADT);
typedef bool (* ADTmatch)(const PADT);

// Generic Linked List Definitions
typedef struct pt_t {
	int x;
	int y;
} POINT;

typedef struct llnode_t {
	           PADT  data;
	struct llnode_t *next;
} LLNode;

typedef struct linkedlist {
	LLNode *head;
	LLNode *tail;
} LinkedList;

void error(const char *str);

// Generic Functions
void addToHead(LinkedList *lst, PADT padt);
void addToTail(LinkedList *lst, PADT padt);
void insertInOrder(LinkedList *lst, 
				   PADT padt,
				   ADTcomp cmp);
PADT remFromHead(LinkedList *lst);
void LLtraverse(LinkedList it,
			   ADTshow shw);
PADT LLfind(LinkedList lst, ADTmatch test);
bool LLdelete(LinkedList *plst, PADT pit,
			   ADTfree fr);
void LLFree( LinkedList *plst, ADTfree fr);
int cmpPts(const PADT pa, const PADT pb);
void showPt(const PADT pit);
PADT *makePtADT(int x, int y);
void freePt(PADT pit);
int cmpInts(const PADT pa, const PADT pb);
void showInt(const PADT pit);
PADT *makeIntADT(int value);
bool checkGTvalue(PADT pit);
void freeInt(PADT pit);
void showStr(const PADT pit);
PADT *makeStrADT(const char *it);
void freeStr(PADT pit);

