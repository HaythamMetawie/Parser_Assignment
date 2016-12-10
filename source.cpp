#include <iostream>
#include <string>
#include <cctype>
#include <fstream>
#include <vector>

using namespace std;


class scanner {
private:

        string RES_WORDS[8] = { "if","then","else","end","repeat","until","read","write" };
        enum states { START, INCOMMENT, INNUM, INID, INASSIGN, DONE };
        string mytoken;
        scanner()
        {
            cout << "scanner is created." << endl;
        }
        static scanner *s_inst;

public:
        std::vector <string> tokens;
        std::vector <string> values;
        static scanner* get_inst()
        {
            if(s_inst == NULL) { s_inst = new scanner();}
            return s_inst;
        }

        bool isSpace(char s) { return (s == ' ' || s == '\t' || s == '\n'); }

        bool isSymbol(char c) { return (c == '+' || '-' || '*' || '/' || '=' || '<' || '(' || ')' || ';' || ':='); }

        states state = START; //initial state

        void getToken(string l)
        {
            bool res_words = 0;
            int i = 0;
            while (state != DONE) {
                switch (state) {
                case START:
                    if (isSpace(l[i])) {
                        i++;
                        if (i == l.length()) state = DONE;
                        else state = START;
                    }
                    else if (isdigit(l[i])) {
                        state = INNUM;
                    }
                    else if (isalpha(l[i])) {
                        state = INID;
                    }
                    else if (l[i] == ':') {
                        state = INASSIGN;
                    }
                    else if (l[i] == '{') {
                        i++;
                        state = INCOMMENT;
                    }
                    else if (isSymbol(l[i])) {
						if (l[i] == ';') {
                         tokens.push_back(";"); values.push_back("semicolon");
                        }
						else
						{	string foo (1,l[i]);
							tokens.push_back(foo); values.push_back(foo);
						}

                        i++;
                        if (i == l.length()) state = DONE;
                        else state = START;
                    }
                    else state = DONE;
                    break;

                case INCOMMENT:
                    if (state == INCOMMENT) {
                        while (l[i] != '}') {
                            i++;
                        }
                        i++;
                        if (i == l.length()) state = DONE;
                        else state = START;
                    }
                    break;

                case INNUM:
                    while (isdigit(l[i])) {
                        mytoken += l[i];
                        i++;
                    }
                    tokens.push_back(mytoken);
                    values.push_back("number");
                    mytoken = "";
                    if (i == l.length()) state = DONE;
                    else state = START;
                    break;

                case INID:
                    while (isalpha(l[i])) {
                        mytoken += l[i];
                        i++;
                    }
                    for (int i = 0; i < 8; i++)
                    {
                        if (RES_WORDS[i] == mytoken) res_words = 1;
                    }
                    if (res_words){
                        tokens.push_back(mytoken);
                        values.push_back(mytoken);
                    }
                    else{
                         tokens.push_back(mytoken);
                         values.push_back("identifier");
                         }
                    mytoken = "";
                    res_words = 0;
                    if (i == l.length()) state = DONE;
                    else state = START;
                    break;

                case INASSIGN:
                    if (l[i] == ':') {
                        i += 2;
                        tokens.push_back(":=");
                        values.push_back("assign");
                        state = START;
                    }
                    else {
                        if (i == l.length()) state = DONE;
                        else state = START;
                    }
                    break;
                case DONE:
                    break;
                }
            }
        }

};

/******************************************************************************************************************/

class treeNode{
private:
    treeNode *r, *m, *l;
    string key, value;
public:
    treeNode(string k = "",string v = ""){key = k; value = v;}
    ~treeNode(){};
    void set_key(string k){key =k;}
    void set_value(string v) {value =v;}
    void set_right(treeNode *right) {r = right;}
    void set_mid(treeNode *mid) {m = mid;}
    void set_left(treeNode *left) {l = left;}
    string get_key() {return key;}
    string get_value() {return value;}
    treeNode *get_right() {return r;}
    treeNode *get_mid() {return m;}
    treeNode *get_left() {return l;}
};

 /***************************************************************************************************************/

scanner *scanner::s_inst = 0;

class parser{
private:
        //treeNode *root;
        parser()
        {
            i = 0;
            sr = scanner::get_inst();
            cout << "parser is created." << endl << "Wait until creating syntax tree!" <<endl;
        }
        static parser *p_inst;
        scanner *sr;
        int i;

         void match (string s)
        {
          cout << "new token " << s << " & " << sr->values.at(i) << endl;
          if (s == sr->values.at(i))
          {
              i++;
          }

        }
       

        void stmt_seq()
        {
            stmt();
            while (sr->tokens.at(i) == ";")
            {
                match("semicolon");
                stmt();
            }
        }


        void stmt()
        {

                if (sr->values.at(i) == "if") if_stmt();
                else if  (sr->values.at(i) == "repeat") repeat_stmt();
                //else if (sr->values.at(i) == ":=") assign_stmt();
                else if (sr->values.at(i) == "write") write_stmt();
                else if (sr->values.at(i) == "read") read_stmt();
				else if (sr->values.at(i) == "identifier") assign_stmt();
        }

        void if_stmt()
        {
            match("if");
            exp();
            match("then");
            stmt_seq();
            if (sr->values.at(i) == "else") stmt_seq();
            match("end");
        }

        void repeat_stmt()
        {
            match("repeat");
            stmt_seq();
            match("until");
            exp();
        }

        void assign_stmt()
        {
            match("identifier");
            match("assign");
  
			exp();
        }
		
        void read_stmt()
        {
            match("read");
            match("identifier");
        }

        void write_stmt()
        {
            match("write");
            exp();
        }

        void exp()
        {
           simple_exp();
           while (sr->tokens.at(i) == "<" || sr->tokens.at(i) == "=")
            {
                match(sr->values.at(i));
                simple_exp();
            }
        }

        void simple_exp()
        {
            term();
            while (sr->tokens.at(i) == "+" || sr->tokens.at(i) == "-")
            {
                match(sr->values.at(i));
                term();
            }
        }

        void term()
        {
            factor();
            while (sr->tokens.at(i) == "*" || sr->tokens.at(i) == "/")
            {

                match(sr->values.at(i));
                factor();
            }
        }

        void mul_op()
        {
            if (sr->tokens.at(i) == "*") match("*");
            if (sr->tokens.at(i) == "/") match("/");
        }

        void factor()
        {
            if(sr->tokens.at(i) == "(")
            {
                match("(");
                exp();
                match(")");
            }
            else if (sr->values.at(i) == "number")
            {
                match("number");
            }
            else if (sr->values.at(i) == "identifier")
            {
                match("identifier");
            }
        }


public:

        static parser* get_inst()
        {
            if(p_inst == NULL) { p_inst = new parser();}
            return p_inst;
        }
		
		void program()
		{
			stmt_seq();
			//treeNode root = new treeNode("Program");
			cout << "Start Parsing." << endl;
		}
};


/***********************************************************************************************************/
/**********************************************************************************************************/

parser *parser::p_inst = 0;

int main() {

	ifstream f;
	string line, code;

    f.open("test_code.txt");
	while (!f.eof()) {
		getline(f, line);
		code += line;
	}
	scanner *sr = scanner::get_inst();
    sr->getToken(code);
    parser *pr = parser::get_inst();
	pr->program();
	//system("pause");
    return 0;
}
