// invertedIndex.c - Generate an inverted index of words from urls
// Written by Kongwei Ying, September 2017
// Modified by Rahil Agrawal, Sept-Oct 2017
#include "invertedIndex.h"

int main(int argc, char *argv[]) {
	Queue collectionUrls = getCollectionUrls();
	getInvertedIndex(collectionUrls);
	disposeQueue(collectionUrls);
	return EXIT_SUCCESS;
}

// Generate InvertedIndex BST from URLs
Tree getInvertedIndex(Queue collectionUrls) {
	Tree t = newTree();
	char *url_from;
	char url_from_location[MAX_CHAR] = {0};
	char curr_word[MAX_CHAR] = {0};
	while (!emptyQueue(collectionUrls)){
		url_from = leaveQueue(collectionUrls);
		strcpy(url_from_location, url_from);
		strcat(url_from_location, ".txt");
		FILE *nextUrlFp = fopen(url_from_location, "r");
		if (nextUrlFp == NULL){
			fprintf(stderr, "Couldn't open %s\n", url_from);
			continue;
		}
		while (fscanf(nextUrlFp, "%s", curr_word) != EOF){
			// Only look for words in Section-2
			if (strcmp(curr_word, "#start") == 0){
				fscanf(nextUrlFp, "%s", curr_word);
				continue;
			}
			else if (strncmp(curr_word, "url", 3) == 0) continue;
			else if (strcmp(curr_word, "#end") == 0){
				fscanf(nextUrlFp, "%s", curr_word);
				continue;
			}

			// Strip words of punctuation
			normalize(curr_word);
			// Adds curr_word to the tree
			insertIntoTree(t, curr_word);
			// Adds the url_from into the list of urls for curr_word
			addUrl(t, curr_word, url_from);
		}
		free(url_from);
		fclose(nextUrlFp);
	}
	FILE *invertedIndexFp = fopen("invertedIndex.txt", "w");
	if (invertedIndexFp == NULL){
		printf("Error, could not open file");
	}
	else {
		// Print the inverted index tree into invertedIndex.txt
		printInOrder(t, invertedIndexFp);
		fclose(invertedIndexFp);
	}
	dropTree(t);
	return t;
}