//
// Created by nicolasfarabegoli on 8/4/17.
//

#ifndef ASD1617_UTILITY_H
#define ASD1617_UTILITY_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#define MAX_DEF 50
#define MAX_WORD 20
#define MIN_WORD 2
#define ELEMENTS sizeof(letter_frequencies) / sizeof(int)



extern unsigned short count_find;


int levenshtein(const char *s, int ls, const char *t, int lt);
unsigned char convert_accent(unsigned char c);
short readWordDef(FILE* f, char* word, char* def, bool* endFile);
void removeChar(char* str, char garbage, char garbage2);
unsigned short alphabeticalOrder(const char* n1, const char* n2);
int distance(char *source, char *target);
double CalculateSimilarity(char *source, char *target);
#endif //ASD1617_UTILITY_H

