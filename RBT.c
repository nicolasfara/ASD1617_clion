//
// Created by nicolasfarabegoli on 8/4/17.
//

#include "RBT.h"

double relMax = 0;
NODO* firstNode;
NODO* secondNode;
NODO* thirdNode;

/**
 *
 * @param dictionary
 * @return
 */
int empties_dictionary(NODO **dictionary) {
    if ((*dictionary) == sentinel || (*dictionary) == NULL)
        return 0;
    NODO *l = (*dictionary)->left;				//salvo due sottoalberi (figli)
    NODO *r = (*dictionary)->right;
    free((*dictionary));						//elimino la radice
    empties_dictionary(&l);						//richiamo sui sottoalberi
    empties_dictionary(&r);
    return 0;
}

/**
 *
 * @param n
 * @param index
 * @param nthnode
 */
void find_index_word(NODO* n, int index, NODO** nthnode){

    if(n == sentinel)
        return;

    find_index_word(n->left, index, nthnode);
    if(count_find++ == index){
        *nthnode = n;
        return;
    }
    find_index_word(n->right, index, nthnode);
}

/**
 *
 * @param n
 * @param head
 * @param word
 * @return
 */
int search_in_node(NODO *n, MSWNode *head, char *word) {
    if (n->word == NULL)
        return 0;

    int ris = search_in_node(n->left, head, word);

    if (!strcmp(n->word, word))
        ris = 1;
    else {
        int ln = (int)strlen(n->word);
        int lw = (int)strlen(word);
        int dist = levenshtein(n->word, ln, word, lw);
        if (dist < head[2].distance) {		//se la nuova parola � pi� simile la sostituisco alla meno simile di quelle gia salvate
            head[2].distance = (short)dist;
            *(head[2].w_pointer) = n->word;
            if (head[2].distance < head[1].distance) { //se � pi� simile della seconda ne cambio posizione
                MSWNode tmp = head[1];
                head[1] = head[2];
                head[2] = tmp;
                if (head[1].distance < head[0].distance) { //idem col precedente if
                    tmp = head[0];
                    head[0] = head[1];
                    head[1] = tmp;
                }
            }
        }
    }
    return ris | search_in_node(n->right, head, word);
}

/**
 *
 * @param dictionary
 * @param word
 */
void setSimilarity(NODO *dictionary, char* word) {

    if(dictionary == sentinel)
        return;

    setSimilarity(dictionary->left, word);
    double sim = CalculateSimilarity(dictionary->word, word);
    dictionary->percentageSimilarity = sim;
    setSimilarity(dictionary->right, word);
}

/**
 *
 * @param root
 * @param x
 */
void leftRotate(NODO** root, NODO* x)
{
    NODO* y = x->right; //Create new NODO and assign to x.right
    x->right = y->left; //Move left sub-tree (y) on the x sub-tree

    if (y->left != sentinel) y->left->parent = x;

    y->parent = x->parent; //Connect parent of x to y

    if (x->parent == sentinel)
        *root = y;
    else if (x == x->parent->left)
        x->parent->left = y;

    else x->parent->right = y;

    y->left = x; //Move x on the left of y
    x->parent = y;
}

/**
 *
 * @param root
 * @param y
 */
void rightRotate(NODO** root, NODO* y)
{
    NODO* x = y->left; //Create new NODO and assign to x.right
    y->left = x->right; //Move left sub-tree (y) on the x sub-tree

    if (x->right != sentinel) x->right->parent = y;

    x->parent = y->parent; //Connect parent of x to y

    if (y->parent == sentinel)
        *root = x;
    else if (y == y->parent->right)
        y->parent->right = x;

    else y->parent->left = x;

    x->right = y; //Move x on the left of y
    y->parent = x;
}

/**
 *
 * @param root
 * @param node
 */
void insertFixUp(NODO** root, NODO** node) {
    NODO* T = *root;
    NODO* z = *node; //Puntatore temporaneo al nodo
    NODO* y = NULL;
    //Ciclo
    while (z->parent != NULL && z->parent->isBlack == false) {
        //Nodo di sinsitra
        if (z->parent == z->parent->parent->left) {
            y = z->parent->parent->left;
            if (y->isBlack == false) {
                z->parent->isBlack = true;
                y->isBlack = true;
                z->parent->parent->isBlack = false;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->right) {
                    z = z->parent;
                    leftRotate(root, z); //rotazione sinistra
                }
                z->parent->isBlack = true;
                z->parent->parent->isBlack = false;
                rightRotate(root, z->parent->parent);
            }
        }
        else { //nodo di destra
            y = z->parent->parent->left;
            if (y->isBlack == false) {
                z->parent->isBlack = true;
                y->isBlack = true;
                z->parent->parent->isBlack = false;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(root, z); //rotazione destra
                }
                z->parent->isBlack = true;
                z->parent->parent->isBlack = false;
                leftRotate(root, z->parent->parent);
            }
        }
    }

    T->isBlack = true;
}

/**
 *
 * @param root
 * @param node
 */
void insertRBT(NODO** root, NODO* node) {
    NODO* y = NULL;
    NODO* x = NULL;

    y = sentinel;
    x = *root;
    //Scorro fino a che non sono finiti i nodi
    while (x != sentinel && x != NULL) {
        y = x;
        switch (alphabeticalOrder(node->word, x->word))	{
            case 0: //Caso maggiore
                x = x->left;
                break;
            case 1: //caso minore
                x = x->right;
                break;
            case 2:
                return;
            default:
                break;
        }
    }

    node->parent = y;
    if (y == sentinel)
        *root = node;
    else {
        switch (alphabeticalOrder(node->word, y->word))	{
            case 0:
                y->left = node;
                break;
            case 1:
                y->right = node;
                break;
            case 2:
                return;
            default:
                break;
        }
    }
    node->left = sentinel;
    node->right = sentinel;
    node->isBlack = false;
    insertFixUp(root, &node);
}

/**
 *
 * @param root
 * @param z
 */
void rb_delete(NODO** root, NODO* z) {
    NODO* y = z;
    NODO* x;
    bool original = y->isBlack;
    //controllo se il figlio destro � la sentinella
    if (z->left == sentinel) {
        x = z->right;
        rb_transplant(root, z, z->right);
    }
        //Controllo se il figlio sinsitro � sentinella
    else if (z->right == sentinel) {
        x = z->left;
        rb_transplant(root, z, z->left);
    }
        //entrambi i figli sono nodi
    else {
        y = treeMinimum(z->right);
        original = y->isBlack;
        x = y->right;
        if (y->parent == z)
            x->parent = y;
        else {
            rb_transplant(root, y, y->right);
            y->right = z->right;
            y->right->parent = y;
        }
        rb_transplant(root, z, y);
        y->left = z->left;
        y->left->parent = y;
        y->isBlack = z->isBlack;
    }
    if (original == true)
        rb_deleteFixUp(root, x);

    free(z);
}

/*NODO* treeSuccessor(NODO* root, NODO* x) {
    if (x->right != sentinel)
        return treeMinimum(x->right); //ritono il minimo

    NODO* y = x->parent;
    //Scorro fino a che non trovo il successore
    while (y != NULL && x == y->right) {
        x = y;
        y = y->parent;
    }
    return y; //successore
}*/

/**
 *
 * @param root
 * @param word
 * @return
 */
NODO* searchWord(NODO* root, char* word) {
    //caso base
    if (root == NULL || alphabeticalOrder(root->word, word) == 2)
        return root;

    switch (alphabeticalOrder(root->word, word)) {
        case 0: //Peso Maggiore
            return searchWord(root->right, word);
            break;
        case 1: //peso minore
            return searchWord(root->left, word);
            break;
        default:
            return NULL;
    }

    //Here there is an error
    return NULL;

}

/**
 *
 * @param root
 * @param u
 * @param v
 */
void rb_transplant(NODO **root, NODO *u, NODO *v) {
    //Sono nella radice
    if (u->parent == sentinel)
        *root = v;
        //figlio sinistro
    else if (u == u->parent->left)
        u->parent->left = v;
    else { //figlio destro
        u->parent->right = v;
    }

    v->parent = u->parent; //aggiorno puntatori
}

/**
 *
 * @param root
 * @param x
 */
void rb_deleteFixUp(NODO** root, NODO * x) {

    NODO* w = NULL;
    //Scorro
    while (*root != x && x->isBlack == true) {
        //� a sinistra
        if (x == x->parent->left) {
            w = x->parent->right;
            if (w->isBlack == false) {
                w->isBlack = true;
                x->parent->isBlack = false;
                leftRotate(root, x->parent); //rotazione
                w = x->parent->right;
            }
            if ((w->left->isBlack == true) && (w->right->isBlack == true)) {
                w->isBlack = false;
                x = x->parent;
            }
            else {
                if (w->right->isBlack == true) {
                    w->left->isBlack = true;
                    w->isBlack = false;
                    rightRotate(root, w); //rotazione destra
                    w = x->parent->right; //nodo di sinistra
                }
                w->isBlack = x->parent->isBlack;
                x->parent->isBlack = true;
                w->right->isBlack = true;
                leftRotate(root, x->parent);
                x = *root; //assegno la radice
            }
        }
        else { //� a destra
            w = x->parent->left;
            if (w->isBlack == false) {
                w->isBlack = true;
                x->parent->isBlack = false;
                rightRotate(root, x->parent);
                w = x->parent->left;
            }
            if ((w->right->isBlack == true) && (w->left->isBlack == true)) {
                w->isBlack = false;
                x = x->parent; //aggiorno al padre
            }
            else {
                if (w->left->isBlack == true) {
                    w->right->isBlack = true;
                    w->isBlack = false;
                    leftRotate(root, w); //rotazione sinistra
                    w = x->parent->left;
                }
                w->isBlack = x->parent->isBlack;
                x->parent->isBlack = true;
                w->left->isBlack = true;
                rightRotate(root, x->parent); //rotazione destra
                x = *root; // assegno radice
            }
        }

    }
    x->isBlack = true; //sistemo colore
}

/**
 *
 * @param x
 * @return
 */
NODO* treeMinimum(NODO* x) {
    //Scorro fino al minimo
    while (x->left != sentinel)
        x = x->left;

    return x;
}

/**
 *
 * @return
 */
int createSentinel() {
    //Create node
    sentinel = (NODO*)malloc(sizeof(NODO));
    //Check for a safe malloc
    if (sentinel == NULL)
        return -1; //Bad malloc

    //Update record for sentinel
    sentinel->isBlack = true;
    sentinel->parent = NULL;
    sentinel->left = NULL;
    sentinel->right = NULL;
    sentinel->word = NULL;
    sentinel->def = NULL;

    return 0; //Ok
}

/**
 *
 * @param dictionary
 * @param exclude1
 * @param exclude2
 * @param count
 */
void relativeMax(NODO* dictionary, NODO* exclude1, NODO* exclude2, int count) {

    if(dictionary == sentinel)
        return;

    relativeMax(dictionary->left, exclude1, exclude2, count);

    if(dictionary != exclude1 && dictionary != exclude2 && dictionary->percentageSimilarity > relMax) {
        relMax = dictionary->percentageSimilarity;
        switch(count) {
            case 1:
                firstNode = dictionary;
                break;
            case 2:
                secondNode = dictionary;
                break;
            case 3:
                thirdNode = dictionary;
                break;
            default:
                break;
        }
    }

    relativeMax(dictionary->right, exclude1, exclude2, count);

}