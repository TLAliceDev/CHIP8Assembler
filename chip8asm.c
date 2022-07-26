#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define INSTRUCTION_TABLE_SIZE 28
#define REGISTER_OPERATIONS_TABLE_SIZE 9
#define INVALID (-1)

typedef enum
{
    SYSC,
    JUMP,
    CALL,
    JEQM,
    JNEM,
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
    KGET,
    DSET,
    ASET,
    ADDI,
    FONT,
    SBCD,
    DUMP,
    LOAD,
    RETN,
    CLEAR
} INST;

typedef enum
{
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

enum FORMATS
{
	NNN,
	XNN,
	XY,
	XYN,
	X
};

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
	int enumAttempt = wordToEnum(str, regOpTable, REGISTER_OPERATIONS_TABLE_SIZE);
	if (enumAttempt != INVALID)
	{
		if (enumAttempt == RSHIFT) enumAttempt = 0xE;
		return enumAttempt;
	}
	int value;
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


int
getCode( const char* word )
{
	const EnumMapper instructionsTable [] = {{"SYSC",SYSC},{"JUMP",JUMP},{"CALL",CALL},{"JNEM",JNEM},{"JEQM",JEQM},{"JEQU",JEQU},{"SETM",SETM},{"ADDM",ADDM},{"RGOP",RGOP},{"JNEQ",JNEQ},{"SETI",SETI},{"JMPV",JMPV},{"RAND",RAND},{"DRAW",DRAW},{"KEYD",KEYD},{"KEYU",KEYU},{"DGET",DGET},{"KGET",KGET},{"DSET",DSET},{"ASET",ASET},{"ADDI",ADDI},{"FONT",FONT},{"SBCD",SBCD},{"DUMP",DUMP},{"LOAD",LOAD}, {"RETN", RETN}, {"CLEAR", CLEAR}};
	INST op = wordToEnum( word, instructionsTable, INSTRUCTION_TABLE_SIZE );
	if (op == INVALID) return INVALID;
	if (op == RETN) return 0x00EE;
	if (op == CLEAR) return 0x00E0;
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
		if (currentChar == lim || currentChar == '\n')
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

int
parseLabel(char* labels[256], int addresses[256], char* label, int n)
{
	for (int i = 0; i < n; i++)
	{
		if (strcmp(labels[i],label) == 0)
		{
			return addresses[i];
		}
	}
	printf("Couldn't find Label: %s\n",label);
	exit(-2);
	return INVALID;
}

short unsigned int
textToInstruction( const char * text, char* labels[256], int addresses[256], int labelsAmount )
{
	const enum FORMATS instructionsFormat [] = { NNN, NNN, NNN, XNN, XNN, XY, XNN, XNN, XYN, XY, NNN, NNN, XNN, XYN, X, X };
	char** words;
	int wordCount = stringSplit(text,' ',&words);
	int op = getCode(words[0]);
	if ( op == INVALID )
	{
		printf("Invalid Instruction: %s\n",words[0]);
		exit(-1);
	}
	if (op == 0x00EE || op == 0x00E0) return op;
	int arguments[4] = {0};
	for (int i = 1; i < wordCount; i++)
	{
		if (words[i][0] == '$')
		{
			int labelAttempt = parseLabel(labels,addresses,words[i],labelsAmount);
			if (labelAttempt != INVALID)
				arguments[i-1] = labelAttempt;
		}
		else
			arguments[i-1]=parseNumber(words[i]);
	}
	for (int i = 0; i < wordCount; i++)
		printf("%s ",words[i]);
	printf(" ( ");
	for (int i = 0; i < wordCount-1; i++)
		printf("%d ",arguments[i]);
	printf(")\n");
	unsigned char firstByte = op >> 12;
	firstByte &= 0xF;
	int format = instructionsFormat[firstByte];
	switch (format)
	{
		case NNN:
			if (wordCount != 2)
				printf("Invalid Argument Count: %d. Required: 1\n",wordCount-1);
			op |= arguments[0] & 0xFFF;
			break;
		case X:
			if (wordCount != 2)
				printf("Invalid Argument Count: %d. Required: 1\n",wordCount-1);
			op |= ( arguments[0] & 0xF ) << 8;
			break;
		case XNN:
			if (wordCount != 3)
				printf("Invalid Argument Count: %d. Required: 2\n",wordCount-1);
			op |= ( arguments[0] & 0xF  ) <<8;
			op |= ( arguments[1] & 0xFF );
			break;
		case XY:
			if (wordCount != 3)
				printf("Invalid Argument Count: %d. Required: 2\n",wordCount-1);
			op |= ( arguments[0] & 0xF  ) <<8;
			op |= ( arguments[1] & 0xF ) <<4;
			break;
		case XYN:
			if (wordCount != 4)
				printf("Invalid Argument Count: %d. Required: 3\n",wordCount-1);
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

unsigned short int
swapEndianess(unsigned short int x)
{
	return (x>>8) | (x << 8);
}

void
insertLabel(int index,char* labels[256], int addresses[256], char* labelName, int address)
{
	for (int i = 0; i < strlen(labelName)+1; i++)
		if (labelName[i] == '\n') labelName[i]=0;
	labels[index] = labelName;
	addresses[index] = address;
}

int 
assemble( const char* inputName, const char* outputName, int bigEndian )
{
	char* labels[256];
	int   addresses[256] = {0};
	int   labelsIndex = 0;
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
	int loc = 0;
	for (int i = 0; i < n; i++)
	{
		if (code[i][0] != '#' && code[i][0] != '\n' && code[i][0] != '$') loc++;
		if (code[i][0] == '$')
		{
			insertLabel(labelsIndex,labels,addresses,code[i],512+(loc*2));
			labelsIndex++;
		}
	}
	for (int i = 0; i < n; i++) 
	{
		if (code[i][0] == '#' || code[i][0] == '\n' || code[i][0] == '$') continue;
		unsigned short int instruction = textToInstruction(code[i], labels, addresses,labelsIndex);
		if (bigEndian)
			instruction = swapEndianess(instruction);
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
	int bigEndian = 1;
	if (argC >= 4 && argV[3][0] == 'l')
		bigEndian = 0;
	assemble(argV[1], out, bigEndian);
	return 0;
}
