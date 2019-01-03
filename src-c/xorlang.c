
#include <stdio.c>
#include <string.c>
#include "xorlang.h"

/*********************************************/
/* ERROR HANDLING                            */
/*********************************************/


enum {
	ERROR_NONE = 0,
	ERROR_PARSE = 1,
};
int error_number = 0;
int error_num_param1 = 0;
const char *error_strings[] = { "Parse error at input pos %d." };
char error_msg[64];

char *xor_error() {
	switch(error_number) {
		case ERROR_NONE:
			return NULL;
		case ERROR_PARSE:
			sprintf(error_msg, error_strings[error_number - 1], error_num_param1);
			break;
	}
	return error_msg;
}

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

/** allocates a new token with the given attributes */
token_t *xor_token_alloc(int type, string_t src, int start, int end) {
	token_t *token = (token_t*)malloc(sizeof(token_t));
	token->prev = NULL;
	token->next = NULL;
	token->ref = xor_string_substring(src, start, end);
	token->offset = start;
	token->type = type;
}

/** add token to token list */
void xor_token_add(token_t **headptr, token_t *tok) {
	token_t *head = *headptr;
	if(head == NULL) {
		*headptr = tok;
	} else {
		token_t *seek = head->next;
		token_t *tail = head;
		while(seek != NULL) {
			tail = seek;
			seek = seek->next;
		}
		tail->next = t;
		t->prev = tail;
	}
}

/***********************/
/* TOKENIZER */

/** skips whitespace in string */
size_t xor_skip_whitespace(string_t *input, size_t index) {
	while(
		input->data[index] == '\t' ||
		input->data[index] == '\r' ||
		input->data[index] == '\n'
	) {
		//TODO handle EOF
		++index;
	}
	return index;
}

/** skips comments in string */
size_t xor_skip_comments(string_t *input, size_t index) {
	/* 
	 * eol ::= '\r\n' | '\n' | '\r'
	 * block_comment ::= '#-' .+? '-#'
	 * line_comment ::= '#' [^\r\n] ( eol | eof )
	 */
	
	if(input->data[index] == '#') {
		++index;
		//block comment?
		if(input->data[index] == '#' && input->data[index + 1] == '-') {
			index += 2;
			//TODO handle EOF
			//TODO find closing of block
		} else {
			//TODO handle EOF
			//find EOL
			char c;
			do {
				c = input->data[index++];
			} while(c != '\r' && c != '\n');
			/* remaining \r if existing, is handled by whitespace skip */
		}
	}
	return index;
}

/** tokenizes keywords from the input */
size_t xor_tok_keywords(token_t **headptr, string_t *input, size_t index) {
	/*
	 * Put longer keywords first, at least ones that use parts of later
	 * keywords or this will not work as expected. So "elseif" should be
	 * placed before "else".
	 */
	const char *keywords[] = {"fn", "end", "while", "next", "last", "if",
		"elseif", "else", "return", "and", "or"};
	const size_t keyword_count = 11; //could get this dynamically
	
	for(int i = 0; i < keyword_count; ++i) {
		char *keyword = keywords[i];
		size_t keyword_length = strlen(keyword);
		if(strncmp(&input->data[index], keyword, keyword_length) == 0) {
			/* This ensures it is not actually an identifer. */
			int testIndex = index + keyword_length;
			int valid = testIndex == input->length;
			if(!valid) {
				//must be followed by nonalphanumeric if not eof
				char testChar = input->data[testIndex];
				boolean isAlphaNum = testChar >= 'A' && testChar <= 'Z'
						|| testChar >= 'a' && testChar <= 'z'
						|| testChar >= '0' && testChar <= '9';
				valid = !isAlphaNum;
			}
			
			if(valid) {
				token_t *token = xor_token_alloc(TYPE_KEYWORD, input, index,
					index + keyword_length);
				xor_token_add(headptr, token);
				index = testIndex;
				break;
			}
		}
	}
	return index;
}

/** tokenizes symbols from the input */
size_t xor_tok_symbols(token_t **headptr, string_t *input, size_t index) {
	/*
	 * Put longer symbols first, at least ones that use parts of later
	 * symbols or this will not work as expected. So ">=" should be before ">".
	 */
	const char *symbols[] = { "==", "<>", "<=", ">=", "<<", ">>", "<", ">", "*", "-", "+", "|", "^", "/",
			"%", "&", "-", "!", ":", "(", ")", "[", "]", "=" };
	const size_t symbol_count = 24;
	for(int i = 0; i < symbol_count; ++i) {
		char *symbol = symbols[i];
		size_t symbol_length = strlen(symbol);
		if (strncmp(&input->data[index], symbol, symbol_length) == 0) {
			token_t *token = xor_token_alloc(TYPE_SYMBOL, input, index,
					index + symbol_length);
			xor_token_add(headptr, token);
			index += symbol_length;
			break;
		}
	}
	return index;
}

/** tokenizes identities */
size_t xor_tok_ident(token_t **headptr, string_t *input, size_t index) {
	//TODO
	return index;
}

/** tokenizes numbers */
size_t xor_tok_number(token_t **headptr, string_t *input, size_t index) {
	//TODO
	return index;
}

/** tokenizes strings */
size_t xor_tok_string(token_t **headptr, string_t *input, size_t index) {
	//TODO
	return index;
}

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
	token_t *head = NULL;
	
	string_t input;
	input.data = str;
	input.length = length;
	
	size_t next = 0;
	size_t curr = 0;
	while(pos < length) {
		//skip whitespace
		nextpos = xor_skip_whitespace(&input, pos);
		if(nextpos != pos) {
			pos = nextpos;
			continue;
		}
		//skip comments
		nextpos = xor_skip_comments(&input, pos);
		if(nextpos != pos) {
			pos = nextpos;
			continue;
		}
		//keywords
		nextpos = xor_tok_keywords(&head, &input, pos);
		if(nextpos != pos) {
			pos = nextpos;
			continue;
		}
		//symbols
		nextpos = xor_tok_symbols(&head, &input, pos);
		if(nextpos != pos) {
			pos = nextpos;
			continue;
		}
		//identity
		nextpos = xor_tok_ident(&head, &input, pos);
		if(nextpos != pos) {
			pos = nextpos;
			continue;
		}
		//number
		nextpos = xor_tok_number(&head, &input, pos);
		if(nextpos != pos) {
			pos = nextpos;
			continue;
		}
		//string
		nextpos = xor_tok_string(&head, &input, pos);
		if(nextpos != pos) {
			pos = nextpos;
			continue;
		}
		
		//set parse error
		error_number = ERROR_PARSE;
		error_num_param1 = pos;
		
		return NULL;
	}
	
	return head;
}


