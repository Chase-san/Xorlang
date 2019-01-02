package org.xorlang;

public class Token {
	public static final int TYPE_IDENTITY = 1;
	public static final int TYPE_NUMBER = 2;
	public static final int TYPE_STRING = 3;
	public static final int TYPE_SYMBOL = 4;
	public static final int TYPE_KEYWORD = 5;
	
	public Token prev;
	public Token next;
	
	public final int type;
	public final String ref;
	public final int offset;
	
	public Token(int type, String src, int start, int end) {
		this.type = type;
		this.ref = src.substring(start, end);
		this.offset = start;
	}
	
	// Java specific
	public String toString() {
		return ref;
	}
	
	
	// LINKED LIST FUNCTIONS
	
	//from head only
	protected void add(Token t) {
		Token seek = next;
		Token tail = this;
		while(seek != null) {
			tail = seek;
			seek = seek.next;
		}
		tail.next = t;
		t.prev = tail;
	}
	
	protected Token peek(int offset) {
		Token t = this;
		while(offset < 0) {
			//TODO handle null
			t = t.prev;
			++offset;
		}
		while(offset > 0) {
			//TODO handle null
			t = t.next;
			--offset;
		}
		return t;
	}
}
