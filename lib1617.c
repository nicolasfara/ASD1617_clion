//
// Created by nicolasfarabegoli on 7/29/17.
//

#include "lib1617.h"
#include "huff/huffman.h"

static int memory_encode_file(FILE *in, FILE *out);
static int memory_decode_file(FILE *in, FILE *out);

NODO *createFromFile(char * nameFile) {
    unsigned short i = 0;
    unsigned char tmp;
    NODO* root = NULL;
    NODO* node = NULL;
    FILE* f = fopen(nameFile, "r");

    if (f == NULL)
        return NULL;

    createSentinel();

    while (true)
    {
        //Create node
        node = (NODO*)malloc(sizeof(NODO));
        if (node == NULL)
            return NULL;

        //Create word space
        node->word = (char*)malloc(sizeof(char) * MAX_WORD);
        if (node->word == NULL)
            return NULL;

        //Add record
        node->def = NULL;
        node->isBlack = false;
        node->left = NULL;
        node->right = NULL;
        node->parent = NULL;

        tmp = (unsigned char)getc(f);
        //Controllo che il carattere letto sia una lettera (anche accentata)
        for (i = 0; ((tmp >= 65 && tmp <= 90) || (tmp >= 97 && tmp <= 122) || (tmp >= 192 && tmp <= 252)); i++) {
            if (tmp >= 192 && tmp <= 252)
                tmp = convert_accent(tmp);
            else
                tmp = (unsigned char)tolower(tmp);

            node->word[i] = tmp;
            tmp = (unsigned char)getc(f);
        }
        node->word[i] = '\0'; //Add string terminator
        //Check if the word is 2 char lenght
        if (strlen(node->word) < 2 || strlen(node->word) > MAX_WORD) {
            free(node); //Release node
        }
        else {
            insertRBT(&root, node); //Insert node in RBT
        }
        //Check if the file pointer is at the end
        if (tmp == 0xff)
            break; //exit from the loop (infinity)
    }

    fclose(f);
    return root;
}

void printDictionary(NODO * dictionary) {//stampa in-order
    if (dictionary != NULL && dictionary != sentinel) {
        printDictionary(dictionary->left);
        if(dictionary == dictionary->parent->left)
            printf("Sinistra: ");
        else if(dictionary == dictionary->parent->right)
            printf("Destra: ");
        printf("\"%s\": ", dictionary->word);
        printf("[%s]\n", dictionary->def);
        printDictionary(dictionary->right);
    }
}

void printDictionaryFile(NODO * dictionary, FILE *f) { //Function for print on file the dictionary
    if (dictionary != NULL && dictionary != sentinel) {
        printDictionaryFile(dictionary->left, f);
        fprintf(f, "\"%s\": ", dictionary->word);
        fprintf(f, "[%s]\n", dictionary->def);
        printDictionaryFile(dictionary->right, f);
    }
}

int countWord(NODO *n) {
    if (n == NULL || n->word == NULL)						//CASO BASE (SENTINELLA)
        return 0;
    return countWord(n->left) + countWord(n->right) + 1;	//NUMERO PAROLE DEL RAMO SINISTRO + DESTRO + IL NODO
}

int insertWord(NODO ** dictionary, char * word) {
    if (strlen(word) < 2)
        return 1;

    NODO* node = (NODO*)malloc(sizeof(NODO));
    if (node == NULL)
        return 1;

    node->def = NULL;
    node->isBlack = false;
    node->left = NULL;
    node->parent = NULL;
    node->right = NULL;
    node->word = (char *)malloc(sizeof(char) * MAX_WORD);
    if (node->word == NULL)
        return 1;
    strncpy(node->word, word, MAX_WORD);

    insertRBT(dictionary, node);		//inserimento del nodo
    return 0;
}

int cancWord(NODO ** dictionary, char *word)
{
    //search node
    NODO* sWord = searchWord(*dictionary, word);
    if (sWord == NULL)
        return 1;
    //delete node
    rb_delete(dictionary, sWord);
    return 0;
}

char* getWordAt(NODO *n, int index){

    if(countWord(n) < index)
        return NULL;

    count_find = 0;
    NODO* tmp = NULL;

    find_index_word(n, index, &tmp);

    return tmp->word;
}

/*char *getWordAt(NODO *n, int index) {
	int counter = 0;
	if (n == NULL)
		return NULL;
	return find_index_word(n, index, &counter);		//richiamo una funzione ausiliaria
}*/

int insertDef(NODO * dictionary, char * word, char * def)
{
    NODO* sWord = searchWord(dictionary, word);
    if (sWord == NULL)
        return 1; //Word not found

    sWord->def = (char*)malloc(sizeof(char) * MAX_DEF);
    if (sWord->def == NULL)
        return 1;
    strncpy(sWord->def, def, MAX_DEF); //Copy the definition with safe string copy
    return 0; //All ok
}

char *searchDef(NODO * dictionary, char * word)
{
    //search the word
    NODO* sWord = searchWord(dictionary, word);
    if (sWord == NULL) //check if the word exist
        return NULL;

    return sWord->def; //return the definition
}

int saveDictionary(NODO * dictionary, char * fileOutput) {
    //check if the dictionary is empty
    if (dictionary == NULL)
        return -1;

    //Create file for write on it
    FILE* f = fopen(fileOutput, "w");
    //check if the open file was fine
    if (f == NULL)
        return -1;

    //Print on file the dictionary
    printDictionaryFile(dictionary, f);

    fclose(f); //Close file
    return 0;
}

NODO *importDictionary(char * fileInput) {

    //open file
    FILE* f = fopen(fileInput, "rb");
    if (f == NULL)
        return NULL;

    bool endFile = false; //for detect EOF
    NODO* root = NULL;
    NODO* node = NULL;

    //Allocate string for read from file
    char* word = (char*)malloc(sizeof(char) * MAX_WORD + 5);
    if (word == NULL)
        return NULL;
    char* def = (char*)malloc(sizeof(char) * MAX_DEF + 5);
    if (def == NULL)
        return NULL;
    //loop end of file
    while (!endFile) {
        //read word and definition
        if (readWordDef(f, word, def, &endFile) == -1)
            return NULL;
        //new node
        node = (NODO*)malloc(sizeof(NODO));
        if (node == NULL)
            return NULL;
        node->def = (char*)malloc(sizeof(char) * MAX_DEF);
        node->word = (char*)malloc(sizeof(char) * MAX_WORD);
        if (node->word == NULL || node->def == NULL)
            return NULL;
        //check if the def is null
        if (!strncmp(def, "(null)", MAX_DEF))
            node->def = NULL;
        else
            strncpy(node->def, def, MAX_DEF);
        strncpy(node->word, word, MAX_WORD);
        node->isBlack = false;
        node->left = sentinel;
        node->parent = sentinel;
        node->right = sentinel;
        //add to the tree
        insertRBT(&root, node);
    }

    return root;
}

/*int searchAdvance(NODO * dictionary, char * word, char ** first, char ** second, char ** third) {
    if (word == NULL || dictionary == NULL) {
        *first = *second = *third = NULL;
        return -1;
    }

    MSWNode head[3];
    head[0].w_pointer = first;
    head[1].w_pointer = second;
    head[2].w_pointer = third;
    head[0].distance = head[1].distance = head[2].distance = 0x7fff;

    int res = search_in_node(dictionary, head, word);	//chiamo search in node
    if (head[2].distance == 0x7fff)						//se ho trovato solo 2 parole simili
        return -1;
    return res;
}*/

int searchAdvance(NODO * dictionary, char * word, char ** first, char ** second, char ** third) {

    if(word == NULL)
        return -1;

    setSimilarity(dictionary, word);

    relMax = 0;
    relativeMax(dictionary, NULL, NULL, 1);
    relMax = 0;
    relativeMax(dictionary, firstNode, NULL, 2);
    relMax = 0;
    relativeMax(dictionary, firstNode, secondNode, 3);

    if(!firstNode->percentageSimilarity || !secondNode->percentageSimilarity || !thirdNode->percentageSimilarity)
        return -1;

    *first = firstNode->word;
    *second = secondNode->word;
    *third = thirdNode->word;
    return 0;
}


int compressHuffman(NODO * dictionary, char * file_name) {
    /*char eof = 27;
    unsigned int code_table[ELEMENTS];		//tabella delle codifiche
    FILE *output_file = fopen(file_name, "wb");
    HNode *root = build_huffman_tree();		//albero delle codifiche
    if (dictionary == NULL || root == NULL)
        return -1;
    fill_table(code_table, root, 0);		//salvo le codifiche nella tabella
    compress_node(dictionary, output_file, code_table);	//comprimo
    compress_string(&eof, output_file, code_table);
    fclose(output_file);

    return 0;*/
    int code;
    char in_name[] = "tmp_compress_huff";
    FILE *out = fopen(file_name, "w");
    saveDictionary(dictionary, in_name);
    FILE *in = fopen(in_name, "r");
    code = memory_encode_file(in, out);

    fclose(in);
    fclose(out);

    remove(in_name);

    return code;
}

int decompressHuffman(char * file_name, NODO ** dictionary) {
    /*int x = 0;
    FILE *input_file = fopen(file_name, "rb");
    HNode *root = build_huffman_tree();
    empties_dictionary(dictionary);						//svuoto il dizionario attuale
    if (sentinel == NULL)
        x = createSentinel();
    (*dictionary) = sentinel;
    if (root == NULL || input_file == NULL || x == -1)
        return -1;
    x = decompress_file(input_file, dictionary, root);	//decomprimo il file
    fclose(input_file);


    return x;*/
    int code;
    char out_name[] = "tmp_decompress_huff";
    FILE *input_file = fopen(file_name, "r");
    FILE *output_file = fopen(out_name, "w");
    code = memory_decode_file(input_file, output_file);
    if(code)
        printf("Male male\n");

    fclose(input_file);
    fclose(output_file);

    *dictionary = importDictionary(out_name);

    remove(out_name);

    return code;
}

static int memory_encode_file(FILE *in, FILE *out)
{
    unsigned char *buf = NULL, *bufout = NULL;
    unsigned int len = 0, cur = 0, inc = 1024, bufoutlen = 0;

    /* Read the file into memory. */
    while(!feof(in))
    {
        unsigned char *tmp;
        len += inc;
        tmp = (unsigned char*)realloc(buf, len);
        if(!tmp)
        {
            if(buf)
                free(buf);
            return 1;
        }

        buf = tmp;
        cur += fread(buf + cur, 1, inc, in);
    }

    if(!buf)
        return 1;

    /* Encode the memory. */
    if(huffman_encode_memory(buf, cur, &bufout, &bufoutlen))
    {
        free(buf);
        return 1;
    }

    free(buf);

    /* Write the memory to the file. */
    if(fwrite(bufout, 1, bufoutlen, out) != bufoutlen)
    {
        free(bufout);
        return 1;
    }

    free(bufout);

    return 0;
}

static int memory_decode_file(FILE *in, FILE *out)
{
    unsigned char *buf = NULL, *bufout = NULL;
    unsigned int len = 0, cur = 0, inc = 1024, bufoutlen = 0;

    /* Read the file into memory. */
    while(!feof(in))
    {
        unsigned char *tmp;
        len += inc;
        tmp = (unsigned char*)realloc(buf, len);
        if(!tmp)
        {
            if(buf)
                free(buf);
            return 1;
        }

        buf = tmp;
        cur += fread(buf + cur, 1, inc, in);
    }

    if(!buf)
        return 1;

    /* Decode the memory. */
    if(huffman_decode_memory(buf, cur, &bufout, &bufoutlen))
    {
        free(buf);
        return 1;
    }

    free(buf);

    /* Write the memory to the file. */
    if(fwrite(bufout, 1, bufoutlen, out) != bufoutlen)
    {
        free(bufout);
        return 1;
    }

    free(bufout);

    return 0;
}

