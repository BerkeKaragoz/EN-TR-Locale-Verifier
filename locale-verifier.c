#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#define DEBUG_LV

#ifdef DEBUG_LV
#include <time.h>
#endif

#define LOCALE_FILENAME "/etc/default/locale"
#define LOCALEGEN_FILENAME "/etc/locale.gen"

#define LANG_TR_TR "tr_TR.UTF-8"
#define LANG_EN_US "en_US.UTF-8"

#define STR(X) #X
#define ADD_QUOTES(X) "\""X"\""

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  (byte & 0x80 ? '1' : '0'), \
  (byte & 0x40 ? '1' : '0'), \
  (byte & 0x20 ? '1' : '0'), \
  (byte & 0x10 ? '1' : '0'), \
  (byte & 0x08 ? '1' : '0'), \
  (byte & 0x04 ? '1' : '0'), \
  (byte & 0x02 ? '1' : '0'), \
  (byte & 0x01 ? '1' : '0')

#define STR_RED(X) 	"\033[1;31m"X"\033[0m"
#define STR_CYAN(X) "\033[1;34m"X"\033[0m"
#define STR_BOLD(X) "\e[1;37m"X"\e[0m"

#define PRINT_BYTE(X) 	printf(STR_CYAN(#X)": "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN"\n", BYTE_TO_BINARY(X>>8), BYTE_TO_BINARY(X));
#define SOUT(T, X)		printf(STR_CYAN(#X)": %"T"\n", X);

#define LOCALE_TURKISH "\
LANG="				""		LANG_TR_TR	"\n\
LANGUAGE="			""					"\n\
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
LANGUAGE="			""					"\n\
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


//		 		   Languages  OtherArgs
//		 		_______|______  __|_
//				0000 0000 0000  0000
enum Flag{
	Flag_Empty	 = 				  0b0, // All 0
	Flag_Unique	 = 			   0b0001, // Get one only
	Flag_Verify  = 			   0b0010,
	Flag_English = 0b0000000000010000,
	Flag_Turkish = 0b0000000000100000
};

// Returns STR's size 
size_t strptrlen(char *str){
	char *tmp = str;
	uint16_t str_lenght = 0;
	// Count STR's lenght
	while(*tmp++) str_lenght++;
	return (size_t) str_lenght * sizeof(char);
}

// Splits STR by the DELIMITER to string array and returns it with element COUNT
char** str_split(char* str, const char delimiter, size_t *count_ptr){
    char** result    = 0;
    char* tmp        = str;
    char* last_comma = 0;
    char delim[2];
    delim[0] = delimiter;
    delim[1] = 0;

	*count_ptr = 0;

    // Count how many elements will be extracted.
    while (*tmp)
    {
        if (delimiter == *tmp)
        {
            (*count_ptr)++;
            last_comma = tmp;
        }
        tmp++;
    }

    // Add space for trailing token.
    *count_ptr += last_comma < (str + strlen(str) - 1);

    // Add space for terminating null string so caller
    // knows where the list of returned strings ends.
    (*count_ptr)++;

    result = malloc(sizeof(char*) * (*count_ptr));

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(str, delim);

        while (token)
        {
            assert(idx < *count_ptr);
            *(result + idx++) = strdup(token);
            token = strtok(0, delim);
        }
        assert(idx == *count_ptr - 1);
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
#ifdef DEBUG_LV
		printf("\n"STR_CYAN("| --- Written --- |")"\n\e[3;37m%s\n"STR_CYAN("| --- EOF --- |")"\n", str);
#endif
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
	size_t output_size = sizeof(char);
	char path[1035];
	char *output = (char *)malloc(output_size);
	*output = '\0';

	fp = popen(command, "r");
	if (fp == NULL) {
		printf("Failed to run command get packages.\n");
		exit(1);
	}

	while (fgets(path, sizeof(path), fp) != NULL) {
		output_size += sizeof(path);
		output = realloc(output, output_size);
		strcat(output, path);
	}

	pclose(fp);
	return output;
}

// Parses the string (LANG_VALUE) to FLAGS
bool set_language_flag(uint16_t *flags, char *lang_value){
	if (!strcmp(lang_value, LANG_EN_US))
	{
		*flags |= Flag_English;
	}
	else if (!strcmp(lang_value, LANG_TR_TR))
	{
		*flags |= Flag_Turkish;
	}
	else 
	{
		*flags |= Flag_English;
		fprintf(stderr, STR_BOLD("LANG")" not detected defaulted to "STR_BOLD("English")".\n");
		return false;
	}
	return true;
}

// Get LANG and write verified values to LOCALE_FILENAME
// default: LOCALE_FILENAME = /etc/default/locale
bool write_lang_to_locale(char* lang, char *locale_filename){
	bool is_success = false;

	if (!strcmp(lang, LANG_EN_US))
	{
		is_success = write_to_file(LOCALE_ENGLISH, locale_filename);
	}
	else if (!strcmp(lang, LANG_TR_TR))
	{
		is_success = write_to_file(LOCALE_TURKISH, locale_filename);
	}
	else 
	{
		printf("LANG not detected defaulted to English.\n");
		is_success = write_to_file(LOCALE_ENGLISH, locale_filename);
	}

	return is_success;
}

// Search LANG if installed
// Same as:
// $ locale -a 2> /dev/null | grep -i LANG
char* search_locale_installed(char* to_search){
	char *search_locale_input = (char *)malloc(0); // first 9 letters are needed, ex: en_US (5) .UTF (4)
	char *output = (char *)malloc(
		sizeof("locale -a 2> /dev/null | grep -i ") + sizeof(char)*9
	);

	strcpy(output, "locale -a 2> /dev/null | grep -i ");
		strncpy(search_locale_input, to_search, 9);
		*(search_locale_input + 9) = '\0';
		strcat(output, search_locale_input);
	free(search_locale_input);
	
	return run_command(output);
}

// Same as:
// $ printf "THE_STRING" 2> /dev/null | cut -d '=' -f2 | sort | uniq | sed '1{/^$/d}'
char** extract_charmaps(char* str, size_t *count){

	char* command = (char *)malloc(
			strptrlen(str) + sizeof("printf \"\" 2> /dev/null | cut -d '=' -f2 | sort | uniq | sed '1{/^$/d}")
		);

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
// $ search="^LANG\." ; result=$(grep $search /usr/share/i18n/SUPPORTED) ; grep -v "$result" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null ; echo $result | sudo tee -a /etc/locale.gen > /dev/null
void add_lang_to_localegen(char* lang){
	char *input_locale_gen = (char *)malloc(
		strptrlen(lang) + sizeof("search=\"^\" ; result=$(grep $search /usr/share/i18n/SUPPORTED) ; grep -v \"$result\" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null ; echo $result | sudo tee -a /etc/locale.gen > /dev/null")
	);
		strcpy(input_locale_gen, "search=\"^");
		strncat(input_locale_gen, lang, 9);
		strcat(input_locale_gen, "\" ; result=$(grep $search /usr/share/i18n/SUPPORTED) ; grep -v \"$result\" /etc/locale.gen | sudo tee /etc/locale.gen > /dev/null ; echo $result | sudo tee -a /etc/locale.gen > /dev/null");
	run_command(input_locale_gen);
	free(input_locale_gen);
}

// Ask if the user want to reboot X seconds, and do it.
void reboot_ask(uint16_t seconds_to_wait, uint16_t flags){
	char *prompt_input = (char *)malloc(0);
	if (flags & Flag_Turkish){
		fprintf(stderr, "\nDil paketi doğrulandı, ayarları uygulamak için baştan başlatmak ister misiniz? (e/h): ");
		scanf("%s", prompt_input);

		*prompt_input = tolower(*prompt_input);
		uint16_t i = 0;
		while(*prompt_input != 'e' && *prompt_input != 'h' && i < 4){
			fprintf(stderr, "\n Lütfen 'e' (evet) ve ya 'h' (hayır) yazınız: ");
			scanf("%s", prompt_input);
			i++;
		}
	}
	else{
		fprintf(stderr, "\nLocale is verified, do you want to reboot to apply settings? (y/n): ");
		scanf("%s", prompt_input);

		*prompt_input = tolower(*prompt_input);
		uint16_t i = 0;
		while(*prompt_input != 'y' && *prompt_input != 'n' && i < 4){
			fprintf(stderr, "\n Type 'y' (yes) or 'n' (no): ");
			scanf("%s", prompt_input);
			i++;
		}
	}
	if (*prompt_input == 'y' || *prompt_input == 'e'){
		free(prompt_input);
		// Reboot, same as:
		// $ echo "Reboot countdown:";for i in 1 2 3 4 5; do echo "$i"; sleep 1; done ; echo "Rebooting..."; sudo reboot now
		if (flags & Flag_Turkish)
			fprintf(stderr, "\nYeniden başlatma gerisayımı:\n");
		else
			fprintf(stderr, "\nReboot countdown:\n");

		// Countdown
		for (; seconds_to_wait > 0; seconds_to_wait--){
			printf("%d\n", seconds_to_wait);
			sleep(1);
		}
		if (flags & Flag_Turkish)
			fprintf(stderr, "Yeniden başlatılıyor...\n");
		else
			fprintf(stderr, "\nRebooting...\n");

		run_command("sudo reboot now");
	}
	else
	{
		// Do not reboot
		free(prompt_input);
	}
}

int main (int argc, char * const argv[]){
#ifdef DEBUG_LV
	clock_t begin = clock();
#endif

	FILE *current_locale_fp = fopen(LOCALE_FILENAME, "r");
	FILE *current_localegen_fp = fopen(LOCALEGEN_FILENAME, "a");

	// Check if script has permissions
	if (current_locale_fp != NULL && current_localegen_fp != NULL)
	{
		fclose(current_locale_fp);
		fclose(current_localegen_fp);

		uint16_t flags = Flag_Empty;
		
		// Get the locale LANG
		char *lang_value = run_command("cat /etc/default/locale 2> /dev/null | grep '^LANG=' | cut -d '=' -f2 | tr '\\n' '\\0'");

		set_language_flag(&flags, lang_value);
		
		extern char* optarg;
		int32_t opt;
		while ((opt = getopt(argc, argv, "vl:L")) != -1) {

			switch (opt) {
				case 'v':
					if (flags & Flag_Turkish)
						fprintf(stderr, "Sistem dili "STR_BOLD("dogrulanacak")".\n");
					else
						fprintf(stderr, "System language will be "STR_BOLD("verified")".\n");
					flags = Flag_Verify | Flag_Unique;
				break;

				case 'l':
					if ( !strcmp("tr", optarg) ){
						flags = Flag_Turkish;
						fprintf(stderr, "Sistem dili "STR_BOLD("Türkçe")"'ye "STR_BOLD("(Turkish)")" cevrilecek.\n");
					} else if ( !strcmp("en", optarg) ) {
						flags = Flag_English;
						fprintf(stderr, "System language will be changed to "STR_BOLD("English")".\n");
					} else {
						fprintf(stderr, "$ %s -l [\""STR_BOLD("en")"\", \""STR_BOLD("tr")"\"]\n", argv[0]);
					}
				break;

				default:
					#define _ARGS_ "[-vl]"
				
					if (flags & Flag_Turkish)
						fprintf(stderr, "Kullanimi: %s "_ARGS_" [deger]\n", argv[0]);
					else
						fprintf(stderr, "Usage: %s "_ARGS_" [value]\n", argv[0]);

					#undef _ARGS_
					exit(EXIT_FAILURE);
			}//switch

		if (flags & Flag_Unique){
			break;
		}
#ifdef DEBUG_LV
			SOUT("c", opt);
			SOUT("d", argc);
			SOUT("s", optarg);
			PRINT_BYTE(flags);
#endif
		} /*else {//opt
			flags = Flag_Verify;
			fprintf(stderr, "System language will be "STR_BOLD("verified")".\n");
		}*/



		if (!(flags & Flag_Verify)){
			lang_value = NULL;
			free(lang_value);	
		}

		if (flags & Flag_Turkish) {
			lang_value = (char *)malloc(sizeof(LANG_TR_TR));
			strcpy(lang_value, LANG_TR_TR);
		} else {
			lang_value = (char *)malloc(sizeof(LANG_EN_US));
			strcpy(lang_value, LANG_EN_US);
		}

		// Extract the LANGs needed to be installed after verifying
		size_t ln_count; // ln == locales_needed
		char **locales_needed = !strcmp(lang_value, LANG_TR_TR) ?
			extract_charmaps(LOCALE_TURKISH, &ln_count) : extract_charmaps(LOCALE_ENGLISH, &ln_count);

		// Check if the LANG is installed -> locale -a
		char *locale_installed;
		bool is_locale_gen_needed = false;
		for(uint16_t ln_i = 0; ln_i < ln_count-1; ln_i++){ //-1 for null

			// $ locale -a 2> /dev/null | grep -i LANG
			locale_installed = search_locale_installed(*(locales_needed+ln_i));

			if (*locale_installed == '\0'){
				if (flags & Flag_Turkish)
					fprintf(stderr, "'%s' yerel dil paketi yuklu degil.\n", *(locales_needed+ln_i));
				else
					fprintf(stderr, "Locale '%s' is not installed.\n", *(locales_needed+ln_i));
				is_locale_gen_needed = true;
				add_lang_to_localegen(*(locales_needed+ln_i));
			}
			free(locale_installed);

		}// for

		// Generate Locales if needed
		if (is_locale_gen_needed){
			if (flags & Flag_Turkish)
				fprintf(stderr, "Dil paketleri yukleniyor...\n");
			else
				fprintf(stderr, "Installing locales...\n");
			run_command("sudo locale-gen");
		}

		// Write to verified locale to /etc/default/locale
		if (write_lang_to_locale(lang_value, LOCALE_FILENAME))
			reboot_ask(5, flags); else printf("Verification failed.\n");
		
		free(lang_value);

	}// if file pointer is null
	else
	{
		fprintf(stderr,\
			STR_BOLD("(TR)")" Gerekli dosyalar acilamiyor, izinler red edildi!\n"\
			STR_BOLD("(EN)")" Cannot open the required files, permissions are denied!\n"\
		);
		exit(1);
	}
#ifdef DEBUG_LV
	clock_t end = clock();
	printf(STR_CYAN("Execution time") " = %lf\n", (double)(end - begin) / CLOCKS_PER_SEC);
#endif
	return 0;
}// main