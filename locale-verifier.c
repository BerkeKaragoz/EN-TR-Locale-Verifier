#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "/etc/default/locale"

#define LANG_TR_TR "tr_TR.UTF-8"
#define LANG_EN_US "en_US.UTF-8"

#define BUFFER_SIZE 256

#define STR(X) #X
#define ADD_QUOTES(X) "\""X"\""

#define LOCALE_TURKISH "\
LANG=tr_TR.UTF-8\n\
LANGUAGE=\n\
LC_CTYPE=\"en_US.UTF-8\"\n\
LC_NUMERIC=\"tr_TR.UTF-8\"\n\
LC_TIME=\"tr_TR.UTF-8\"\n\
LC_COLLATE=\"tr_TR.UTF-8\"\n\
LC_MONETARY=\"en_US.UTF-8\"\n\
LC_MESSAGES=\"tr_TR.UTF-8\"\n\
LC_PAPER=\"en_US.UTF-8\"\n\
LC_NAME=\"tr_TR.UTF-8\"\n\
LC_ADDRESS=\"tr_TR.UTF-8\"\n\
LC_TELEPHONE=\"en_US.UTF-8\"\n\
LC_MEASUREMENT=\"en_US.UTF-8\"\n\
LC_IDENTIFICATION=\"en_US.UTF-8\"\n\
LC_ALL=\n"

#define LOCALE_ENGLISH "\
LANG=en_US.UTF-8\n\
LANGUAGE=\n\
LC_CTYPE=\"en_US.UTF-8\"\n\
LC_NUMERIC=\"en_US.UTF-8\"\n\
LC_TIME=\"en_US.UTF-8\"\n\
LC_COLLATE=\"en_US.UTF-8\"\n\
LC_MONETARY=\"en_US.UTF-8\"\n\
LC_MESSAGES=\"en_US.UTF-8\"\n\
LC_PAPER=\"en_US.UTF-8\"\n\
LC_NAME=\"en_US.UTF-8\"\n\
LC_ADDRESS=\"en_US.UTF-8\"\n\
LC_TELEPHONE=\"en_US.UTF-8\"\n\
LC_MEASUREMENT=\"en_US.UTF-8\"\n\
LC_IDENTIFICATION=\"en_US.UTF-8\"\n\
LC_ALL=\n"

enum Language{
	l_other = 0,
	l_english = 1,
	l_turkish = 2,
};

void write_to_file(char *str, char *filename){
	FILE *fp;
	fp = fopen(filename, "w");

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

void change_locale(char* lang, char *filename){
	if (!strcmp(lang, LANG_EN_US)){
		write_to_file(LOCALE_ENGLISH, filename);
	} else if (!strcmp(lang, LANG_TR_TR)){
		write_to_file(LOCALE_TURKISH, filename);
	} else {
		printf("LANG not detected defaulted to English.\n");
		//lang = LANG_EN_US;
		write_to_file(LOCALE_ENGLISH, filename);
	}
}

char* run_command(char *command){
	FILE *fp;
	char path[1035];
	char *output = (char *)malloc(2048);
	*output = '\0';

	fp = popen(command, "r");
	if (fp == NULL) {
		printf("Failed to run command get packages.\n");
		exit(1);
	}

	while (fgets(path, sizeof(path), fp) != NULL) {
		strcat(output, path);
	}

	printf("\nOUTPUT of %s:\n%s", command, output);
	pclose(fp);
	return output;
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
	    fread (content,1,fileSize,current_locale_fp);

	    //Allocate
	    char 	*value = (char *)malloc(BUFFER_SIZE),
	    		*buffer = (char *)malloc(BUFFER_SIZE),
				*locale_content = (char *)malloc(BUFFER_SIZE);

   		//Extract Variables and Values
	    char ch = '\0';
	    int i = 0,
	    	buffer_index = 0;

	    for( i = 0; i <= fileSize; i++){
	   		ch = content[i];

	   		//Set the buffer as variable till '='
	   		if(ch == '='){
	   			buffer_index = 0; free(buffer); buffer = (char *)malloc(BUFFER_SIZE);
	   		}

	   		//Set the buffer as value till '\n' or EOF
	   		else if (ch == '\n' || i == fileSize){
	   			strcpy(value, buffer);
	   			buffer_index = 0; free(buffer); buffer = (char *)malloc(BUFFER_SIZE);
				break;
	   		}
	   		else if (buffer_index > BUFFER_SIZE) {
	   			printf("Reached BUFFER_SIZE\n");
	   			exit(0);
	   		}
			//Continue Cursor
	   		else{buffer[buffer_index++] = ch;}

	    }//for

	    fclose(current_locale_fp);

		//$ locale -a | grep -i LANGUAGE
		char *input_locale_installed = (char *)malloc(BUFFER_SIZE);
		strcpy(input_locale_installed, "locale -a | grep -i ");
			char *input_search_locale_installed = (char *)malloc(sizeof(char)*9);
			strncpy(input_search_locale_installed, value, 9);
			*(input_search_locale_installed+9) = '\0';
			strcat(input_locale_installed, input_search_locale_installed);

		char *locale_installed = run_command(input_locale_installed);

		if (*locale_installed != '\0'){
			//installed
			change_locale(value, FILENAME);
		}
		else //not installed
		{
			// Check if it is added on locale.gen
			// not needed -> $ cat /etc/locale.gen | grep -v "#" | if grep -sqi "LANG"; then echo "LANG" |  sudo tee -a /etc/locale.gen > /dev/null; fi
			//$ variable=test | sed "s/# "$variable"/"$variable"/g" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null

			char *locale_gen = run_command("");
		}

	    free(content);//todo frees

	}//fp if
	else
	{
		printf("\nCannot open file!\n");
		exit(0);
	}
	return 0;
}
/*End of main

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