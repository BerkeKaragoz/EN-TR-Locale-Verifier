#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "/etc/default/locale"

#define LANG_TR_TR "tr_TR.UTF-8"
#define LANG_EN_US "en_US.UTF-8"

#define BUFFER_SIZE 128

#define STR(X) #X
#define ADD_QUOTES(X) "\""X"\""

#define HASH_LANG 2089278247
#define HASH_LANGUAGE -437333815
#define HASH_LC_CTYPE -1488816680
#define HASH_LC_NUMERIC -772922490
#define HASH_LC_TIME -2126925502
#define HASH_LC_COLLATE 1965034167
#define HASH_LC_MONETARY 1474917506
#define HASH_LC_MESSAGES 1656742859
#define HASH_LC_PAPER -1474092661
#define HASH_LC_NAME -2127149836
#define HASH_LC_ADDRESS -1057658503
#define HASH_LC_TELEPHONE -1347176393
#define HASH_LC_MEASUREMENT 1203488121
#define HASH_LC_IDENTIFICATION 1629225725
#define HASH_LC_ALL -1105677076


enum Locale_Variable{
	lv_none = 0,
	lv_lang = 1,
	lv_language = 2,
	lv_lc_ctype = 3,
	lv_lc_numeric = 4,
	lv_lc_time = 5,
	lv_lc_collate = 6,
	lv_lc_monetary = 7,
	lv_lc_messages = 8,
	lv_lc_paper = 9,
	lv_lc_name = 10,
	lv_lc_address = 11,
	lv_lc_telephone = 12,
	lv_lc_measurement = 13,
	lv_lc_identification = 14,
	lv_lc_all = 15,
};

enum Language{
	l_other = 0,
	l_english = 1,
	l_turkish = 2,
};

typedef struct locale_var{
	char *variable,
		*value;
	int id;
}locale_var_t;

const unsigned int hash(const char *str) {
    unsigned int hash = 5381;  
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

unsigned int count_character(char *str, char src_ch){
	unsigned int i, count;
	for (i=0, count=1; str[i]; i++)
		count += (str[i] == src_ch);
	return count;
}

char* locale_var_to_string(locale_var_t *lvar, int size){
	char *output = (char *)malloc(BUFFER_SIZE*size);
	strcpy(output, "\0");

	for(int i = 0; i < size; i++){
		if(lvar[i].variable[0] != '\0'){
			strcat(output, lvar[i].variable);
			strcat(output, "=");
			strcat(output, lvar[i].value);
			strcat(output, "\n");
		}
	}

	return output;
}

void write_to_file(char *str, char *file_name){
	FILE *fp;
	fp = fopen(file_name, "w");

	if (fp != NULL){
		fprintf(fp, "%s", str);
		printf("WRITTEN:\n%s\n", str);
		fclose(fp);
		return;
	} else {
		printf("Cannot Write File!\n");
		return;
	}
}

int categorize_locale_var(locale_var_t lvar){
    switch(hash(lvar.variable)) {
    	case HASH_LANG:					return lv_lang;
    	case HASH_LANGUAGE:				return lv_language;
    	case HASH_LC_CTYPE:				return lv_lc_ctype;
    	case HASH_LC_NUMERIC:			return lv_lc_numeric;
    	case HASH_LC_TIME:				return lv_lc_time;
    	case HASH_LC_COLLATE:			return lv_lc_collate;
    	case HASH_LC_MONETARY:			return lv_lc_monetary;
    	case HASH_LC_MESSAGES:			return lv_lc_messages;
    	case HASH_LC_PAPER:				return lv_lc_paper;
    	case HASH_LC_NAME:				return lv_lc_name;
    	case HASH_LC_ADDRESS:			return lv_lc_address;
    	case HASH_LC_TELEPHONE:			return lv_lc_telephone;
    	case HASH_LC_MEASUREMENT:		return lv_lc_measurement;
    	case HASH_LC_IDENTIFICATION:	return lv_lc_identification;
    	case HASH_LC_ALL:				return lv_lc_all;
	    default:
	    	return lv_none;
	        printf("'%s' is not a valid variable.\n", lvar.variable);
    }
	return -1;
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
		
		unsigned int variable_count= count_character(content, '\n');

		//Alloc for config variables
		locale_var_t *lvar = (struct locale_var *)malloc(sizeof(locale_var_t)*variable_count);

		int i = 0;
		for (; i < variable_count; i++){
			(*(lvar+i)).variable = (char *)malloc(BUFFER_SIZE);
			(*(lvar+i)).value = (char *)malloc(BUFFER_SIZE);
		}
		
   		//Extract Variables and Values
	    char ch = '\0';
	    int j = 0, rowIndex = 0, assigned_id = 0;
	    for( i = 0; i <= fileSize; i++){
	   		ch = content[i];
	   		
	   		//Set the buffer as variable till '='
	   		if(ch == '='){
	   			strcpy((*(lvar+rowIndex)).variable, buffer);
	   			//printf("#define %s \"%s\"\n", lvar.variable, lvar.variable);
	   			//printf("HASH_%s = %d;\n", (*(lvar+rowIndex)).variable, hash((*(lvar+rowIndex)).variable));
	   			//Generate Cases
	   			//printf("case HASH_%s:\treturn = %d;\n", (*(lvar+rowIndex)).variable, rowIndex);
	   			
	    		(*(lvar+rowIndex)).id = assigned_id = categorize_locale_var(*(lvar+rowIndex));
	   			j = 0; free(buffer); buffer = (char *)malloc(BUFFER_SIZE);
	   		}
	   		//Set the buffer as value till '\n' or EOF
	   		else if (ch == '\n' || i == fileSize){
	   			strcpy((*(lvar+rowIndex)).value, buffer);

	   			j = 0; free(buffer); buffer = (char *)malloc(BUFFER_SIZE);
	   			rowIndex++;

	   		}
	   		else if (j > BUFFER_SIZE) {
	   			printf("Reached BUFFER_SIZE\n");
	   			exit(0);
	   		}
			//Continue Cursor
	   		else{buffer[j++] = ch;}
	   		
	    }//for

	    fclose(current_locale_fp);

	    int language = l_other;
	    //Change Locale Variables
	    for(i = 0; i < variable_count; i++){

	    	if(lvar[i].id == lv_lang){
	    		if (!strcmp(lvar[i].value, LANG_EN_US)){
	    			language = l_english;
	    		} else if (!strcmp(lvar[i].value, LANG_TR_TR)){
	    			language = l_turkish;
	    		} else {
	    			printf("LANG not detected defaulted to English\n");
	    			language = l_english;
	    			lvar[i].value = LANG_EN_US;
	    		}
			}
			else if(lvar[i].id == lv_language || lvar[i].id == lv_lc_all) continue;
			else if(lvar[i].id != lv_none){
				switch(language){
					case l_english: 
						lvar[i].value = ADD_QUOTES(LANG_EN_US);
					break;
					case l_turkish: 
						switch(lvar[i].id){
							case lv_lc_ctype: 			lvar[i].value = ADD_QUOTES(LANG_EN_US); break;
							case lv_lc_numeric:			lvar[i].value = ADD_QUOTES(LANG_EN_US);	break;
							case lv_lc_time:			lvar[i].value = ADD_QUOTES(LANG_TR_TR);	break;
							case lv_lc_collate:			lvar[i].value = ADD_QUOTES(LANG_TR_TR);	break;
							case lv_lc_monetary:		lvar[i].value = ADD_QUOTES(LANG_EN_US);	break;
							case lv_lc_messages:		lvar[i].value = ADD_QUOTES(LANG_TR_TR);	break;
							case lv_lc_paper:			lvar[i].value = ADD_QUOTES(LANG_EN_US);	break;
							case lv_lc_name:			lvar[i].value = ADD_QUOTES(LANG_TR_TR);	break;
							case lv_lc_address:			lvar[i].value = ADD_QUOTES(LANG_TR_TR);	break;
							case lv_lc_telephone:		lvar[i].value = ADD_QUOTES(LANG_EN_US);	break;
							case lv_lc_measurement:		lvar[i].value = ADD_QUOTES(LANG_EN_US);	break;
							case lv_lc_identification:	lvar[i].value = ADD_QUOTES(LANG_EN_US);	break;
						}
					break;
				}
			}
	    }	    
	    //printf("%s", locale_var_to_string(lvar, rowIndex));
	    write_to_file(locale_var_to_string(lvar, rowIndex), FILENAME);
	    printf("Contents Before:\n%s\n", content);
	    free(content);
	    
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
				switch(language){
					case l_english: 
						lvar[i].value = ADD_QUOTES(LANG_EN_US);
					break;
					case l_turkish: 
						lvar[	lv_lc_ctype				].value		= ADD_QUOTES(LANG_EN_US);
						lvar[	lv_lc_numeric			].value 	= ADD_QUOTES(LANG_TR_TR);
						lvar[	lv_lc_time				].value 	= ADD_QUOTES(LANG_TR_TR);
						lvar[	lv_lc_collate			].value 	= ADD_QUOTES(LANG_TR_TR);
						lvar[	lv_lc_monetary			].value 	= ADD_QUOTES(LANG_EN_US);
						lvar[	lv_lc_messages			].value 	= ADD_QUOTES(LANG_TR_TR);
						lvar[	lv_lc_paper				].value 	= ADD_QUOTES(LANG_EN_US);
						lvar[	lv_lc_name				].value 	= ADD_QUOTES(LANG_TR_TR);
						lvar[	lv_lc_address			].value 	= ADD_QUOTES(LANG_TR_TR);
						lvar[	lv_lc_telephone			].value 	= ADD_QUOTES(LANG_EN_US);
						lvar[	lv_lc_measurement		].value 	= ADD_QUOTES(LANG_EN_US);
						lvar[	lv_lc_identification	].value 	= ADD_QUOTES(LANG_EN_US);
					break;
				}
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