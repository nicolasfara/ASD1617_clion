//
// Created by nicolasfarabegoli on 8/4/17.
//

#ifndef ASD1617_RBT_H
#define ASD1617_RBT_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "utility.h"

#define MAX_DEF 50
#define MAX_WORD 20
#define MIN_WORD 2
#define ELEMENTS sizeof(letter_frequencies) / sizeof(int)

//NODO RBT
typedef struct NODO {
    char* word;
    char* def;
    bool isBlack;
    struct NODO *left;
    struct NODO *right;
    struct NODO *parent;
    double percentageSimilarity;
}NODO;

//NODO AUSILIARIO RICERCA PAROLE PIU' SIMILI
typedef struct MSW {		//most similar word
    char **w_pointer;
    short int distance;
} MSWNode;

NODO *sentinel; //Sentinella per le foglie e padre della radice

extern double relMax;
extern NODO* firstNode;
extern NODO* secondNode;
extern NODO* thirdNode;

void leftRotate(NODO** root, NODO* x);
void rightRotate(NODO** root, NODO* y);
void insertFixUp(NODO** root, NODO** node);
void insertRBT(NODO** root, NODO* node);
void rb_delete(NODO** root, NODO* z);
//NODO* treeSuccessor(NODO* root, NODO* x);
NODO* searchWord(NODO* root, char* word);
void rb_transplant(NODO **root, NODO *u, NODO *v);
void rb_deleteFixUp(NODO** root, NODO * x);
NODO* treeMinimum(NODO* x);
int createSentinel();

int empties_dictionary(NODO **dictionary);
void find_index_word(NODO* n, int index, NODO** nthnode);
int search_in_node(NODO *n, MSWNode *head, char *word);
void setSimilarity(NODO *dictionary, char* word);
void relativeMax(NODO* dictionary, NODO* exclude1, NODO* exclude2, int count);

#endif //ASD1617_RBT_H



