#ifndef __XORLANG__
#define __XORLANG__

typdef struct string string_t;
struct string {
	char *data;
	size_t length;
};

enum {
	TYPE_IDENTITY = 1,
	TYPE_NUMBER = 2,
	TYPE_STRING = 3,
	TYPE_SYMBOL = 4,
	TYPE_KEYWORD = 5,
};

typedef struct token token_t;
struct token {
	/* linked list */
	token_t *prev;
	token_t *next;
	/* token data */
	string_t ref;
	int type;
	int offset;
};

/** peeks before or after in the token list */
token_t *xor_token_peek(token_t *current, int offset);

/** frees the token list */
void xor_token_free(token_t *head);

/** tokenize the given string */
token_t *xor_tokenize(char *str, size_t length);

#endif
