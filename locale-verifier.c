#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

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

char** str_split(char* a_str, const char a_delim, size_t *count){
    char** result    = 0;
    char* tmp        = a_str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = a_delim;
    delim[1] = 0;

	*count = 0;

    // Count how many elements will be extracted.
    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            (*count)++;
            last_comma = tmp;
        }
        tmp++;
    }

    // Add space for trailing token.
    *count += last_comma < (a_str + strlen(a_str) - 1);

    // Add space for terminating null string so caller
    // knows where the list of returned strings ends.
    (*count)++;

    result = malloc(sizeof(char*) * (*count));

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            assert(idx < *count);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == *count - 1);
        *(result + idx) = 0;
    }

    return result;
}

bool write_to_file(char *str, char *filename){
	FILE *fp;
	fp = fopen(filename, "w");

	if (fp != NULL){
		fprintf(fp, "%s", str);
		printf("\n| --- Written --- |\n%s\n", str);
		fclose(fp);
		return true;
	} else {
		printf("Cannot write to the file, check permissions!\n");
		return false;
	}
}

char* run_command(char* command){
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

bool change_locale(char* lang, char *filename){
	bool is_success = false;
	if (!strcmp(lang, LANG_EN_US)){
		is_success = write_to_file(LOCALE_ENGLISH, filename);
	} else if (!strcmp(lang, LANG_TR_TR)){
		is_success = write_to_file(LOCALE_TURKISH, filename);
	} else {
		printf("LANG not detected defaulted to English.\n");
		is_success = write_to_file(LOCALE_ENGLISH, filename);
	}
	return is_success;
}

char* search_locale(char* to_search){
	//$ locale -a 2> /dev/null | grep -i LANG
	// Search LANG if installed
	char *output = (char *)malloc(BUFFER_SIZE);
	char *search_locale_input = (char *)malloc(sizeof(char)*9);

	strcpy(output, "locale -a 2> /dev/null | grep -i ");
		strncpy(search_locale_input, to_search, 9);
		*(search_locale_input+9) = '\0';
		strcat(output, search_locale_input);
	free(search_locale_input);
	return output;
}

char** extract_charmaps(char* str, size_t *count){
	char* command = (char *)malloc(sizeof(char)*1024);//TODO correct malloc
		strcpy(command, "printf \"");
		strcat(command, str);
		strcat(command, "\" 2> /dev/null | cut -d '=' -f2 | sort | uniq | sed '1{/^$/d}'");
	
	char* temp = run_command(command);
		
	free(command);
	return str_split(temp, '\n', count);
}

void add_lang_to_localegen(char* lang_value){
	// Check if it is added on locale.gen
	//$ variable=LANG ; sed "s/# "$variable"/"$variable"/gi" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null
	char *input_locale_gen = (char *)malloc(BUFFER_SIZE);
		strcpy(input_locale_gen, "variable=");
		strncat(input_locale_gen, lang_value, 9);
		strcat(input_locale_gen, " ; sed \"s/# \"$variable\"/\"$variable\"/gi\" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null");
	run_command(input_locale_gen);
	free(input_locale_gen);
}

void reboot_ask(){
	printf("\nLocale is verified, do you want to reboot to apply settings? (y/n): ");
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
	FILE *current_locale_fp = fopen(FILENAME, "r");
	FILE *current_localegen_fp = fopen("/etc/locale.gen", "w");

	// Check if has permissions
	if (current_locale_fp != NULL && current_localegen_fp != NULL)
	{
		fclose(current_locale_fp);
		fclose(current_localegen_fp);
		char *lang_value = run_command("cat /etc/default/locale 2> /dev/null | grep '^LANG=' | cut -d '=' -f2");

		size_t ln_count;
		// Search LANGs if installed
		char **locales_needed = !strcmp(lang_value, LANG_TR_TR) ? extract_charmaps(LOCALE_TURKISH, &ln_count) : extract_charmaps(LOCALE_ENGLISH, &ln_count);

		char *locale_installed;
		bool is_locale_gen_needed = false;
		for(int ln_i = 0; ln_i < ln_count-1; ln_i++){//-1 for null

			//$ locale -a 2> /dev/null | grep -i LANG
			locale_installed = run_command(search_locale(*(locales_needed+ln_i)));
			if (*locale_installed == '\0'){
				printf("Locale '%s' is not installed.\n", *(locales_needed+ln_i));
				is_locale_gen_needed = true;
				add_lang_to_localegen(*(locales_needed+ln_i));
			}
		free(locale_installed);
		}

		if (is_locale_gen_needed){
			//Generate Locales
			printf("Installing locales...\n");
			run_command("sudo locale-gen");
		}

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
		printf("\nCannot open required files, permissions are denied!\n");
		fclose(current_locale_fp);
		fclose(current_localegen_fp)
		exit(1);
	}
	return 0;
}