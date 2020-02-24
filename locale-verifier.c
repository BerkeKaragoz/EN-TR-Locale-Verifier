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

char* command_get_output(char *command){
	FILE *fp;
	char path[1035];
	char *output = strcpy(output, "\0");

	fp = popen(command, "r");
	if (fp == NULL) {
		printf("Failed to run command get packages.\n" );
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
	    		*buffer = (char *)malloc(BUFFER_SIZE);

   		//Extract Variables and Values
	    char ch = '\0';
	    int i = 0,
	    	j = 0;

	    for( i = 0; i <= fileSize; i++){
	   		ch = content[i];

	   		//Set the buffer as variable till '='
	   		if(ch == '='){
	   			j = 0; free(buffer); buffer = (char *)malloc(BUFFER_SIZE);
	   		}

	   		//Set the buffer as value till '\n' or EOF
	   		else if (ch == '\n' || i == fileSize){
	   			strcpy(value, buffer);
	   			j = 0; free(buffer); buffer = (char *)malloc(BUFFER_SIZE);
				break;
	   		}
	   		else if (j > BUFFER_SIZE) {
	   			printf("Reached BUFFER_SIZE\n");
	   			exit(0);
	   		}
			//Continue Cursor
	   		else{buffer[j++] = ch;}

	    }//for

	    fclose(current_locale_fp);

	    int language = l_other; //for further use
		if (!strcmp(value, LANG_EN_US)){
			language = l_english;
			write_to_file(LOCALE_ENGLISH, FILENAME);
		} else if (!strcmp(value, LANG_TR_TR)){
			language = l_turkish;
			write_to_file(LOCALE_TURKISH, FILENAME);
		} else {
			printf("LANG not detected defaulted to English.\n");
			language = l_english;
			value = LANG_EN_US;
			write_to_file(LOCALE_ENGLISH, FILENAME);
		}

	    //printf("Contents Before:\n%s\n", content);
	    char *locale_gen = command_get_output("/bin/cat /etc/locale.gen | /bin/grep -v \"#\" | /bin/grep \" \""),
	    		*locale_installed = command_get_output((char *)sprintf("/bin/locale -a ; echo %s", value));
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