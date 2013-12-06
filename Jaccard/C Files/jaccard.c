#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include "..\..\List-Library\Header Files\List.h"

#define true 1
#define false 0
#define tblSize 104729L

int hash(void* v, unsigned int length);
int hashString(char *text);
void interpretConsoleFlags(int argc, char* argv[], List* inputFileNames);
char* readTextFile(const char* fileName);
List* tokenizeString(char* inputString, const char* delimiters);
void printDebug(char* debugText, ...); /**Wraps printf(), calling it only if global variable debugFlag is true**/
List* hashList(List* itemList);
List* unionLists(List* list1, List* list2);
List* intersectLists(List* list1, List* list2);
void removeDuplicates(List* inputList);

/**
 * Declare any (absolutely necessary) global variables:
 */
int debugFlag = false;

int main(int argc, char* argv[]) {
	/**Read the input file names from the arguments:**/
	List* inputFileNames = createList(ARRAY_LIST);
	interpretConsoleFlags(argc, argv, inputFileNames);
	
	/**Load the text content corresponding to the read file names:**/
	List** inputFileText = malloc(listSize(inputFileNames) * sizeof(List*)); //Array of List*
	for(int i = 0; i < listSize(inputFileNames); i++) {
		inputFileText[i] = tokenizeString(readTextFile(getFromList(inputFileNames, i)), ",");
	}
	
	/**File and gram information**/
	printDebug("\nInput file names are:\n");
	for(int i = 0; i < listSize(inputFileNames); i++) {
		printDebug("   %s\n", getFromList(inputFileNames, i));
	}
	printDebug("\n");
	for(int i = 0; i < listSize(inputFileNames); i++) {
		printDebug("There are %d n-grams in file \"%s\"\n", listSize(inputFileText[i]), getFromList(inputFileNames, i));
	}
	
	/**Replace each List in inputFileText with its hashed version:**/
	printDebug("\n");
	List* temp = NULL;
	for(int i = 0; i < listSize(inputFileNames); i++) {
		printDebug("Hashing contents of \"%s\"\n", getFromList(inputFileNames, i));
		temp = hashList(inputFileText[i]);
		deleteList(inputFileText[i]);
		inputFileText[i] = temp;
		printDebug("  Successfully hashed.\n");
	}
	
	/**Generate an Intersection and Union list between each pair of files, and find the Jaccard similarity for each:**/
	List* intersectedList = NULL;
	List* unionedList = NULL;
	double listSimilarity = 0.0;
	for(int i = 0; i < listSize(inputFileNames) - 1; i++) {
		for(int k = i + 1; k < listSize(inputFileNames); k++) {
			unionedList = unionLists(inputFileText[i], inputFileText[k]);
			intersectedList = intersectLists(inputFileText[i], inputFileText[k]);
			
			listSimilarity = (double)listSize(intersectedList) / (double)listSize(unionedList);
			printf("\nComparing files \"%s\" and \"%s\":\n", getFromList(inputFileNames, i), getFromList(inputFileNames, k));
			printDebug("  The intersection results in %d n-grams\n", listSize(intersectedList));
			printDebug("  The union results in %d n-grams\n", listSize(unionedList));
			printf("  Files \"%s\" and \"%s\" are %.2f%% similar.\n", getFromList(inputFileNames, i), getFromList(inputFileNames, k), listSimilarity * 100);
			
			deleteList(unionedList);
			deleteList(intersectedList);
		}
	}
	
	printDebug("\nFreeing memory...\n");
	
	for(int i = 0; i < listSize(inputFileNames); i++) {
		free(inputFileText[i]);
	}
	free(inputFileText);
	deleteList(inputFileNames);
	printDebug("  Memory freed successfully.\n");
	
	return 0;
}

/**
 * Expanded version of the hash function. Hashes the number of bytes specified,
 * starting at address v. Used for hashing generic data other than strings.
 */
int hash(void* v, unsigned int length) {
	int h = 0, a = 31, count = 0;
	if(v == NULL) {
		printf("\nWARNING: Tried to hash a NULL pointer!\n");
		return 0;
	}
	
	while(count < length) {
		h = (a * h + *((char*)v)) % tblSize;
		v++;
		count++;
	}
	return h;
}

int hashString(char *text) {
	int h, a = 31;
	for (h = 0; *text != '\0'; text++) {
		h = (a * h + *text) % tblSize;
	}
	return h;
}

void interpretConsoleFlags(int argc, char* argv[], List* inputFileNames) {
	if(argc > 1) {
		int gatheringInput = true; //If true, all subsequent unrecognized (not a flag) arguments are assumed to be input file names
		for(int i = 1; i < argc; i++) {
			/*if(strcmp(argv[i], "-help") == 0) { //Didn't have time to write a help paragraph
				printHelpText();
				exit(0);
			} else */if(strcmp(argv[i], "-i") == 0) { //Start reading a stream of file names, stopping at the next argument flag
				gatheringInput = true;
			} else if(strcmp(argv[i], "-d") == 0) { //If the user enabled the verbose debug messages
				debugFlag = true;
				gatheringInput = false;
			} else if(gatheringInput == true) {
				addToList(inputFileNames, argv[i]);
			}
		}
	}
}

char* readTextFile(const char* fileName) {
	FILE* textFile = NULL;
	int fileLength = 0; //In bytes
	char* textBuffer = NULL;
	
	textFile = fopen(fileName, "rb");
	if (textFile == NULL) {
		printf("\nERROR: File \"%s\" not found!\n", fileName);
		exit(1);
	}
	
	fseek(textFile, 0, SEEK_END); //Set the file position indicator to the end of the file
	fileLength = ftell(textFile); //get the value of the file position indicator, which now represents the length of the file
	
	printDebug("\n >Reading from file \"%s\"\n", fileName); //<DEBUG>
	printDebug("The file is %d bytes long.\n", fileLength); //<DEBUG>
	
	textBuffer = malloc(fileLength * sizeof(char) + 1); //Allocate enough buffer space for the text file, plus a terminating zero
	if (textBuffer == NULL) {
		fclose(textFile);
		exit(1);
	}
	
	fseek(textFile, 0, SEEK_SET); //Set the file position indicator back to the beginning of the file
	fread(textBuffer, sizeof(char), fileLength, textFile);
	textBuffer[fileLength] = '\0';
	
	fclose(textFile);
	printDebug("File \"%s\" read successfully.\n", fileName);
	
	return textBuffer;
}

List* tokenizeString(char* inputString, const char* delimiters) {
	List* outputStorage = createList(ARRAY_LIST);
	char* temp = strtok(inputString, delimiters); //strtok returns a pointer to a position *INSIDE* inputString! Not a new copy!
	char* tempCopy = malloc(sizeof(char) * strlen(temp) + 1); //Therefore, we need to make a new copy if we want to treat inputString as immutable
	strcpy(tempCopy, temp);
	addToList(outputStorage, tempCopy);
	
	while((temp = strtok(NULL, delimiters)) != NULL) {
		tempCopy = malloc(sizeof(char) * strlen(temp) + 1);
		strcpy(tempCopy, temp);
		addToList(outputStorage, tempCopy);
	}
	return outputStorage;
}

void printDebug(char* debugText, ...) { /**Wraps printf(), calling it only if global variable debugFlag is true**/
	if(debugFlag == true) {
		va_list args; //Set up our variable argument's data structure
		va_start(args, debugText); //The variable argument "starts" right after our last finite argument, which is *debugText
		
		char* character = "%";
		
		char* subString = NULL;
		char* position = debugText; //This variable will point to characters in the middle of debugText, serving as the start of any remaining unprocessed text
		int subStrLength = 0;
		
		while(position[0] != '\0') {
			//Get the length of the string preceding the character '%' if there is one, else the total length remaining:
			subStrLength = strcspn(position, character);
			
			//Copy the contents of the sub-string we just found into a buffer, which will be printed later
			subString = malloc(sizeof(char) * subStrLength + 1);
			strncpy(subString, position, subStrLength);
			subString[subStrLength] = '\0';
			
			//Point position to the first character after the sub-string we just found:
			position += subStrLength;
			
			printf(subString);
			
			//If a print directive is present, determine its type, print the appropriate variable argument, and consume the directive:
			if(position[0] == '%') { 
				if(position[1] == 'c') {
					printf("%c", va_arg(args, unsigned int)); //For %c
					position += 2;
				} else if(position[1] == 's') {
					printf("%s", va_arg(args, unsigned int)); //For %s
					position += 2;
				} else if(position[1] == 'd') {
					printf("%d", va_arg(args, signed int*)); //For %d
					position += 2;
				} else if(position[1] == 'p') {
					printf("%p", va_arg(args, void*)); //For %p
					position += 2;
				} else {//This accounts for an edge case where a string contains a % with no valid following type character
					position++; //Skip the lone %
				}
			}
		}
		
		va_end(args); //Clean up the variable argument's data structure
	}
}

List* hashList(List* itemList) {
	List* hashedList = createList(ARRAY_LIST);
	int* hashValue = NULL;
	for(int i = 0; i < listSize(itemList); i++) {
		//We cannot store the returned value of hashString() directly, because it has no memory address (it's in a CPU register)
		hashValue = malloc(sizeof(int));
		*hashValue = hashString(getFromList(itemList, i)); //Copy the value from the register to a newly-created location on the heap
		addToList(hashedList, hashValue);
	}
	return hashedList;
}

List* unionLists(List* list1, List* list2) {
	List* unionedList = createList(ARRAY_LIST);
	for(int i = 0; i < listSize(list1); i++) { //Add all elements in list1 to unionedList
		int* temp = malloc(sizeof(int));
		*temp = *((int*)getFromList(list1, i));
		addToList(unionedList, temp);
	}
	for(int i = 0; i < listSize(list2); i++) { //Add all elements in list2 to unionedList
		int* temp = malloc(sizeof(int));
		*temp = *((int*)getFromList(list2, i));
		addToList(unionedList, temp);
	}
	removeDuplicates(unionedList); //Remove any duplicates that were likely introduced
	return unionedList;
}

List* intersectLists(List* list1, List* list2) {
	List* intersectedList = createList(ARRAY_LIST);
	for(int i = 0; i < listSize(list1); i++) { //For each element in the first list:
		for(int k = 0; k < listSize(list2); k++) { //Compare it to each element in the second list:
			if(*((int*)getFromList(list1, i)) == *((int*)getFromList(list2, k))) { //If a match is found, add it to intersectedList (including duplicates)
				int* temp = malloc(sizeof(int));
				*temp = *((int*)getFromList(list1, i));
				addToList(intersectedList, temp);
			}
		}
	}
	removeDuplicates(intersectedList);
	return intersectedList;
}

void removeDuplicates(List* inputList) {
	int* currentItem = NULL;
	int i, k;
	
	for(i = 0; i < listSize(inputList); i++) {
		currentItem = getFromList(inputList, i);
		for(k = i + 1; k < listSize(inputList); k++) { //Iterate through every element after currentItem
			if(*currentItem == *((int*)getFromList(inputList, k))) { //If an element is found that is identical to currentItem, it needs to be removed
				removeFromList(inputList, k);
				k--; //The contents of the list have shifted left, re-check this index
			}
		}
	}
}
