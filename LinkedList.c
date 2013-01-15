// LinkedList.c
//
#include "LinkedList.h"
#include <stdlib.h>
#include <string.h>
// POINT specific functions
int cmpPts(const PADT pa, const PADT pb) {
	// returns > 0 if a > b
	// return    0 if a == b
	// returns < 0 if a , b
	return ((POINT *)pa)->x - ((POINT *)pb)->x;
}




PADT *makePtADT(int x, int y) {
	PADT *pres = malloc(sizeof(POINT));
	((POINT *)pres)->x = x;
	((POINT *)pres)->y = y;
	return pres;
}

void freePt(PADT pit) {
	free(pit);
}

// int specific functions
int cmpInts(const PADT pa, const PADT pb) {
	// returns > 0 if a > b
	// return    0 if a == b
	// returns < 0 if a , b
	int *pia = (int *) pa;
	int *pib = (int *) pb;
	return *pia - *pib;
}



PADT *makeIntADT(int value) {
	PADT *pres = malloc(sizeof(int));
	*((int *) pres) = value;
	return pres;
}


void freeInt(PADT pit) {
	free(pit);
}

// string specific functions
//
// don't need a comparator - strcmp does fine
//



PADT *makeStrADT(const char *it) {
	return (PADT *) strdup(it);
}

void freeStr(PADT pit) {
	free(pit);
}















// Generic Functions
void addToHead(LinkedList *lst, PADT padt) {
	LLNode *pnode = 
		(LLNode *) calloc(1, sizeof(LLNode));
	pnode->data = padt;
	pnode->next = lst->head;
	lst->head = pnode;
	if(!lst->tail) lst->tail = lst->head;
}

void addToTail(LinkedList *lst, PADT padt) {
	LLNode *pnode = 
		(LLNode *) calloc(1, sizeof(LLNode));
	pnode->data = padt;
	if(lst->tail) {
		lst->tail->next = pnode;
		lst->tail = pnode;
	} else {
		lst->tail = pnode;
		lst->head = lst->tail;
	}
}

void error(const char *str) {
	printf("error: %s\n", str);
	exit(1);
}


void insertInOrder(LinkedList *plst, 
				   PADT padt,
				   ADTcomp cmp) {
	LLNode **pphere = &(plst->head);
	LLNode *pnode = 
		(LLNode *) calloc(1, sizeof(LLNode));
	pnode->data = padt;
	while(*pphere 
		&& cmp((*pphere)->data, pnode->data) < 0 ) {
		pphere = &((*pphere)->next);
	}
	if(*pphere) {
		pnode->next = *pphere;
	} else {
		plst->tail = pnode;
	}
	*pphere = pnode;
}


/*
PADT remFromHead(LinkedList *lst) {
	PADT ret;
	LLNode *nxt;
	if(lst->head) {
		ret = lst->head->data;
		nxt = lst->head->next;
		free(lst->head);
		lst->head = nxt;
	} else {
		error("removing from an empty list");
	}
	return ret;
}

void LLtraverse(LinkedList it,
			   ADTshow shw) {
	LLNode *phere = it.head;
	while(phere) {
		shw(phere->data);
		phere = phere->next;
	}
}
*/

PADT LLfind(LinkedList it,
			   ADTmatch mtch) {
	PADT res = NULL;
	LLNode *phere = it.head;
	bool found = mtch(phere->data);
	while(phere && !found) {
		phere = phere->next;
		if(phere) found = mtch(phere->data);
	}
	if(found) res = phere->data;
	return res;
}

/*
bool LLdelete(LinkedList *plst, PADT pit,
			ADTfree fr){
	//bool res = false;
	LLNode *pnode;
	LLNode **pphere = &(plst->head);
	while(*pphere && (*pphere)->data != pit) {
		pphere = &((*pphere)->next);
	}
	if(*pphere) {
		pnode = *pphere;
		*pphere = pnode->next;
		fr(pnode->data);
		free(pnode);
	}
}
*/

void LLFree( LinkedList *it,
			ADTfree fr) {
	LLNode *phere = it->head;
	LLNode *theNext;
	while(phere) {
		theNext = phere->next;
		fr(phere->data);
		free(phere);
		phere = theNext;
	}
	it->head = NULL;
	it->tail = NULL;
}





