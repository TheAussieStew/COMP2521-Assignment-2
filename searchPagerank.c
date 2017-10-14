//
//
// Modified by Rahil Agrawal, September
#include "searchPagerank.h"
#include "colours.h"
#include "pagerank.h"
#include "readData.h"
#include "list.h"

// Finds urls from invertedIndex that contain query words
// Fills matchedUrlList with urls
// Returns number of queries entered
int findMatchedUrls(char matchedUrlList[MAX_V][MAX_CHAR]) {
    FILE *invertedIndexFp = fopen("invertedIndex.txt", "r");
    if (invertedIndexFp == NULL) {
        perror("Error opening file");
        exit(1);
    }
    char queries[MAX_QUERIES][MAX_CHAR] = {{0}};
    char buf[MAX_CHAR] = {0};
    int nQueries = 0;
    int nMatchedUrls = 0;

    // Scan queries
    printf(RED "\n\n               Welcome to Yaggle. Enter your query.\n" RESET);
    fgets(buf, MAX_CHAR, stdin);
    char *query = NULL;
    query = strtok(buf, " ");
    for (int i = 0; query != NULL && i < MAX_QUERIES; i++) {
        // Remove non-char characters from the string
        if (sscanf(query, "%s", query) == EOF) break;
        strcpy(queries[i], query);
        nQueries++;
        // printf("'%s;\n", queries[i]);
        query = strtok(NULL, " ");
    }
    while (1) {
        char invertedIndexStr[MAX_CHAR] = {0};
        // Scan every word of the invertedIndex
        if (fscanf(invertedIndexFp, "%s", invertedIndexStr) == EOF)
            break;
        // If the query matches the word in the index, load the word's urls
        for (int i = 0; i < nQueries; i++) {
            if (strcmp(invertedIndexStr, queries[i]) == 0) {
                // Read the rest of the line into buf
                memset(buf, 0, MAX_CHAR);
                fgets(buf, MAX_CHAR, invertedIndexFp);
                // Then extract the strings and place them into matchedUrlList
                char *url = NULL;
                url = strtok(buf, " ");
                for (int j = nMatchedUrls; url != NULL && j < MAX_QUERIES; j++) {
                    // Remove non-char characters such as new lines from the string
                    if (sscanf(url, "%s", url) == EOF) break;
                    strcpy(matchedUrlList[j], url);
                    nMatchedUrls++;
                    // printf("%d\n", nMatchedUrls);
                    // printf("%s\n", matchedUrlList[j]);
                    url = strtok(NULL, " ");
                }
            }
        }
    }
    fclose(invertedIndexFp);
    return nQueries;
}

// Sorts matchedUrlList according to pagerankList
// Returns a string array of urls
void findPagerank(int nQueries, char matchedUrlList[MAX_V][MAX_CHAR]) {
    // An array of urls indexed by frequency of matching query words
    List *queryTable = malloc(sizeof(List) * nQueries);
    for (int i = 0; i < nQueries; i++) {
        queryTable[i] = newList();
    }
    for (int i = 0; *matchedUrlList[i]; i++) {
        int count = 1;
        if (strcmp(matchedUrlList[i], "-1") == 0) continue;
        for (int j = 0; *matchedUrlList[j]; j++) {
            if (i != j && strcmp(matchedUrlList[i], matchedUrlList[j]) == 0 && strcmp(matchedUrlList[j], "-1") != 0) {
                strcpy(matchedUrlList[j], "-1");
                count++;
            }
        }
        if (count > 0)
            appendList(queryTable[count - 1], matchedUrlList[i]);
    }
    int nResults = 30;
    for (int i = nQueries - 1; i >= 0; i--) {
        // printf("Urls with %d query: ", i + 1);
        sortList(queryTable[i], cmpPagerank);
        showList(queryTable[i], stdout, '\n', &nResults);
    }
}
