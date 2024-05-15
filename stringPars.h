#include <string.h> 
#include <stdlib.h> 
#include <stdio.h> 
#include <ctype.h>

#ifndef ASSEMBLER_STRING_H
#define ASSEMBLER_STRING_H

typedef struct
{ 
	char* label; 
	char* instruction; 
	char* operand; 
	char* comment;
	
}AssemblerString;


AssemblerString* AssemblerString_ctor() 
{
	AssemblerString* asPtr = calloc(1, sizeof(AssemblerString));
	return asPtr; 

}

void AssemblerString_dctor(AssemblerString* asPtr) 
{
	if (asPtr == NULL) return;
	if (asPtr->label != NULL) free(asPtr->label);
	if (asPtr->instruction != NULL) free(asPtr->instruction); 
	if (asPtr->operand != NULL) free(asPtr->operand);
	if (asPtr->comment != NULL) free(asPtr->comment); 
	free(asPtr);

}

void AssemblerString_toString(AssemblerString* asPtr, char* outBuffer) 
{
if (asPtr == NULL) return;

sprintf(outBuffer, "Label:%s \t\t Instruction:%s \t\t Operand:%s \t\t Comment:%s", 
	asPtr->label == NULL ? "none" : asPtr->label,
	asPtr->instruction == NULL ? "none" : asPtr->instruction,
	asPtr->operand == NULL ? "none" : asPtr->operand,
	asPtr->comment == NULL ? "none" : asPtr->comment 
	);

}

const char* instructionArray[] ={"START", "END", "BYTE", "WORD", "RESB", "RESW", "MOV", "ADD", "SUB", "CMP", 
"MUL", "DIV", "JE", "LOOP", "CALL", "RET", "HLT"}; 
const int instructionsCount = 17;

char* StringCopy(const char* string, size_t copyCount)
{
	if (string == NULL) return NULL; 
	if (strlen(string) <= 0) return NULL; 

	if(copyCount == 0)
		copyCount = strlen(string);

	char* cString = calloc(copyCount + 1, sizeof(char)); 
	if (cString == NULL) return NULL; 
	memcpy(cString, string, copyCount); 
	cString[copyCount] = '\0';
	return cString;

}

int IsInstruction(const char* string) 
{
	if (string == NULL) return 0;
	char* cString = StringCopy(string, 0); 

	for (int i = 0; i < instructionsCount; i++) {
		if (strcmp(cString, instructionArray[i]) == 0) {
			free(cString);
			return 1; 
		}
 	} 

	free(cString); 
	return 0;

}

void DeleteNewStr(char* str) 
{
	if (str[strlen(str) - 1] == '\n') 
		str[strlen(str) - 1] = '\0';

}

void ParseString(char* string, AssemblerString* result) 
{
	if (string == NULL || strlen(string) <= 1) return; 
	char* comentStart = strstr(string, "/"); 

	if(comentStart != NULL)
	{
		char* commentCopy = StringCopy(comentStart + 1, 0); 
		DeleteNewStr(commentCopy);
		result->comment = commentCopy;
		*(comentStart) = '\0';
	}

	char* labelEnd = strstr(string, ":"); 
	int labelOffset = 0;

	if (labelEnd != NULL)
	{
		int labelStart = 0;
		while (string[labelStart] == ' ' && labelStart < labelEnd - string)
			labelStart++;
		labelOffset = labelEnd - (string + labelStart);
		char* labelCopy = StringCopy(string + labelStart, labelOffset); 
		DeleteNewStr(labelCopy);
		result->label = labelCopy;
		labelOffset = labelStart + labelOffset + 1;

 	}

	char* token = strtok(string + labelOffset, " \n"); 
	char** tokens = malloc(2 * sizeof(char*));
	int tokensCount = 0;

	while (token != NULL && tokensCount < 2)
	{
		char* tokenCopy = StringCopy(token, 0); 
		DeleteNewStr(tokenCopy); 
		tokens[tokensCount] = tokenCopy;
		token = strtok(NULL, " "); 
		tokensCount++;
	}

	if (tokensCount == 0) goto EndFunc; 
	int tokensOffset = 0;

	if (IsInstruction(tokens[0]))
	{
		result->instruction = StringCopy(tokens[tokensOffset], 0); 
		tokensOffset++;
		if (tokensCount == 1) goto EndFunc;
	}

	result->operand = StringCopy(tokens[tokensOffset], 0);

	EndFunc:
	for (int i = 0; i < tokensCount; i++) free(tokens[i]); 
	free(tokens);

}

void ParseFile(char* InputFileName, char* OutputFileName)
 {
	FILE* fInPtr = fopen(InputFileName, "r");
	FILE* fOutPtr = fopen(OutputFileName, "w");
	if (fInPtr == NULL || fOutPtr == NULL) return; 
	char* lineBuffer = calloc(128, sizeof(char));
	char* parsedLineBuffer = calloc(128, sizeof(char));;

	while (!feof(fInPtr)) {
		if (!fgets(lineBuffer, 126, fInPtr)) continue; 
		AssemblerString* aStrPtr = AssemblerString_ctor(); 
		ParseString(lineBuffer, aStrPtr);
		if (aStrPtr == NULL) continue; 
		AssemblerString_toString(aStrPtr, parsedLineBuffer); 
		fputs(parsedLineBuffer, fOutPtr);
		fputc('\n', fOutPtr);
		AssemblerString_dctor(aStrPtr); 
	}

	free(parsedLineBuffer); 
	free(lineBuffer); 
	fclose(fInPtr); 
	fclose(fOutPtr);

}


#endif // ASSEMBLER_STRING_H