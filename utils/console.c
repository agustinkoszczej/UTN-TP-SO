#include "console.h"

void wait_any_key() {
	fgetc(stdin);
}

void clear_screen() {
	system("clear");
}

void println(char* text) {
	printf("%s\n", text);
}

void println_with_clean(char* text) {
	clear_screen();
	println(text);
}

void show_title(char* title) {
	int length = string_length(title);
	char* separator = string_repeat('*', length);
	printf("%s\n%s\n%s\n\n", separator, title, separator);
}
