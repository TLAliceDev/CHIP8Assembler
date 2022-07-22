#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INSTRUCTION_TABLE_SIZE 26
#define REGISTER_OPERATIONS_TABLE_SIZE 9
#define INVALID (-1)

typedef enum{
    SYSC,
    JUMP,
    CALL,
    JNEM,
    JEQM,
    JEQU,
    SETM,
    ADDM,
    RGOP,
    JNEQ,
    SETI,
    JMPV,
    RAND,
    DRAW,
    KEYD,
    KEYU,
    DGET,
    KEYG,
    DSET,
    ASET,
    ADDI,
    FONT,
    SBCD,
    DUMP,
    LOAD,
} INST;

typedef enum{
	ASSIGN,
	BWOR,
	BWAND,
	BWXOR,
	ADD,
	SUB,
	LSHIFT,
	NEGATE,
	RSHIFT
} REGISTER_OPERATIONS;

typedef struct 
{
    const char * text;
    int     enumValue;
}EnumMapper;


int
wordToEnum (const char *str, const EnumMapper table[], int tableSize)
{
     int j;
     for (j = 0;  j < tableSize;  ++j)
         if (!strcmp (str, table[j].text))
             return table[j].enumValue;    
     return INVALID;
}


int 
parseNumber(const char* str)
{
	const EnumMapper regOpTable [] ={{"ASSIGN",ASSIGN},{"BWORD",BWOR},{"BWAND",BWAND},{"BWXOR",BWXOR},{"ADD",ADD},{"SUB",SUB},{"LSHIFT",LSHIFT},{"NEGATE",NEGATE},{"RSHIFT",RSHIFT}};
	int value;
	int enumAttempt = wordToEnum(str, regOpTable, REGISTER_OPERATIONS_TABLE_SIZE);
	if (enumAttempt != INVALID)
	{
		if (enumAttempt == RSHIFT) enumAttempt = 0xE;
		return enumAttempt;
	}
	if (str[0] == '0') 
	{
		char *newString = malloc(sizeof(char)*strlen(str)-1);
		strcpy(newString, str+2);
		newString[strlen(str)-1] = '\0';
		switch (str[1])
		{
			case 'x':
				value = strtol(newString,NULL,16);
				break;
			case 'o':
				value = strtol(newString,NULL,8);
				break;
			case 'd':
				value = strtol(newString,NULL,10);
				break;
			case 'b':
				value = strtol(newString,NULL,2);
				break;
			default:
				value = 0;
				break;

		}
		free(newString);
	}
	else
		value = atoi(str);
	return value;
}

int
wordCounter(const char* str, const char lim)
{
	int n = 1;
	char currentChar = 1;
	int currentIndex = 0;
	while ( currentChar != '\0' )
	{
		currentChar = str[currentIndex];
		currentIndex++;
		if (currentChar == lim) n++;
	}
	return n;
}


short unsigned int
getCode( const char* word )
{
	const EnumMapper instructionsTable [] = {{"SYSC",SYSC},{"JUMP",JUMP},{"CALL",CALL},{"JNEM",JNEM},{"JEQM",JEQM},{"JEQU",JEQU},{"SETM",SETM},{"ADDM",ADDM},{"RGOP",RGOP},{"JNEQ",JNEQ},{"SETI",SETI},{"JMPV",JMPV},{"RAND",RAND},{"DRAW",DRAW},{"KEYD",KEYD},{"KEYU",KEYU},{"DGET",DGET},{"KEYG",KEYG},{"DSET",DSET},{"ASET",ASET},{"ADDI",ADDI},{"FONT",FONT},{"SBCD",SBCD},{"DUMP",DUMP},{"LOAD",LOAD}};
	INST op = wordToEnum( word, instructionsTable, INSTRUCTION_TABLE_SIZE );
	int option = 0x000;
	if (op == 0xE) option = 0x09E;
	else if (op == 0xF)
	{
		op = 0xE;
		option = 0x0A1;
	}
	if (op > 0xF)
	{
		int o = op - 0xF;
		switch ( o ) {
			case 1:
				option = 0x007;
				break;
			case 2:
				option = 0x00A;
				break;
			case 3:
				option = 0x015;
				break;
			case 4:
				option = 0x018;
				break;
			case 5:
				option = 0x01E;
				break;
			case 6:
				option = 0x029;
				break;
			case 7:
				option = 0x033;
				break;
			case 8:
				option = 0x055;
				break;
			case 9:
				option = 0x065;
				break;
			default:
				break;
		}
		op = 0xF;
	}
	return (op << 12) | option;
}

int
stringSplit( const char * text, const char lim, char*** words)
{
	int wordCount = wordCounter(text, lim);
	(*words) = malloc(sizeof(char*) * wordCount);
	int currentIndex = 0;
	int currentChar  = lim+1;
	int currentWordSize = -1;
	int currentWordIndex = 0;
	int currentWordStartingPoint = 0;
	while ( currentWordIndex < wordCount )
	{
		currentChar = text[currentIndex];
		currentWordSize++;
		if (currentChar == lim)
		{
			(*words)[currentWordIndex] = malloc(sizeof(char)*currentWordSize);
			strncpy((*words)[currentWordIndex], text+currentWordStartingPoint,currentWordSize);
			(*words)[currentWordIndex][currentWordSize] = 0;
			currentWordSize = 0;
			currentWordIndex++;
			currentIndex++;
			currentWordStartingPoint = currentIndex;
		}
		currentIndex++;
	}
	return wordCount;
}

short unsigned int
textToInstruction( const char * text )
{
	char** words;
	int wordCount = stringSplit(text,' ',&words);
	unsigned short int op = getCode(words[0]);
	int arguments[wordCount-1];
	for (int i = 1; i < wordCount; i++)
	{
		arguments[i-1]=parseNumber(words[i]);
	}
	for (int i = 0; i < wordCount; i++)
		printf("%s ",words[i]);
	printf(" ( ");
	for (int i = 0; i < wordCount-1; i++)
		printf("%d ",arguments[i]);
	printf(")\n");
	switch (wordCount-1)
	{
		case 1:
			if (op & 0xE000 || op & 0xF000)
				op |= ( arguments[0] & 0x0F) << 8;
			else
				op |= ( arguments[0] & 0x0FFF);
			break;
		case 2:
			op |= ( arguments[0] & 0xF ) <<8;
			if ( op & 0x3000 || op & 0x4000 || op & 0x6000 || op & 0x7000 || op & 0xC000)
				op |= ( arguments[1] & 0xFF );
			else
				op |= ( arguments[1] & 0xFF ) <<4;
			break;
		case 3:
			op |= ( arguments[0] & 0xF ) <<8;
			op |= ( arguments[1] & 0xF ) <<4;
			op |= ( arguments[2] & 0xF ) ;
			break;
	}
	for (int i = 0; i < wordCount; i++)
		free(words[i]);
	free(words);
	return op;
}

int 
assemble( const char* inputName, const char* outputName )
{
	FILE* sourceFile, *destinationFile;
	char code[1024][1024];
	sourceFile = fopen(inputName, "r");
	if (sourceFile == NULL)
	{
		printf("Failure in reading file %s\n",inputName);
		return 0;
	}
	int n = 0;
	while ( fgets( code[n], 1024, sourceFile) )
		n++;
	fclose(sourceFile);
	destinationFile = fopen(outputName,"w");
	for (int i = 0; i < n; i++) 
	{
		int instruction = textToInstruction(code[i]);
		fwrite(&instruction,2,1,destinationFile);
	}
	fclose(destinationFile);
	return 1;
}

int main ( int argC, char **argV )
{
	if ( argC < 2 )
	{
		printf("No files provided.\n");
		return 1;
	}
	char* out = "cart.c8";
	if (argC < 3 )
		printf("Output file name not provided. Defaulting to cart.c8.\n");
	else
		out = argV[2];
	assemble(argV[1], out);
	return 0;
}
