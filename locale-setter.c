#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "localeTest.txt"

#define LANG_TR_TR "tr_TR.UTF-8"
#define LANG_EN_US "en_US.UTF-8"

#define BUFFER_SIZE 128

#define LANG "LANG"
#define LANGUAGE "LANGUAGE"
#define LC_CTYPE "LC_CTYPE"
#define LC_NUMERIC "LC_NUMERIC"
#define LC_TIME "LC_TIME"
#define LC_COLLATE "LC_COLLATE"
#define LC_MONETARY "LC_MONETARY"
#define LC_MESSAGES "LC_MESSAGES"
#define LC_PAPER "LC_PAPER"
#define LC_NAME "LC_NAME"
#define LC_ADDRESS "LC_ADDRESS"
#define LC_TELEPHONE "LC_TELEPHONE"
#define LC_MEASUREMENT "LC_MEASUREMENT"
#define LC_IDENTIFICATION "LC_IDENTIFICATION"
#define LC_ALL "LC_ALL"


typedef struct locale_var{
	char *variable,
		*value;
}locale_var_t;


void change_value(locale_var_t *var){
	printf("%s<->%s\n", var->variable, var->value);
}

int main (){
	printf("\n");
	FILE *current_locale_fp;
	current_locale_fp = fopen(FILENAME, "r");

	if (current_locale_fp != NULL)
	{
		//Get Filesize
		fseek(current_locale_fp, 0, SEEK_END);
		unsigned int fileSize = ftell(current_locale_fp);
		rewind(current_locale_fp);

		//Read File
		char *content = NULL;
		content = (char *)malloc(fileSize);
	    size_t res = fread (content,1,fileSize,current_locale_fp);

	    //Allocate
	    char *variable = (char *)malloc(BUFFER_SIZE),
	    		*value = (char *)malloc(BUFFER_SIZE),
	    		*buffer = (char *)malloc(BUFFER_SIZE);
		
		//Alloc for config variables
		locale_var_t *lvar = (struct locale_var *)malloc(sizeof(locale_var_t)*64);
		
		int i = 0;
		for (; i < 64; i++){
			(*(lvar+i)).variable = (char *)malloc(BUFFER_SIZE);
			(*(lvar+i)).value = (char *)malloc(BUFFER_SIZE);
		}
		
   		//Extract Variables and Values
	    char ch = '\0';
	    int j = 0, rowIndex = 0;
	    for( i = 0; i <= fileSize; i++){
	   		ch = content[i];
	   		
	   		//Set the buffer as variable till '='
	   		if(ch == '='){

	   			strcpy((*(lvar+rowIndex)).variable, buffer);
	   			//printf("#define %s \"%s\"\n", lvar.variable, lvar.variable);
	   			
	   			j = 0; free(buffer); buffer = (char *)malloc(BUFFER_SIZE);
	   		}
	   		//Set the buffer as value till '\n' or EOF
	   		else if (ch == '\n' || i == fileSize){
	   			strcpy((*(lvar+rowIndex)).value, buffer);

	   			j = 0; free(buffer); buffer = (char *)malloc(BUFFER_SIZE);
	   			rowIndex++;
	   		}
	   		//Continue Cursor
	   		else{buffer[j++] = ch;}
	   		
	    }//for

	    printf("Final (RI:%d) :\n", rowIndex);
	    
	    for(i = 0; i < 16; i++){
	    	printf("Variable = %-20s\tValue = %-20s\n", lvar[i].variable, lvar[i].value);
	    }

	    //Exit
		free(content);
	    fclose(current_locale_fp);
	    printf("\n");
	    
	}//fp if
	else
	{
		printf("\nCannot open file!\n");
		exit(0);
	}
	return 0;
}//End of main

/*

/etc/default/locale 

LANG=tr_TR.UTF-8
LANGUAGE=
LC_CTYPE="en_US.UTF-8"
LC_NUMERIC="tr_TR.UTF-8"
LC_TIME="tr_TR.UTF-8"
LC_COLLATE="tr_TR.UTF-8"
LC_MONETARY="en_US.UTF-8"
LC_MESSAGES="tr_TR.UTF-8"
LC_PAPER="en_US.UTF-8"
LC_NAME="tr_TR.UTF-8"
LC_ADDRESS="tr_TR.UTF-8"
LC_TELEPHONE="en_US.UTF-8"
LC_MEASUREMENT="en_US.UTF-8"
LC_IDENTIFICATION="en_US.UTF-8"
LC_ALL=

*/

	    /*
	    char *token = strtok(content, "=");
	    while (token != NULL)
	    {
	        variable = token;
	        token = strtok(NULL, "\n");
	        if (token == NULL || token == "\n"){
	        	value = NULL;
	        	puts("FFFFF");
	        }
	        else{
	        	value = token;
	        	token = strtok(NULL, "=");
	        }
			printf("Variable = %-20s Value = %-20s\n", variable, value);
	    }
	    */