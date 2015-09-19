/*
Keywords: Forth, Machine lot, Zadeh
File : 4th.cpp
To compile you must install libreadline :
	# apt install libreadline-gplv2-dev 
	$ g++ 4th.cpp -std=c++14 -lreadline -Wfatal-errors -o 4th
*/

#include "4th.h"

using namespace  std;

// Stack class ===============================================================


void Stack::push(int v)
{ // ( -- a)
	Node * node = new Node;
	node->val = v;
	node->next = top;
	top = node;
}

int Stack::drop()
{
	if(top != NULL ) {
		int v = top->val;
		Node * node = top;
		top = top->next;
		delete node;
		return v;
	} else {
		msg_poor_stack("drop");
		return 0;
	}
}

void Stack::show()
{
	if(showstack) {
		cout << "Stack:\n";
		Node * node = top;
		while(node != NULL) {
			cout << "\t" << node->val << endl;
			node = node->next;
		}
	} 
}

void Stack::dup()
{ // (a -- aa)
	if(top != NULL) {
		Node * node = new Node;
		node->val = top->val;
		node->next = top;
		top = node;
	} 
	else 	msg_empty_stack("dup");
}

void Stack::swap()
{ // (a b -- b a)
	if(top != NULL && top->next != NULL) {
		Node * rank = top->next;
		int v = rank->val; rank->val = top->val; top->val = v;
	}
	else 	msg_empty_stack("swap");
}

void Stack::over()
{ // (a b -- a b a)
	if(top != NULL && top->next != NULL) {
		Node * rank = top->next;
		Node * node = new Node;
		node->val = rank->val;
		node->next = top;
		top = node;
	} 
	else 	msg_empty_stack("over");

}

void Stack::rot()
{ // (a b c -- c a b)
	if(top != NULL && top->next != NULL && top->next->next != NULL) {
		Node * t1 = top->next, * t2 = t1->next;
		int v = top->val; top->val = t1->val; t1->val = t2->val; t2->val = v;
	}
	else msg_poor_stack("rot");
}

void Stack::op(string tok)
{
	char c = tok.at(0);

	if(top != NULL && top->next != NULL) {
		int x = drop();
		int y = drop();
		int z;
		switch(c) {
			case '+': z = x + y;
				break;
			case '-': z = x - y;
				break;
			case '*': z = x * y;
				break;
			case '/': z = x / y;
				break;
			case '%': z = x % y;
				break;
			case '=': z = x == y ? -1 : 0;
				break;
			case '>': z = x > y ? -1 : 0;
				break;
			case '<': z = x < y ? -1 : 0;
		}
		push(z);
	}
	else msg_poor_stack("operator");
}
 
void Stack::print()
{
	if(top != NULL) {
	cout << top->val << " ok" << endl;
	drop();
	}
	else 	msg_empty_stack("'.'");
}

// Machine class =============================================================

void Machine::load(string filename = "")
{
	if(filename.empty()) {
		cout << "filename : ";
		cin >> filename;
	}
	string line;
	ifstream ifile(filename);
	while(getline(ifile, line)) read(line);
	ifile.close();
}

void Machine::word(stringstream& sstrm)
{
	string key, value;
	// cout << "in word, sstrm.str(): " << sstrm.str() << endl;
	sstrm >> key;
	if(is_word(key)) words.erase(key);
	string s(sstrm.str(), sstrm.tellg());
	size_t p = s.find_first_of(";");
	if(p != string::npos) {
		sstrm.clear();
		sstrm.str(s.substr(p + 1));
		value = s.substr(0, p);
		words.insert(pair<string, string>(key, value));
		cout << key << " " << value << " ; ok\n";
	} else {
		sstrm.clear();
		cout << "Bug in word: ';' absent\n";
	}
}

void Machine::printstr(stringstream& sstrm)
{
	string s(sstrm.str(), sstrm.tellg());
	size_t p = s.find_first_of("\"");
	if(p != string::npos) {
		sstrm.clear();
		sstrm.str(s.substr(p + 1));
		cout << s.substr(0, p);
	} else {
		sstrm.clear();
		cout << "Bug in .\" .. \" '\"' absent\n";
	}
}

void Machine::see(string w)
{
	if(is_word(w)) 
		cout << ": " << w << " " << words.find(w)->second << " ; ok\n";
}

bool Machine::is_word(string w)
{
	return words.find(w) != words.end();
}

// if <if true> then  <rest>
// if <if true> then <if false> else <rest> 
void Machine::conditional(stringstream& sstrm)
{
	string s(sstrm.str(), sstrm.tellg());
	// cout << "in conditional, sstrm.str(): " << s << endl;
	size_t pthen = s.find("then"); // position of 'then'
	size_t pelse = s.find("else"); // position of 'else'
	if(pthen  != string::npos) {
		sstrm.clear();
		if(pelse != string::npos) {
			sstrm.str(s.substr(pelse + 4));
			s.erase(pelse); // between if and 'else'
		} else {
			sstrm.str(s.substr(pthen + 4));
			s.erase(pthen); // between if and 'then'
		}
		if(top != NULL) {
			int cond = drop();
			if(cond != 0) read(s.substr(0, pthen));
			else 
				if(pelse != string::npos) 
					read(s.substr(pthen + 4));
		} else cout << "Bug condition: stack empty!\n";
	} else {
		sstrm.clear();
		cout << "Bug condition: 'then' absent! \n";
	}
} // Machine::conditional

// end begin do <todo> loop
void Machine::loop(stringstream& sstrm)
{
	string s(sstrm.str(), sstrm.tellg());
	// cout << "in loop, sstrm.str(): " << s << endl;
	size_t p = s.find("loop");
	if(p != string::npos) {
		sstrm.seekg(p + sstrm.tellg() + 4);
		if(top != NULL && top->next != NULL) {
			int begin = drop();
			int end = drop();
			string tok;
			s.erase(p);
			stringstream todo;
			stringstream loopstream("");
			int i = begin;
			while(true) {
				todo.clear();
				todo.str(s);
				while(todo >> tok) {
					if(tok == "i") loopstream << i << " ";
					else loopstream << tok << " ";
				}
				if(begin < end) i++; else i--;
				if(i == end) break;
			} // while true
			read(loopstream.str());	
		} else cout << "Bug do: need two numbers\n";
	} else {
		sstrm.str();
		cout << "Bug do: 'loop' absent:\n";
	}
	
} // Machine::loop

void Machine::help()
{
	int c;
	while(true) {
		cout << "bye to quit, showstack to toggle between verbose or not\n";
		cout << "\\ this is a comment\n";
		cout << "0: return - 1: stack - 2: operators - 3: words - ";
		cout << "4: conditionals - 5: loops\n";
		cin >> c;
		if(c == 0) break;
		switch (c) {
			case 1: 
				cout << "\nSTACK\n";
				cout << "dup: duplicate the top, a -> a a \n";
				cout << "drop: remove the top; 1 2 -> 1\n";
				cout << "swap: swap between the top and the second, a b -> b a\n";
				cout << "rot: rotate the 3 elements, 1 2 3 -> 3 1 2\n";
				cout << "over: duplicate the second, 1 2 -> 1 2 1\\nn";
				cout << ". drop and display the top\n";
				cout << ".\" <text> \" display <text>\n";
				cout << "cr new line\n";
				break;
			case 2:
				cout << "\nOPERATORS\n";
				cout << "12 8 + -> 20\n";
				cout << "12 8 - -> -4\n";
				cout << "12 8 * -> 96\n";
				cout << "5 23 / -> 4\n";
				cout << "5 23 % -> 3\n";
				break;
			case 3:
				cout << "\nWORDS\n";
				cout << ": word <definition> ;\n";
				cout << "Example: \n";
				cout << "\t: fibo over over + ;\n";
				cout << "see fibo shows the definition\n";
			case 4:
				cout << "\nCONDITIONALS\n";
				cout << "0 == false, true otherwise\n";
				cout << "1 2 = \\ 0 == false\n";
				cout << "1 1 = \\ -1 == true\n";
				cout << "<cond> if <to do if true> then <to do if false> else\n";
				cout << "Unlike Forth, can be used on the fly\n";
			case '5':
				cout << "\nLOOPS\n";
				cout << "<end> <begin> do <to do> loop\n";
				cout << "0 4 do drop loop -> drop for elements if any\n";
				cout << "5 0 do i dup * loop -> 0 1 4 9 16\n";
				break;
		}
		cout << endl;
	}
}
void Machine::read(string s)
{
	string tok, tok2;
	stringstream sstrm(s);
	while(sstrm >> tok) {
		if(tok == "\\") break;
		else if(tok == "load") load();
		else if(tok == "showstack") showstack = !showstack;
		else if(tok == "help") help();
		else if(tok == ".") print();
		else if(tok == ".\"") printstr(sstrm);
		else if(tok == "cr") cout << endl;
		else if(tok == "dup") dup();
		else if(tok == "drop") drop();
		else if(tok == "swap") swap();
		else if(tok == "over") over();
		else if(tok == "rot") rot();
		else if(tok == "+" || tok == "-" || tok == "*" || tok == "/" ||
			tok == "%" || tok == "=" || tok == "<" || tok == ">")
			op(tok);
		else if(tok == ":") word(sstrm);
		else if(tok == "see") { sstrm >> tok2; see(tok2);}
		else if(is_word(tok)) read(words.find(tok)->second);
		else if(tok == "if") conditional(sstrm);
		else if(tok == "do") loop(sstrm);
		else if(is_int(tok)) push(stoi(tok));
		else cout << "Bug: " << tok << " ?\n";
	} // while token
} // Machine::read


// main ======================================================================

void usage(string prog)
{
	cout << "Usage " << prog << " [options] [FILE] ...\n";
	cout << "Options : \n";
	cout << "\t-v verbose: diplay the stack contents all time\n";
	cout << "\t-h this help\n";
	cout << "\t-l FILE: load script file\n";
	exit(0);
}

int main(int argc, char * argv[])
{

	char opt; // for getopt
	int index; // for non options arguments
	Machine fz;
	// options
	while ((opt = getopt(argc, argv, "l:hv")) != -1) {
		switch (opt) {
			case 'l':
				fz.load(optarg);
				break;
			case 'h':
				usage(argv[0]);
				break;
			case 'v':
				fz.showstack = true;
				break;
			default: /* '?' */
				cout << "Usage: " << argv[0];
				cout << "[-l] [-h] [-v] [FILE]\n";
				exit(EXIT_FAILURE);
		} // switch
	} // while getopt
	// non option arguments
	for(index = optind; index < argc; index++) {
		fz.load(argv[optind]);
	}


	string line;
	char * buffer;
	// fz.load("zob.fz");
	cout << "\nLanguage Fz - 0.1 -  Mourad Arnout - GNU-GPL -\n";
	cout << "Type \"bye\" to quit\n";
	while((buffer = readline("\n> ")) != NULL) {
		if((strcmp(buffer,"bye") == 0)) break;
		add_history(buffer);
		line.assign(buffer);
		fz.read(line);
		fz.show();
	}

	free(buffer);
	return 0;
}

