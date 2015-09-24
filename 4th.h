/*
Keywords: Forth, Machine lot, Zadeh
File : 4th.h
To compile you must install libreadline :
	# apt install libreadline-gplv2-dev 
	$ g++ machine.cpp -std=c++14 -lreadline -Wfatal-errors -o machine
http://www.grognon.net
*/
#define PROJECT "4th"
#define VERSION 0.2
#define AUTHOR "Mourad Arnout"



#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <map>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

using namespace  std;

void msg_poor_stack(string s) {
	cout << "Bug in " << s << ": Poor stack\n";
}

void msg_empty_stack(string s) {
	cout << "Bug in " << s << ": Empty stack\n";
}

void msg_absent(string s, string p) {
	cout << "Bug in " << s << ": " << p << " absent\n";
}

bool is_int(string s)
{
	stringstream ss(s);
	int i;
	return !(ss >> i).fail();
}


// Node struct ===============================================================

struct Node {
	int val;
	struct Node * next;
};


// Stack class ===============================================================

class Stack
{
public:
	// stack manipulations
	// Forth notation (before -- afet)
	void push(int);	// push int ( -- f)
	int  drop();		// remove (a -- )
	void dup();			// duplicate (a -- a a)
	void swap();		// swap top with second ( -- )
	void over();		// duplicate the second on the top ( -- a)
	void rot();			// rotate the top three elements 1 2 3 -> 3 1 2
	// numeric operations
	void op(string);	// numeric operations : + - * / %	
	// utilities
	void print();		// remove and display the top (a -- )
	void show();		// show the stack

	bool showstack = false; // toggle 
protected:
	Node * top = NULL;
};

// Machine class ===============================================================

class Machine: public Stack
{
public:
	void read(string);	// interpreter
	void load(string);	// load file
	void word(stringstream&);	// store a word 
	bool is_word(string);	// search if word
	void see(string);		// display word  definition
	void conditional(stringstream&); // # if .. then [.. else]
	void loop(stringstream&);	// # # do .. loop
	void printstr(stringstream&);
	void help();
private:
	Stack rstack;
	map<string, string> words; // dict of words

}; // Machine


