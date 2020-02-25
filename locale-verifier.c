#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>

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

typedef enum {false, true} bool;

enum Language{
	l_other = 0,
	l_english = 1,
	l_turkish = 2,
};

bool write_to_file(char *str, char *filename){
	FILE *fp;
	fp = fopen(filename, "w");

	if (fp != NULL){
		fprintf(fp, "%s", str);
		printf("---Written---\n%s\n", str);
		fclose(fp);
		return true;
	} else {
		printf("Cannot write to the file, check permissions!\n");
		return false;
	}
}

bool change_locale(char* lang, char *filename){
	bool is_success = false;
	if (!strcmp(lang, LANG_EN_US)){
		is_success = write_to_file(LOCALE_ENGLISH, filename);
	} else if (!strcmp(lang, LANG_TR_TR)){
		is_success = write_to_file(LOCALE_TURKISH, filename);
	} else {
		printf("LANG not detected defaulted to English.\n");
		//lang = LANG_EN_US;
		is_success = write_to_file(LOCALE_ENGLISH, filename);
	}
	return is_success;
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

	//printf("\nOUTPUT of %s:\n%s", command, output);
	pclose(fp);
	return output;
}

void reboot_ask(){
	printf("\nLocale is verified, do you want to reboot now? (y/n): ");
	char *prompt_input = (char *)malloc(sizeof(char)*256);
	scanf("%s", prompt_input);

	*prompt_input = tolower(*prompt_input);
	int i = 0;
	while(*prompt_input != 'y' && *prompt_input != 'n' && i < 4){
		printf("\nType 'y' or 'n': ");
		scanf("%s", prompt_input);
		i++;
	}

	if (*prompt_input == 'y'){
		free(prompt_input);
		// Reboot
		// echo "Close to cancel reboot.";for i in 1 2 3 4 5; do echo "$i"; sleep 1; done ; echo "Rebooting..."; sleep 1; sudo reboot now

		printf("\nClose to cancel rebooting.\n");

		int i = 5;
		for (; i > 0; i--){
			printf("%d\n", i);
			sleep(1);
		}

		printf("\nRebooting...\n");
		run_command("sudo reboot now");
	}
	else
	{
		// Not reboot
		free(prompt_input);
	}
}

int main (){
	printf("\n");

	FILE *current_locale_fp;
	current_locale_fp = fopen(FILENAME, "r");

	if (current_locale_fp != NULL)
	{
		//Get Filesize
		fseek(current_locale_fp, 0, SEEK_END);
		unsigned int file_size = ftell(current_locale_fp);
		rewind(current_locale_fp);

		//Read File
		char *content = NULL;
		content = (char *)malloc(file_size);
	    fread (content, 1,file_size,current_locale_fp);

	    //Allocate
	    char 	*lang_value = (char *)malloc(BUFFER_SIZE),
	    		*buffer = (char *)malloc(BUFFER_SIZE);

   		//Extract LANG
	    char ch = '\0';
	    int i = 0,
	    	buffer_index = 0;

	    for( i = 0; i <= file_size; i++){
	   		ch = content[i];

	   		//Set the buffer as variable till '='
	   		if(ch == '='){
	   			buffer_index = 0; free(buffer); buffer = (char *)malloc(BUFFER_SIZE);
	   		}

	   		//Set the buffer as value till '\n' or EOF
	   		else if (ch == '\n' || i == file_size){
	   			strcpy(lang_value, buffer);
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

		free(content);
		free(buffer);
	    fclose(current_locale_fp);

		//$ locale -a 2> /dev/null | grep -i LANG
		// Search LANG if installed
		char *input_locale_installed = (char *)malloc(BUFFER_SIZE);
		strcpy(input_locale_installed, "locale -a 2> /dev/null | grep -i ");
			char *input_search_locale_installed = (char *)malloc(sizeof(char)*9);
			strncpy(input_search_locale_installed, lang_value, 9);
			*(input_search_locale_installed+9) = '\0';
			strcat(input_locale_installed, input_search_locale_installed);
		free(input_search_locale_installed);

		char *locale_installed = run_command(input_locale_installed);
		free(input_locale_installed);

		if (*locale_installed == '\0'){
			//if not installed
			printf("Locale is not installed.\n");
			// Check if it is added on locale.gen
			//$ variable=LANG ; sed "s/# "$variable"/"$variable"/gi" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null
			char *input_locale_gen = (char *)malloc(BUFFER_SIZE);
				strcpy(input_locale_gen, "variable=");
				strncat(input_locale_gen, lang_value, 9);
				strcat(input_locale_gen, " ; sed \"s/# \"$variable\"/\"$variable\"/gi\" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null");
			run_command(input_locale_gen);
			free(input_locale_gen);
			
			//Generate Locales
			printf("Getting locales...\n");
			run_command("sudo locale-gen");
		}
		free(locale_installed);

		if (change_locale(lang_value, FILENAME)){
			reboot_ask();
		}
		else
		{
			printf("\nVerification failed.\n");
		}
		free(lang_value);

	}//if file pointer is null
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

Get locale
$ cat /etc/locale.gen | grep -v "#" | if grep -sqi "LANG"; then echo "LANG" |  sudo tee -a /etc/locale.gen > /dev/null; fi
Get all locale packs inside
$ locale 2> /dev/null | cut -d '=' -f2 | sort | uniq | sed '1{^$/d}'
*/