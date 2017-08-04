//
// Created by nicolasfarabegoli on 8/4/17.
//

#include "huffman.h"

int letter_frequencies[31] = { 81, 15, 28, 43, 128, 23, 20, 61, 71, 2, 1, 40, 24, 69, 76,
        //  P   Q  R   S   T   U   V   W   X   Y  Z  SPC NULL NL  '  EOF
                                  20, 1, 61, 64, 91, 28, 10, 24, 1, 20, 1, 130, 80, 80, 5, 80};

HNode *build_huffman_tree() {
    HNode *nodes_head = allocates_node(0);				//ALLOCO IL PRIMO NODO DELLA LISTA
    HNode *temp = nodes_head;

    if (temp == NULL)									//CONTROLLO L'ALLOCAZIONE
        return NULL;

    temp->frequencies = letter_frequencies[0];			//FREQUENZA DELLA LETTERA (CONTENUTA NELL'ARRAY)

    for (int i = 1; i < ELEMENTS; i++) {				//PER OGNI LETTERA DOPO LA PRIMA, ALLOCO UN NODO (CREO UNA LISTA)
        temp->next = allocates_node(i);					//ALLOCO UN NUOVO NODO E LO FACCIO PUNTARE DAL NEXT DEL PRECEDENTE
        temp = temp->next;								//SPOSTO LA VAR TEMP AL NODO APPENA CREATO (O A NULL IN CASO D'ERRORE)
        if (temp == NULL)								//CONTROLLO L'ALLOCAZIONE
            return NULL;
        temp->frequencies = letter_frequencies[i];		//ASSOCIO LA FREQUENZA DELLA LETTERA CONTENUTA NELL'ARRAY ALLA VARIABILE NEL NODO
    }

    while (nodes_head->next != NULL) {					//QUANDO HO ALTRI NODI OLTRE LA "RADICE" (SE NON CI SON STATI ERRORI PRECEDENTI CI SARA' SEMPRE ALMENO UN NODO; IN CASO CONTRARIO AVREBBE RESTITUITO NULL)
        temp = allocates_node(127);						//ALLOCO UN NODO CHE NON CORRISPONDE A NESSUNA LETTERA
        if (temp == NULL)								//CONTROLLO L'ALLOCAZIONE
            return NULL;
        temp->left = extract_smaller_one(&nodes_head);	//ESTRAGGO IL NODO CON LETTERA DI FREQUENZA MINORE DALLA LISTA E LO FACCIO PUNTARE DAL FIGLIO SINISTRO DEL NUOVO NODO
        temp->right = extract_smaller_one(&nodes_head);	//ESTRAGGO IL NODO CON LETTERA DI FREQUENZA MINORE DALLA LISTA E LO FACCIO PUNTARE DAL FIGLIO DESTRO DEL NUOVO NODO
        temp->frequencies = temp->left->frequencies + temp->right->frequencies;	//LA FREQUENZA EQUIVALE ALLA SOMMA DELLE DUE
        temp->next = nodes_head;						//INSERISCO IL NODO NELLA LISTA
        nodes_head = temp;
    }
    return nodes_head;									//RITORNO LA TESTA DELLA LISTA CHE ORA CORRISPONDERA' ALLA RADICE DELL'ALBERO DI HUFFMAN
}

HNode *allocates_node(int i) {
    HNode *node = (HNode *)malloc(sizeof(HNode));		//ALLOCO IL NODO
    if (node == NULL)									//CONTROLLO L'ALLOCAZIONE
        return NULL;

    node->letter = (char)i;									//INDICE DELL'ARRAY CORRISPONDENTE ALLA LETTERA
    node->left = node->right = node->next = NULL;

    return node;										//PUNTATORE AL NODO
}

HNode *extract_smaller_one(HNode **nodes_head) {
    HNode *minimum = *nodes_head;
    HNode *scrolling = *nodes_head, *previous = NULL;

    if (*nodes_head == NULL)							//SE LA LISTA NON CONTIENE ELEMENTI
        return NULL;

    if ((*nodes_head)->next == NULL) {					//SE CONTIENE SOLO UN ELEMENTO
        *nodes_head = NULL;								//SVUOTO LA LISTA
        return minimum;									//RITORNO LA RADICE DELL'ALBERO
    }

    while (scrolling->next != NULL) {					//SCORRO LA LISTA
        if (scrolling->next->frequencies < minimum->frequencies) {	//SE TROVO UN ELEMENTO DI FREQUENZA MINORE
            previous = scrolling;						//TENGO IN MEMORIA IL NODO PRECEDENTE AL MINIMO
            minimum = scrolling->next;					//AGGIORNO IL MINIMO
        }
        scrolling = scrolling->next;					//VADO ALL'ELEMENTO SUCCESSIVO DELLA LISTA
    }
    if (previous == NULL) {								//SE L'ELEMENTO ERA IL PRIMO DELLA LISTA
        *nodes_head = minimum->next;					//SPOSTO LA TESTA ALL'ELEMENTO SUCCESSIVO
        minimum->next = NULL;							//SCOLLEGO IL PUNTATORE AL SUCCESSIVO DEL MINIMO
    }
    else {												//SE IL MINIMO NON ERA IL PRIMO DELLA LISTA
        previous->next = minimum->next;					//IL PUNTATORE NEXT DEL NODO PRECEDENTE PUNTA AL SUCCESSIVO DI MINIMO
        minimum->next = NULL;							//SCOLLEGO IL PUNTATORE NEXT DEL MINIMO
    }
    return minimum;										//RESTITUISCO L'INDIRIZZO DEL NODO CON FREQUENZA MINORE
}

void fill_table(unsigned int *code_table, HNode *tree_node, unsigned int code) {
    if (tree_node->letter != 127)									//SE SIAMO ALLA FOGLIA (C'E' UN VALORE != DA 127)
        code_table[(int)tree_node->letter] = code;					//"CODE" HA ASSUNTO IL VALORE "BINARIO" DEL PERCORSO DA RADICE->FOGLIA
    else {															//SE NON SONO ANCORA GIUNTO ALLA FOGLIA
        if ((code % 10) == 1 || (code % 10) == 2) {
            fill_table(code_table, tree_node->left, code + 3);		//SE VADO A SX IL RAMO HA VALORE 0(1) - IN QUESTO CASO E' STATO MESSO +3 PER EVITARE CHE L'INT RAGGIUNGESSE I 10 MILIARDI
            fill_table(code_table, tree_node->right, code + 5);		//SE VADO A DX IL RAMO HA VALORE 1(2) - IN QUESTO CASO E' STATO MESSO +5 PER EVITARE CHE L'INT RAGGIUNGESSE I 10 MILIARDI
        }
        else {
            fill_table(code_table, tree_node->left, code * 10 + 1);	//SE VADO A SX IL RAMO HA VALORE 0(1)
            fill_table(code_table, tree_node->right, code * 10 + 2);//SE VADO A DX IL RAMO HA VALORE 1(2)
        }
    }

    return;
}

int compress_node(NODO * n, FILE * output, unsigned int *code_table) {
    if (n == NULL || n->word == NULL)
        return -1;
    compress_node(n->left, output, code_table);			//compressione in order
    compress_string(n->word, output, code_table);
    compress_string(n->def, output, code_table);
    compress_node(n->right, output, code_table);

    return 0;
}

void compress_string(char *n_string, FILE *output, unsigned int *code_table) {
    char bit = 0, c = 0, byte = 0;										//1 BYTE
    unsigned int code = 0, lenght = 0, i = 0, bits_left = 8;

    do{
        if (n_string == NULL) {
            lenght = (unsigned int)log10((code_table[27]) + 1);			//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
            code = code_table[27];								//CODIFICA BIN
        }
        else {
            c = n_string[i];

            if (c >= 97 && c <= 122) {							//LETTERE MINUSCOLE
                lenght = (unsigned int)log10((code_table[c - 97]) + 1);	//LENGHT = LUNGHEZZA CODIFICA (CODIFICA ASCII DELLE LETTERE - 97 = INDICE LETTERA NELLA TABELLA)
                code = code_table[c - 97];						//CODIFICA "BINARIA" DELLA LETTERA
            }
            if (c == 32) {										//CARATTERE SPAZIO
                lenght = (unsigned int)log10((code_table[26]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
                code = code_table[26];							//CODIFICA BIN
            }
            if (c == 0) {										//CARATTERE NULL
                lenght = (unsigned int)log10((code_table[27]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
                code = code_table[27];							//CODIFICA BIN
            }
            if (c == 10) {										//CARATTERE "NEW LINE"
                lenght = (unsigned int)log10((code_table[28]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
                code = code_table[28];							//CODIFICA BIN
            }
            if (c == 96) {										//CARATTERE "APOSTROFO"
                lenght = (unsigned int)log10((code_table[29]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
                code = code_table[29];							//CODIFICA BIN
            }
            if (c == 27) {										//CARATTERE "ESCAPE"
                lenght = (unsigned int)log10((code_table[30]) + 1);		//LUNGHEZZA BINARIA DEL NUMERO CODIFICATO
                code = code_table[30];							//CODIFICA BIN
            }
        }
        while (lenght + 1 > 0)									//MI SCORRO TUTTI I "BIT" DELLA CODIFICA
        {
            bit = (char)((code / pow(10, lenght)) - 1);					//PRENDO "BIT PER BIT" LA CODIFICA DELLA LETTERA (-1 PERCHE' GLI 0 = 1 E 1 = 2 NELLA TABELLA)
            switch (bit)
            {
                case 3:
                    bit = 0;
                    code -= (3 * pow(10, lenght));
                    lenght++;
                    break;
                case 4:
                    bit = 1;
                    code -= (4 * pow(10, lenght));
                    lenght++;
                    break;
                case 5:
                    bit = 0;
                    code -= (4 * pow(10, lenght));
                    lenght++;
                    break;
                case 6:
                    bit = 1;
                    code -= (5 * pow(10, lenght));
                    lenght++;

                    break;
                default:
                    code -= (bit + 1) * pow(10, lenght);			//ELIMINO IL BIT VALUTATO
            }
            byte = byte | bit;									//PRENDO LA CODIFIA BINARIA DEL RESTO E LA METTO IN OR CON LE PRECEDENTI
            bits_left--;										//BIT NON "UTILIZZATI"
            lenght--;											//LUNGHEZZA DELLA CODIFICA RIMANENTE DA INSERIRE NEL FILE
            if (bits_left == 0) {								//SE HO RIEMPITO IL BYTE CON LA CODIFICA DELLE VARIE LETTERE
                fputc(byte, output);							//SCRIVO LA CODIFICA
                byte = 0;										//AZZERO X PER POTER "SCRIVERE" IL NUOVO BYTE
                bits_left = 8;									//RESETTO I BYTE "RIMANENTI" DI "BYTE"
            }
            byte = byte << 1;									//AVENDO INSERITO UNA CODIFICA "SHIFTO" I BIT PER NON SOVRASCRIVERLI
        }
        i++;
    } while (n_string != NULL && c != '\0');

    if (bits_left != 8){										//SE CODIFICANDO L'INTERA LINEA NON HO "RIEMPITO" IL BYTE
        byte = byte << (bits_left - 1);							//SHIFTO I BIT A SINISTA IN MODO DA NON AVERE "ZERI" CHE SI FRAPPONGANO FRA L'ULTIMA E LA PRECEDENTE LETTERA
        fputc(byte, output);									//SCRIVO IL BYTE NEL FILE
    }

    return;
}

int decompress_file(FILE *input, NODO **dict_root, HNode *tree) {
    HNode *current = tree;
    register char c;
    register int k = 0;
    char bit;
    int i, end_of_file = 0;

    NODO *nodo_pointer = (NODO *)malloc(sizeof(NODO));
    char *string = (char *)malloc(sizeof(char) * 20);
    if (nodo_pointer == NULL || string == NULL)
        return -1;
    nodo_pointer->word = string;

    if ((c = (char)fgetc(input)) == EOF)
        return -1;

    while (end_of_file != 1) {											//ACQUISISCE CICLICAMENTE I CARATTERI FINCHE' NON ARRIVA ALLA FINE DEL FILE
        for (i = 0; i<8; i++) {											//PER OGNI BIT
            bit = (char)(c & 0x80);												//PRENDE IL PRIMO BIT (AND CON 10000000)
            c = c << 1;													//SHIFTO IL BYTE
            if (bit == 0)												//SE IL BIT "ESTRATTO" E' "ZERO"
                current = current->left;								//MI SPOSTO SUL RAMO SINISTRO DELL'ALBERO
            else
                current = current->right;

            if (current->letter != 127){								//SE SONO GIUNTO AD UNA FOGLIA
                if (current->letter >= 0 && current->letter <= 25) {	//SE E' UNA LETTERA
                    string[k] = (char)(current->letter + 97);
                    k++;
                }
                if (current->letter == 26) {							//SE E' LO SPAZIO
                    string[k] = 32;
                    k++;
                }
                if (current->letter == 27) {							//SE E' NULL
                    if (k != 0)
                        string[k] = '\0';
                    else {
                        free(string);
                        if (string == nodo_pointer->word)
                            nodo_pointer->word = NULL;
                        else
                            nodo_pointer->def = NULL;
                        string = NULL;
                    }
                    if (string == nodo_pointer->word) {
                        string = (char *)malloc(sizeof(char) * 50);
                        if (string == NULL)
                            return -1;
                        nodo_pointer->def = string;
                    }
                    else {
                        insertRBT(dict_root, nodo_pointer);
                        nodo_pointer = (NODO *)malloc(sizeof(NODO));
                        if (nodo_pointer == NULL)
                            return -1;
                        string = (char *)malloc(sizeof(char) * 20);
                        if (string == NULL)
                            return -1;
                        nodo_pointer->word = string;
                    }
                    if (string == NULL)
                        return -1;
                    k = 0;
                    i = 7;
                }
                if (current->letter == 28) {							//SE E' NEW LINE
                    string[k] = 10;
                    k++;
                }
                if (current->letter == 29) {							//SE E' L'APOSTROFO
                    string[k] = 96;
                    k++;
                }														//"STAMPO" NEW LINE NEL FILE
                if (current->letter == ELEMENTS - 1) {					//SE E' L'EOF CREATO DA ME
                    end_of_file = 1;
                }
                current = tree;											//TORNO ALLA RADICE
            }
        }
        c = (char)fgetc(input);
    }
    return 0;
}