//This file is auto-generated from ./type/string_type.c
#ifndef __TYPE_STRING_TYPE_C
#define __TYPE_STRING_TYPE_C
//EXPORT

typedef struct _line {
	int count;
	char string[BUFFER_SIZE];
	struct _line *next;
} line;

typedef struct _text {
	line* line;
	struct _text *prev;
	struct _text *next;
} text;


//PUBLIC
line* line_insert(line* current);
void line_set_string(line* head, char* string);
text* text_insert(text* current);
text* text_malloc(void);
#endif
