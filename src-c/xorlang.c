
#include "xorlang.h"


/*********************************************/
/* PRIVATE                                   */
/*********************************************/

/***********************/
/* STRINGS */

/** allocates a new string_t */
string_t *xor_string_alloc() {
	string_t *str = (string_t*)malloc(sizeof(string_t));
	str->data = NULL;
	str->length = 0;
	return str;
}

/** wrap a char/length in a string_t */
string_t *xor_string_wrap(char *str, size_t length) {
	string_t *str = xor_string_alloc();
	str->data = str;
	str->length = length;
	return str;
}

/** free a string_t */
void xor_string_free(string_t *str) {
	free(str);
}

/** substrings of a string_t */
string_t *xor_string_substring(string_t src, int start, int end) {
	return xor_string_wrap(src->data + start, end - start);
}

/***********************/
/* TOKENS */
token_t *xor_token_alloc(int type, string_t src, int start, int end) {
	token_t *token = (token_t*)malloc(sizeof(token_t));
	token->prev = NULL;
	token->next = NULL;
	token->ref = xor_string_substring(src, start, end);
	token->offset = start;
	token->type = type;
}

/** add token to token list */
void xor_token_add(token_t *head, token_t *tok) {
	token_t *seek = head->next;
	token_t *tail = head;
	while(seek != NULL) {
		tail = seek;
		seek = seek->next;
	}
	tail->next = t;
	t->prev = tail;
}

/***********************/
/* TOKENIZER */



/*********************************************/
/* PUBLIC                                    */
/*********************************************/


/** peeks before or after in the token list */
token_t *xor_token_peek(token_t *current, int offset) {
	token_t *t = current;
	while(offset < 0) {
		//TODO handle null
		t = t->prev;
		++offset;
	}
	while(offset > 0) {
		//TODO handle null
		t = t->next;
		--offset;
	}
	return t;
}

/** frees the token list */
void xor_token_free(token_t *head) {
	
}

/** tokenize the given string */
token_t *xor_tokenize(char *str, size_t length) {
	
}

