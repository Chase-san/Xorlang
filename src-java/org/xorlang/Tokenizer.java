package org.xorlang;

import java.text.ParseException;

public class Tokenizer {
	private Token head;
	private String input;
	
	
	public Tokenizer() {
		
	}
	
	public Token getHead() {
		return head;
	}
	
	private void add(Token t) {
		if(head == null) {
			head = t;
		} else {
			head.add(t);
		}
	}
	
	public void setInput(String input) {
		this.input = input;
	}
	
	private int skipComments(int index) {
		/* 
		 * eol ::= '\r\n' | '\n' | '\r'
		 * block_comment ::= '##-' .+? '-##'
		 * line_comment ::= '#' [^\r\n] eol
		 */
		
		if(input.charAt(index) == '#') {
			++index;
			
			//block comment?
			if(input.charAt(index) == '#' && input.charAt(index + 1) == '-') {
				index += 2;
				
				//TODO find closing of block
				//TODO handle EOF
			} else {
				//find EOL
				//TODO handle EOF
				char c;
				do {
					c = input.charAt(index++);
				} while(c != '\r' && c != '\n');
				
				//check for windows extra \n
				if(c == '\r' && input.charAt(index) == '\n') {
					++index;
				}
				
			}
			
		}
		
		return index;
	}

	private int skipWhitespace(int index) {
		String whitespace = " \t\r\n";
		
		while(whitespace.indexOf(input.charAt(index)) != -1) {
			++index;
		}
		
		return index;
	}
	
	public void tokenize() throws ParseException {
		head = null;
		
		int length = input.length();
		int nextpos = 0;
		int pos = 0;
		while(pos < length) {
			//skip whitespace
			nextpos = skipWhitespace( pos);
			if(nextpos != pos) {
				pos = nextpos;
				continue;
			}
			//skip comments
			nextpos = skipComments( pos);
			if(nextpos != pos) {
				pos = nextpos;
				continue;
			}
			//keywords
			nextpos = tokenizeKeywords(pos);
			if(nextpos != pos) {
				pos = nextpos;
				continue;
			}
			//symbols
			nextpos = tokenizeSymbols(pos);
			if(nextpos != pos) {
				pos = nextpos;
				continue;
			}
			//identity
			nextpos = tokenizeIdentity(pos);
			if(nextpos != pos) {
				pos = nextpos;
				continue;
			}
			//number
			nextpos = tokenizeNumber(pos);
			if(nextpos != pos) {
				pos = nextpos;
				continue;
			}
			//string
			nextpos = tokenizeString(pos);
			if(nextpos != pos) {
				pos = nextpos;
				continue;
			}
			//throw exception on parse failure
			throw new ParseException("Invalid input at index " + pos + ".", pos);
		}
	}
	
	private int tokenizeIdentity(int index) {
		//  identity ::= [A-Za-z_][A-Za-z0-9_]*
		String check = "_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
		String tail = "0123456789";
		
		int start = index;
		if(check.indexOf(input.charAt(index)) != -1) {
			check = check + tail;
			//TODO handle EOF
			while(check.indexOf(input.charAt(++index)) != -1);
			add(new Token(Token.TYPE_IDENTITY, input, start, index));
		}
		
		return index;
	}
	
	private int tokenizeKeywords(int index) {
		String[] keywords = new String[] {
			"fn", "end", "while", "next", "last", "if", "elseif", "else", "return", "and", "or",	
		};
		for(String keyword : keywords) {
			if(input.startsWith(keyword, index)) {
				int testIndex = index + keyword.length();
				boolean valid = testIndex == input.length();
				if(!valid) {
					//must be followed by nonalphanumeric if not eof
					char testChar = input.charAt(testIndex);
					boolean isAlphaNum = testChar >= 'A' && testChar <= 'Z'
							|| testChar >= 'a' && testChar <= 'z'
							|| testChar >= '0' && testChar <= '9';
					valid = !isAlphaNum;
				}
				
				if(valid) {
					add(new Token(Token.TYPE_KEYWORD, input, index, index + keyword.length()));
					index = testIndex;
					break;
				}
			}
		}
		return index;
	}
	
	private int tokenizeNumber(int index) {
		String check = "0123456789";
		int start = index;
		if(check.indexOf(input.charAt(index)) != -1) {
			while(check.indexOf(input.charAt(++index)) != -1);
			if(input.charAt(index) == '.') {
				++index;
				while(check.indexOf(input.charAt(++index)) != -1);
				//TODO handle EOF
			}
			add(new Token(Token.TYPE_NUMBER, input, start, index));
		}
		return index;
	}
	
	private int tokenizeString(int index) {
		int start = index;
		if(input.charAt(index) == '\'') {
			
			while(true) {
				char c = input.charAt(++index);
				//TODO handle EOF
				//TODO handle escape characters
				if(c == '\'') {
					++index;
					break;
				}
			}
			add(new Token(Token.TYPE_STRING, input, start + 1, index - 1));
		}
		return index;
	}
	
	private int tokenizeSymbols(int index) {
		String[] symbols = new String[] { "==", "<>", "<=", ">=", "<<", ">>", "<", ">", "*", "-", "+", "|", "^", "/",
				"%", "&", "-", "!", ":", "(", ")", "[", "]", "=" };
		for (String symbol : symbols) {
			if (input.startsWith(symbol, index)) {
				add(new Token(Token.TYPE_SYMBOL,  input, index, index + symbol.length()));
				index += symbol.length();
				break;
			}
		}
		return index;
	}
}
