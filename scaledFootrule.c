// Scaled footrule main
// Written by Kongwei Ying, September 2017

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "scaledFootrule.h"
#include "hungarian.h"

struct UrlRep {
    char url[MAX_CHAR];
} UrlRep;

int main(int argc, char *argv[]) {
    // A 2d array to hold different ranking list rankings
    Url rankArrays[MAX_LISTS][N_ROWS];
    for (int i = 0; i < MAX_LISTS; i++) {
        for (int j = 0; j < N_ROWS; j++) {
            rankArrays[i][j] = newUrl();
        }
    }
    int maxRows = 0; // Determined by the no. entries in a rank file
    if(!argsOk(argc, argv)) return EXIT_FAILURE;

    for (int i = 0; i < (argc - 1); i++) {
        readRankFile(argv[i + 1], rankArrays[i], &maxRows);
        // printf("Ranking list %d\n", i);
        // for (int j = 0; j < maxRows; j++) {
        //     printf("%s ", rankArrays[i][j]->url);
        // }
        // printf("\n");
    }
    
    // Set of urls, a union of all the lists
    Url urlSet[MAX_LISTS * N_ROWS];
    for (int i = 0; i < MAX_LISTS * N_ROWS; i++) urlSet[i] = newUrl();
    int nElems = 0;
    for (int i = 0; i < (argc - 1); i++) {
        for (int j = 0; j < maxRows; j++) {
            insertSetArray(urlSet, &nElems, MAX_LISTS * N_ROWS, rankArrays[i][j]);
        }
    }

    // Sort set
    qsort(urlSet, nElems, sizeof(Url), cmpString);
    
    // Print set
    // printf("Set of urls:\n");
    // for (int i = 0; i < nElems; i++) {
    //     printf("%s ", urlSet[i]->url);
    // }
    // printf("\n");

    double **costMatrix = (double **)calloc(nElems, sizeof(double *));
	for (int i = 0; i < nElems; i++) {
		costMatrix[i] = (double*)calloc(nElems, sizeof(double));
    }
    // For each url
    for (int y = 0; y < nElems; y++) {
        // For each position of that url
        for (int x = 0; x < nElems; x++) {
            // For each list
            for (int i = 0; i < (argc - 1); i++) {
                int tc = findUrlPositionInRankList(rankArrays[i], urlSet[y], nElems);
                int ti = findRankListSize(rankArrays[i], nElems);
                // printf("%d|", ti);
                costMatrix[y][x] += scaledFootruleDistance(nElems, x + 1, tc, ti);
                // printf("%s is rank %d in ranklist %d which has size %d and position %d\n", urlSet[y]->url, tc, i, ti, x + 1);
            }
            // printf("%.2lf ", costMatrix[y][x]);
        }
        // printf("\n");
    }

	//initialise hungarian
	hProblem p = newHungarian(costMatrix, nElems);

	//print cost matrix
	// hungarian_print_costmatrix(p);

	//solve
	printf("%.6lf\n", solveHungarian(p));

	//print assignment
	// printf("Assignment:\n");
    // hungarian_print_assignment(p);
    
    for (int i = 0; i < nElems; i++) {
        printf("%s\n", urlSet[getUrlFromRank(p, i, nElems)]->url);
    }

    disposeHungarian(p);
    
    // Free urlSet
    for (int i = 0; i < MAX_LISTS * N_ROWS; i++) free(urlSet[i]);

    // Free costMatrix
	for (int i = 0; i < nElems; i++) {
		free(costMatrix[i]);
    }
    free(costMatrix);

    // Free rankArrays
    for (int i = 0; i < MAX_LISTS; i++) {
        for (int j = 0; j < N_ROWS; j++) {
            free(rankArrays[i][j]);
        }
    }
    return EXIT_SUCCESS;
}

Url newUrl() {
    Url url = malloc(sizeof(struct UrlRep));
    memset(url->url, 0, MAX_CHAR);
    return url;
}

// Reads the ranked lists of urls into an array
void readRankFile(char *filename, Url *array, int *maxRows) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("File opening failed\n");
        exit(1);
    }
    int numRows = 0;
    int i = 0;
    while (1) {
        char buf[MAX_CHAR] = {0};
        numRows = i;
        if (fscanf(fp, "%s\n", buf) == EOF) break;
        strcpy(array[i]->url, buf);
        i++;
    }
    fclose(fp);
    if (numRows > *maxRows) {
        *maxRows = numRows;
    }
}

void insertSetArray(Url *array, int *nElems, int size, Url elem) {
    for (int i = 0; i < size; i++) {
        if (strcmp(array[i]->url, elem->url) == 0) return;
    }
    strcpy(array[*nElems]->url, elem->url);
    (*nElems)++;
}

int cmpString(const void *a, const void *b) {
    Url pa = *(Url *)a;
    Url pb = *(Url *)b;
    return strcmp(pa->url, pb->url);
 }

// Checks if correct number of commandline arguments are given
int argsOk(int argc, char *argv[]) {
    if (argc == 1 || argc == 2) {
        printf("Usage: ./scaledFootrule file1 file2\n");
        return 0;
    }
    else
        return 1;
}

int findRankListSize(Url rankArray[], int maxUrls) {
    int i;
    for (i = 0; i < maxUrls; i++) {
        if (rankArray[i]->url[0] == 0) break;
    }
    return i;
}

int findUrlPositionInRankList(Url rankArray[], Url url, int maxUrls) {
    int i;
    for (i = 0; i < maxUrls; i++) {
        // This means there's no such url in this rankArray
        if (rankArray[i]->url[0] == 0) return 0;
        if (strcmp(rankArray[i]->url, url->url) == 0) break;
    }
    return i + 1;
}

double scaledFootruleDistance(int n, int p, int tc, int ti) {
    double sfd = 0.0;
    if (tc == 0) return sfd;
    if (ti == 0 || n == 0) printf("NAN\n");
    sfd = fabs(((double)tc / (double)ti) - ((double)p / (double)n));
    return sfd;
}