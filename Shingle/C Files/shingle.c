#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "..\..\List-Library\Header Files\List.h"

/**
 * Define all constants:
 */
#define true 1
#define false 0

#define defaultShingleSize 2
#define defaultInputFile "input.txt"
#define defaultOutputFile "output.txt"

/**
 * Prototype all functions:
 */
void interpretConsoleFlags(int argc, char* argv[], char** inputFileName, char** outputFileName, int* shingleSize);
void printDebug(char* debugText, ...); /**Wraps printf(), calling it only if global variable debugFlag is true **/
void printHelpText();
char* readTextFile(const char* fileName);
List* tokenizeString(char* inputString, const char* delimiters);
List* computeShingles(List* inputList, int groupSize);
void removeDuplicates(List* inputList);
char* commaDelimitArray(List* inputList);
	int computeSizeOfContents(List* inputList); /** Sub-function of commaDelimitArray to get the number of bytes used by all strings in a given List (minus null-terminators) **/
void writeTextFile(const char* fileName, char* text);

/**
 * Declare any (absolutely necessary) global variables:
 */
int debugFlag = false;

int main(int argc, char* argv[]) {
	/**
	 * Declare all variables and assign them their default values:
	 */
	char* inputFileName = defaultInputFile;
	char* outputFileName = defaultOutputFile;
	
	char* rawText = NULL;
	char* delimitedText = NULL;
	List* tokenList = NULL;
	List* shingleList = NULL;
	
	int shingleSize = defaultShingleSize;
	int i;
	
	/**
	 * Interpret optional command-line flags, modifying the relevant variables as appropriate:
	 */
	interpretConsoleFlags(argc, argv, &inputFileName, &outputFileName, &shingleSize);
	
	printDebug("\n >Debug flag is set, program will print additional debug information.\n");
	
	/**
	 * Read the text file into rawText:
	 */
	printDebug("\n >Opening File \"%s\"...\n", inputFileName);
	rawText = readTextFile(inputFileName);
	printDebug("\n >The text contained in the text file is:\n");
	printDebug("%s\n", rawText);
	
	/**
	 * Tokenize rawText and store it in the List named tokenList:
	 */
	printDebug("\n >Tokenizing the text read from file...\n");
	tokenList = tokenizeString(rawText, " .,\"\n\r()");
	printDebug("Done.\n");
	
	/**
	 * Compute the shingles from tokenList and store them in the List named shingleList:
	 */
	printDebug("\n >Shingle-izing the tokens, using a shingle size of %d...\n", shingleSize);
	shingleList = computeShingles(tokenList, shingleSize);
	printDebug("Done.\n");
	
	/**
	 * Remove and duplicate shingles:
	 */
	printDebug("\n >Removing any duplicate shingles...\n");
	removeDuplicates(shingleList);
	printDebug("Done.\n");
	
	/**
	 * Write out all the shingles stored in shingleList to a comma-delimited text file:
	 */
	printDebug("\n >Writing comma-delimited text to File \"%s\"...\n", outputFileName);
	delimitedText = commaDelimitArray(shingleList);
	writeTextFile(outputFileName, delimitedText);
	printDebug("Done.\n");
	
	printDebug("\n >The comma-delimited text is:\n");
	printDebug("%s\n", delimitedText);
	
	/**
	 * Free all memory allocated with malloc():
	 */
	printDebug("\n >Freeing program memory...\n");
	free(rawText);
	rawText = NULL;
	free(delimitedText);
	delimitedText = NULL;
	deleteList(tokenList);
	tokenList = NULL;
	deleteList(shingleList);
	shingleList = NULL;
	printDebug("Memory freed successfully.\n");
	
	printDebug("\nPROGRAM COMPLETE\n\n");
	
	return 0;
}

void interpretConsoleFlags(int argc, char* argv[], char** inputFileName, char** outputFileName, int* shingleSize) {
	if(argc > 1) { //If the user entered any of the optional flags
		int i;
		for(i = 1; i < argc; i++) {
			if(strcmp(argv[i], "-help") == 0) {
				printHelpText();
				exit(0);
			} else if(strcmp(argv[i], "-i") == 0) { //If the user manually specified an input file name
				*inputFileName = argv[i + 1];
			} else if(strcmp(argv[i], "-o") == 0) { //If the user manually specified an output file name
				*outputFileName = argv[i + 1];
			} else if(strcmp(argv[i], "-s") == 0) { //If the user manually specified the shingle size to use
				*shingleSize = atoi(argv[i + 1]);
				if(*shingleSize < 1) {
					printf("\nERROR: You must enter a shingle size of at least 1!\n");
					exit(1);
				}
			} else if(strcmp(argv[i], "-d") == 0) { //If the user enabled the verbose debug messages
				debugFlag = true;
			}
		}
	}
}

void printDebug(char* debugText, ...) { /**Wraps printf(), calling it only if global variable debugFlag is true **/
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

void printHelpText() {
	printf("This program takes a text file full of any arbitrary set of words as input.\n");
	printf("It then decomposes them into a comma-delimited set of \"shingles\" inside another text file.\n");
	printf("\nThe program supports several optional command-line flags, which are listed below:\n");
	
	printf("\n-help\tPrints out a brief description of the program and all the commands it accepts. You're reading it now.\n");
	printf("\tUsage:\t\"project1 -help\"\n");
	
	printf("\n-i\tSpecifies the name of the file to be read as INPUT. The default is \"input.txt\"\n");
	printf("\tUsage:\t\"project1 -i filename.txt\"\n");
	
	printf("\n-o\tSpecifies the name of the file to be read as OUTPUT and written to. The default is \"output.txt\"\n");
	printf("\tUsage:\t\"project1 -o filename.txt\"\n");
	
	printf("\n-s\tSets the size, in words, of each shingle outputted by the program.\n");
	printf("\tUsage:\t\"project1 -s 5\"\n");
	
	printf("\n-d\tPrints out additional debug information as the program runs (a LOT of it).\n");
	printf("\tUsage:\t\"project1 -d\"\n");
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
	
	printDebug("\n >Reading from file...\n"); //<DEBUG>
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

List* computeShingles(List* inputList, int groupSize) {
	List* output = createList(ARRAY_LIST);
	char* shingle = NULL;
	int i, j, strSize;
	
	for(i = 0; i < listSize(inputList) - (groupSize - 1); i++) { //The final grouping will *begin* at element number [numItems - (groupSize - 1)]
		strSize = 0;
		for(j = 0; j < groupSize; j++) { //Calculate the size of the new String we will need
			strSize += strlen(getFromList(inputList, i + j)); 
		}
		strSize += groupSize + 1; //Make the size larger to accommodate for spaces between words and a terminating zero
		
		shingle = calloc(strSize, sizeof(char));
		
		for(j = 0; j < groupSize; j++) { //Append our words to form the shingle, adding spaces after each word
			strcat(shingle, (char*)getFromList(inputList, i + j));
			strcat(shingle, " ");
		}
		shingle[strSize - 2] = '\0'; //Replace the final space with a terminating zero
		
		addToList(output, shingle);
	}
	
	return output;
}

void removeDuplicates(List* inputList) {
	char* currentItem = NULL;
	int i, k;
	
	for(i = 0; i < listSize(inputList); i++) {
		currentItem = getFromList(inputList, i);
		for(k = i + 1; k < listSize(inputList); k++) { //Iterate through every element after currentItem
			if(strcmp(currentItem, getFromList(inputList, k)) == 0) { //If an element is found that is identical to currentItem, it needs to be removed
				removeFromList(inputList, k);
				k--; //The contents of the list have shifted left, re-check this index
			}
		}
	}
}

char* commaDelimitArray(List* inputList) {
	int strSize = computeSizeOfContents(inputList) + listSize(inputList) + 1; //Allocate enough space to hold all the items, as well as a comma for each, plus the null-terminator
	char* delimitedString = calloc(strSize, sizeof(char));
	int i;
	
	for(i = 0; i < listSize(inputList); i++) {
		strcat(delimitedString, (char*)getFromList(inputList, i));
		strcat(delimitedString, ",");
	}
	delimitedString[strSize - 2] = '\0';
	
	return delimitedString;
}

int computeSizeOfContents(List* inputList) { /** Sub-function of commaDelimitArray to get the number of bytes used by all strings in a given List (minus null-terminators) **/
	int size = 0;
	int i;
	for(i = 0; i < listSize(inputList); i++) {
		size += strlen((char*)getFromList(inputList, i));
	}
	return size;
}

void writeTextFile(const char* fileName, char* text) {
	FILE* outputFile = NULL;
	int i;
	
	outputFile = fopen(fileName, "wb"); //Open the file for [w]riting in [b]inary mode (creating it if it doesn't yet exist)
	if(outputFile == NULL) { //If for some reason opening the file fails, exit immediately with a failure
		printf("\nERROR: File \"%s\" not found!\n", fileName);
		exit(1);
	} else {
		printDebug("File opened successfully.\n");
	}
	
	for(i = 0; text[i] != '\0'; i++) {
		fputc(text[i], outputFile);
	}
	

	if (ftell(outputFile) != strlen(text)) { //If not all of the text was written to the file, an error has occurred
		printf("\nERROR: Not all characters were written to file!");
		fclose(outputFile);
		exit(1);
	} else {
		printDebug("Output written to file successfully.\n");
	}
	
	fclose(outputFile);
}