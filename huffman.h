//
// Created by nicolasfarabegoli on 8/4/17.
//

#ifndef ASD1617_HUFFMAN_H
#define ASD1617_HUFFMAN_H


#include <stdio.h>
#include <stdlib.h>
#include "RBT.h"


//NODO ALBERO DI HUFFMAN
typedef struct NODE{
    int frequencies;
    char letter;
    struct NODE *left, *right;
    struct NODE *next;
} HNode;


extern int letter_frequencies[31];

HNode *build_huffman_tree();
HNode *allocates_node(int i);
HNode *extract_smaller_one(HNode **nodes_head);
void fill_table(unsigned int *code_table, HNode *tree_node, unsigned int code);
int compress_node(NODO * n, FILE * output, unsigned int *code_table);
void compress_string(char *n_string, FILE *output, unsigned int *code_table);
int decompress_file(FILE *input, NODO **dict_root, HNode *tree);

#endif //ASD1617_HUFFMAN_H

