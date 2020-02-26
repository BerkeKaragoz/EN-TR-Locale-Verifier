#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define LOCALE_FILENAME "/etc/default/locale"
#define LOCALEGEN_FILENAME "/etc/locale.gen"

#define LANG_TR_TR "tr_TR.UTF-8"
#define LANG_EN_US "en_US.UTF-8"

#define BUFFER_SIZE 256

#define STR(X) #X
#define ADD_QUOTES(X) "\""X"\""

#define LOCALE_TURKISH "\
LANG="				""		LANG_TR_TR	"\n\
LANGUAGE="								"\n\
LC_CTYPE="			"\""	LANG_EN_US	"\"\n\
LC_NUMERIC="		"\""	LANG_TR_TR	"\"\n\
LC_TIME="			"\""	LANG_TR_TR	"\"\n\
LC_COLLATE="		"\""	LANG_TR_TR	"\"\n\
LC_MONETARY="		"\""	LANG_EN_US	"\"\n\
LC_MESSAGES="		"\""	LANG_TR_TR	"\"\n\
LC_PAPER="			"\""	LANG_EN_US	"\"\n\
LC_NAME="			"\""	LANG_TR_TR	"\"\n\
LC_ADDRESS="		"\""	LANG_TR_TR	"\"\n\
LC_TELEPHONE="		"\""	LANG_EN_US	"\"\n\
LC_MEASUREMENT="	"\""	LANG_EN_US	"\"\n\
LC_IDENTIFICATION="	"\""	LANG_EN_US	"\"\n\
LC_ALL="								"\n"

#define LOCALE_ENGLISH "\
LANG="				""		LANG_EN_US	"\n\
LANGUAGE="								"\n\
LC_CTYPE="			"\""	LANG_EN_US	"\"\n\
LC_NUMERIC="		"\""	LANG_EN_US	"\"\n\
LC_TIME="			"\""	LANG_EN_US	"\"\n\
LC_COLLATE="		"\""	LANG_EN_US	"\"\n\
LC_MONETARY="		"\""	LANG_EN_US	"\"\n\
LC_MESSAGES="		"\""	LANG_EN_US	"\"\n\
LC_PAPER="			"\""	LANG_EN_US	"\"\n\
LC_NAME="			"\""	LANG_EN_US	"\"\n\
LC_ADDRESS="		"\""	LANG_EN_US	"\"\n\
LC_TELEPHONE="		"\""	LANG_EN_US	"\"\n\
LC_MEASUREMENT="	"\""	LANG_EN_US	"\"\n\
LC_IDENTIFICATION="	"\""	LANG_EN_US	"\"\n\
LC_ALL="								"\n"

typedef enum {false, true} bool;

// Splits STR by the DELIMITER to string array and returns it with element COUNT
char** str_split(char* str, const char delimiter, size_t *count){
    char** result    = 0;
    char* tmp        = str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = delimiter;
    delim[1] = 0;

	*count = 0;

    // Count how many elements will be extracted.
    while (*tmp)
    {
        if (delimiter == *tmp)
        {
            (*count)++;
            last_comma = tmp;
        }
        tmp++;
    }

    // Add space for trailing token.
    *count += last_comma < (str + strlen(str) - 1);

    // Add space for terminating null string so caller
    // knows where the list of returned strings ends.
    (*count)++;

    result = malloc(sizeof(char*) * (*count));

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(str, delim);

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

// Write STR to FILENAME
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

// Executes sh COMMAND and returns the OUTPUT
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

// Get LANG and write verified values to LOCALE_FILENAME
// default: LOCALE_FILENAME = /etc/default/locale
bool write_lang_to_locale(char* lang, char *locale_filename){
	bool is_success = false;
	if (!strcmp(lang, LANG_EN_US)){
		is_success = write_to_file(LOCALE_ENGLISH, locale_filename);
	} else if (!strcmp(lang, LANG_TR_TR)){
		is_success = write_to_file(LOCALE_TURKISH, locale_filename);
	} else {
		printf("LANG not detected defaulted to English.\n");
		is_success = write_to_file(LOCALE_ENGLISH, locale_filename);
	}
	return is_success;
}

// Search LANG if installed
// Same as:
// $ locale -a 2> /dev/null | grep -i LANG
char* search_locale(char* to_search){
	char *output = (char *)malloc(BUFFER_SIZE);
	char *search_locale_input = (char *)malloc(sizeof(char)*9);

	strcpy(output, "locale -a 2> /dev/null | grep -i ");
		strncpy(search_locale_input, to_search, 9);
		*(search_locale_input+9) = '\0';
		strcat(output, search_locale_input);
	free(search_locale_input);
	return output;
}

// Same as:
// $ printf "THE_STRING" 2> /dev/null | cut -d '=' -f2 | sort | uniq | sed '1{/^$/d}'
char** extract_charmaps(char* str, size_t *count){
	char* command = (char *)malloc(sizeof(char)*1024);//TODO correct malloc
		strcpy(command, "printf \"");
		strcat(command, str);
		strcat(command, "\" 2> /dev/null | cut -d '=' -f2 | sort | uniq | sed '1{/^$/d}'");
	
	char* temp = run_command(command);
		
	free(command);
	// Return after splitting the results to string array
	return str_split(temp, '\n', count);
}

// Add LANG to /etc/locale.gen to install later on
// Same as:
// $ variable=LANG ; sed "s/# "$variable"/"$variable"/gi" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null
void add_lang_to_localegen(char* lang){
	char *input_locale_gen = (char *)malloc(BUFFER_SIZE);
		strcpy(input_locale_gen, "variable=");
		strncat(input_locale_gen, lang, 9);
		strcat(input_locale_gen, " ; sed \"s/# \"$variable\"/\"$variable\"/gi\" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null");
	run_command(input_locale_gen);
	free(input_locale_gen);
}

// Ask if the user want to reboot X seconds, and do it.
void reboot_ask(int seconds_to_wait){
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
		// Reboot, same as:
		// $ echo "Close to cancel reboot.";for i in 1 2 3 4 5; do echo "$i"; sleep 1; done ; echo "Rebooting..."; sleep 1; sudo reboot now
		printf("\nClose to cancel rebooting.\n");

		// Countdown
		for (; seconds_to_wait > 0; seconds_to_wait--){
			printf("%d\n", seconds_to_wait);
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
	FILE *current_locale_fp = fopen(LOCALE_FILENAME, "r");
	FILE *current_localegen_fp = fopen(LOCALEGEN_FILENAME, "a");

	// Check if script has permissions
	if (current_locale_fp != NULL && current_localegen_fp != NULL)
	{
		fclose(current_locale_fp);
		fclose(current_localegen_fp);
		
		// Get the systems locale LANG
		char *lang_value = run_command("cat /etc/default/locale 2> /dev/null | grep '^LANG=' | cut -d '=' -f2 | tr '\\n' '\\0'");
		
		// Extract the LANGs needed to be installed after verifying
		size_t ln_count; // ln == locales_needed
		char **locales_needed = !strcmp(lang_value, LANG_TR_TR) ?
			extract_charmaps(LOCALE_TURKISH, &ln_count) : extract_charmaps(LOCALE_ENGLISH, &ln_count);

		// Check if the LANG is installed -> locale -a
		char *locale_installed;
		bool is_locale_gen_needed = false;
		for(int ln_i = 0; ln_i < ln_count-1; ln_i++){//-1 for null

			// $ locale -a 2> /dev/null | grep -i LANG
			locale_installed = run_command(search_locale(*(locales_needed+ln_i)));

			if (*locale_installed == '\0'){
				printf("Locale '%s' is not installed.\n", *(locales_needed+ln_i));
				is_locale_gen_needed = true;
				add_lang_to_localegen(*(locales_needed+ln_i));
			}
			free(locale_installed);

		}

		// Generate Locales if needed
		if (is_locale_gen_needed){
			printf("Installing locales...\n");
			run_command("sudo locale-gen");
		}

		// Write to verified locale to /etc/default/locale
		if (write_lang_to_locale(lang_value, LOCALE_FILENAME))
			reboot_ask(5); else printf("Verification failed.\n"); free(lang_value);

	}// if file pointer is null
	else
	{
		printf("Cannot open required files, permissions are denied!\n");
		exit(1);
	}

	return 0;
}// main