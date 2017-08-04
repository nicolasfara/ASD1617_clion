//
// Created by nicolasfarabegoli on 8/4/17.
//

#include "utility.h"

unsigned short count_find = 0;

/*char *find_index_word(NODO *n, int index, int *counter_pt) {
	if (n->word == NULL)										//CASO BASE (SENTINELLA)
		return NULL;

	char *result = find_index_word(n->left, index, counter_pt);	//CONTROLLO IL FIGLIO SINISTRO
	if (result != NULL)											//SE HO TROVATO LA PAROLA LA RESTITUISCO
		return result;

	if (*counter_pt == index)									//SE SONO ALLA I-ESIMA PAROLA (PARTENDO A CONTARE DA 1)
		return n->word;											//LA RESTITUISCO
	(*counter_pt)++;
	return find_index_word(n->right, index, counter_pt);		//ALTRIMENTI RESTITUISCO  QUELLO CHE MI PASSA IL FIGLIO DESTRO
}*/

int levenshtein(const char *s, int ls, const char *t, int lt) {
    if (!ls)
        return lt;
    if (!lt)
        return ls;

    if (s[ls] == t[ls])
        return levenshtein(s, ls - 1, t, lt - 1);		//mi sposto sui vari caratteri delle parole

    int a = levenshtein(s, ls - 1, t, lt - 1);
    int b = levenshtein(s, ls, t, lt - 1);
    int c = levenshtein(s, ls - 1, t, lt);

    if (a > b) a = b;									//prendo la pi� simile
    if (a > c) a = c;

    return a + 1;
}

unsigned char convert_accent(unsigned char c) {
    if ((c >= 192 && c <= 198) || (c >= 224 && c <= 230))	//trasformo gli accenti in caratteri
        return 'a';
    if ((c >= 200 && c <= 203) || (c >= 232 && c <= 235))
        return 'e';
    if ((c >= 204 && c <= 207) || (c >= 236 && c <= 239))
        return 'i';
    if ((c >= 210 && c <= 214) || c == 216 || c == 240 || (c >= 242 && c <= 248))
        return 'o';
    if ((c >= 217 && c <= 220) || (c >= 249 && c <= 252))
        return 'u';
    if (c == 223)
        return 'b';
    if (c == 199 || c == 231)
        return 'c';
    if (c == 208)
        return 'd';
    if (c == 209 || c == 241)
        return 'n';
    if (c == 222)
        return 'p';
    if (c == 215)
        return 'x';
    if (c == 221)
        return 'y';

    return ' '; //NEVER HERE
}


short readWordDef(FILE* f, char* word, char* def, bool* endFile) {

    char rchar; //simple sentinel for find EOF
    //Check if the argumets are omogeneous
    if (f == NULL || word == NULL || def == NULL)
        return -1; //bad

    fscanf(f, "%s", word); //read the line
    removeChar(word, '\"', ':');

    fscanf(f, "%[^\"]s", def); //read the line
    removeChar(def, '[', ']'); //remove [] character
    removeChar(def, '\n', ' ');
    removeChar(def, '\r', '\r');
    //detect the EOF
    if ((rchar = getc(f)) == EOF)
        *endFile = true;

    return 0; //ok

}

void removeChar(char* str, char garbage, char garbage2) {
    char *src, *dst; //to strings pointer
    for (src = dst = str; *src != '\0'; src++) {
        *dst = *src; //copy char form src to dst
        if (*dst != garbage && *dst != garbage2) dst++; //not find garbage character
    }
    *dst = '\0'; //ad end string
}



/*
*RETURN 0: n2 is grater than n1
*RETURN 1: n1 is grater than n2
*RETURN 2: n1 is n2
*RETURN 3: n1-> NULL or n2-> NULL
*/
unsigned short alphabeticalOrder(char* n1, char* n2) {

    if (n1 == NULL || n2 == NULL)
        return 3;
    //Check character
    for (int i = 0; i < MAX_WORD; i++) {
        if (n1[i] < n2[i])
            return 0;
        else if (n1[i] > n2[i])
            return 1;
    }

    return 2;
}

int distance(char *source, char *target) {

    if((strlen(source) == 0) || (strlen(target) == 0)) return 0;
    if(source == NULL || target == NULL) return 0;
    if(!strcmp(source, target)) return (int)strlen(target);

    unsigned int sourceWordCount = (unsigned int)strlen(source);
    unsigned int targetWordCount = (unsigned int)strlen(target);

    int distance[sourceWordCount + 1][targetWordCount + 1];

    for (int i = 0; i <= sourceWordCount ; distance[i][0] = i++);
    for (int j = 0; j <= targetWordCount ; distance[0][j] = j++);

    for (int i = 1; i <= sourceWordCount ; i++) {
        for (int j = 1; j <= targetWordCount ; j++) {
            int cost = (target[j - 1] == source[i - 1]) ? 0 : 1;

            distance[i][j] = (int)fmin((float)fmin((float)distance[i - 1][j] + 1, (float)distance[i][j - 1] + 1), distance[i - 1][j - 1] + cost);

        }

    }

    return distance[sourceWordCount][targetWordCount];
}

double CalculateSimilarity(char *source, char *target) {

    if(source == NULL || target == NULL) return 0.0;
    if(!strlen(source) ||!strlen(target)) return 0.0;
    if(!strcmp(source, target)) return 1.0;

    int stepsToSame = distance(source, target);
    if(stepsToSame < 0)
        return 0.0;
    return (1.0f - ((double)stepsToSame / fmax((double)strlen(source), (double)strlen(target))));
}