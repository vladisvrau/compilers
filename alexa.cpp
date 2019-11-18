#include "symbols.cpp"
#include "DICIONARIO.h"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctype.h>


using namespace std;

class retorno_lexico{
    public:
        string lexema;
        int TOKEN;

        retorno_lexico(int tok, string lex){
            TOKEN = tok;
            lexema = lex;
        }

        retorno_lexico(){}

        retorno_lexico(int tok){
            TOKEN = tok;
            lexema = "";
        }
};
class Analisador_Lexico{

    private:

        vector <char> letras{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
							 'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','W','Y','Z' };
		vector <char> digitos{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
		vector <char> hexa{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', 'A', 'B', 'C', 'D', 'E', 'F'};
		vector <char> permitidos{ 'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                                  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','W','Y','Z',
								  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
								  '.', ',', ':', '[', ']', '+', '-', '*', '/', ';', '"', '\'', ' ', '\n', '\r', '\t', '>', '<', '=', '{', '}', '!', '&', '(', ')', '?', '_'};
		tabela_simbolos table;
        char* file_name;
		void return_C() {
            if (!this->entrada.eof())
			    this->entrada.seekg(-1, ios::cur);
			
		}//fim return_C

		bool pertence(vector<char> v, char c) {

			bool awnser = false;

			if (find(v.begin(), v.end(), c) != v.end()) awnser = true;

			return awnser;	
		
		}//fim pertence

    public:

		int line;
		ifstream entrada;

		Analisador_Lexico() {
		
            this->entrada.open("exemplo1.txt");
			this->line = 1;

		}//fim construtor

		Analisador_Lexico(char* entrada_nome) {
            this->file_name = entrada_nome;
			this->entrada.open(entrada_nome);
			this->line = 1;

		}//fim construtor

        void open(char* arq){
            this->entrada.close();
            this->entrada.open(arq);
            this->file_name = arq;
        }
        
        void close() {
            this->entrada.close();
            remove(this->file_name);
        }

        retorno_lexico number(string lexema) {
            char c;
            entrada >> noskipws >> c;
            if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);

            // cout << "if" << endl;
            if (!lexema.compare("0") && c == 'h') {
                // cout << "\t hexa" << endl;
                lexema.append(1, c);
                entrada >> noskipws >> c;
                if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);
                if (!pertence(hexa, c) || entrada.eof()) return retorno_lexico(LEX_ERROR);
                lexema.append(1, c);
                entrada >> noskipws >> c;
                if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);
                if (!pertence(hexa, c) || entrada.eof()) return retorno_lexico(LEX_ERROR);
                lexema.append(1, c);
                return retorno_lexico(T_VALUE, lexema);

            }//fim if
            // cout << "number" << endl;
            while(pertence(digitos, c) && !entrada.eof()) {
                lexema.append(1, c);
                entrada >> noskipws >> c;
                if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);
                // cout << '\t' << lexema << endl;
            }//fim while

            // cout << c<< endl;
            // cout << lexema << endl;
            return_C(); 
            // cout << c<< endl;
            // cout << lexema << endl;
            return retorno_lexico(T_VALUE, lexema);

        }//fim number

        retorno_lexico strings(string lexema) {
            int size = 0;
            char c;
            bool continua = true;
            do {
                entrada >> noskipws >> c;
                if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);

                if (c == '\'' && !entrada.eof()) { 
                    lexema.append(1, c);
                    size++;
                    entrada >> noskipws >> c;
                    if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);

                    if (entrada.eof()) continua = false;
                    else if (c != '\'' && !entrada.eof()) {
                        // cout << c << endl;
                        continua = false;
                    }
                    else {
                        lexema.append(1, c);
                        size++;
                    }
                }
                else if (c == '\n' && !entrada.eof()) {
                    line++;
                    return retorno_lexico(LEX_ERROR);
                }
                else if (pertence(permitidos, c) && !entrada.eof()) {
                    lexema.append(1, c);
                    size++;
                }
                else return retorno_lexico(UNEXPECTED_EOF);
                // cout << lexema << endl;
            } while (continua && size <= 255);

            if (size > 255) return retorno_lexico(LEX_ERROR);

            // cout << lexema << endl;
            return_C();
            return retorno_lexico(T_STRING_LITERAL, lexema);
        }

        retorno_lexico words(string lexema){
            char c;
            entrada >> noskipws >> c;
            if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);

            // cout << c << endl;
            while ((pertence(letras, c) || pertence(digitos,c) ||  c == '_') && !entrada.eof()) {
                lexema.append(1, c);
                entrada >> noskipws >> c;
                if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);
                // cout << c << endl;
                // cout << "\t" << lexema << endl;
            }//fim while
            
            int token = table.eh_palavra_reservada(lexema);
            if (token == 101) {
				token = T_NEW_ID;
                table.add_id(lexema, 'b', 0);
            }//fim if
            return_C();
            // cout << c << endl;
            // cout << lexema << endl;

            return retorno_lexico(token, lexema);
        }//fim words()

        int equals(string lexema) {
            char c;
            entrada >> noskipws >> c;
            if (!pertence(permitidos, c)) return INVALID_CHAR;
            if (c == '=' && !entrada.eof()) {
                lexema.append(1, c);
                return T_EQUALS;
            }
            return_C();
            return T_ASSIGN;
        }

        int greater_than(string lexema) {
            char c;
            entrada >> noskipws >> c;
            if (!pertence(permitidos, c)) return INVALID_CHAR;
            if (c == '=' && !entrada.eof()) {
                lexema.append(1, c);
                return T_GREATER_THAN_EQUAL;
            }
            return_C();
            return T_GREATER_THAN;
        }

        int lesser_than(string lexema) {
            char c;
            entrada >> noskipws >> c;
            if (!pertence(permitidos, c)) return INVALID_CHAR;
            if (c == '=' && !entrada.eof()) {
                lexema.append(1, c);
                return T_LESSER_THAN_EQUAL;
            }
            return_C();
            return T_LESSER_THAN;
        }

        int not_equal(string lexema) {
            char c;
            entrada >> noskipws >> c;
            if (!pertence(permitidos, c)) return INVALID_CHAR;
            if (c == '=' && !entrada.eof()) {
                lexema.append(1, c);
                return T_NOT_EQUAL;
            }
            return LEX_ERROR;
        }

        int comments() {
            char c;
            entrada >> noskipws >> c;
            // cout << c;
            if (!pertence(permitidos, c)) return INVALID_CHAR;
            while(pertence(permitidos, c)  && !entrada.eof()) {
                if (c == '\n') line++;
                while (c == '*') {
                    entrada >> noskipws >> c;
                    if (!pertence(permitidos, c)) return INVALID_CHAR;
                    // cout << c;
                    if (c == '/') return 0; //terminou o comentário
                }//fim if
                entrada >> noskipws >> c;
                if (!pertence(permitidos, c)) return INVALID_CHAR;
                // cout << c;
            }//fim while
            if (entrada.eof()) return UNEXPECTED_EOF;
			return INVALID_CHAR; //se tiver algum caractere não permitido
        }//fim comments()

        retorno_lexico start() {
            // cout << "Alexa Start!" << endl;
            string lexema;
            lexema = "";
            retorno_lexico token;

            char c;
            entrada >> noskipws >> c;
            if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);
            // cout << c << endl;
            while ((isspace(c) || c == '\n' || c == '\t' || c == '/') && !entrada.eof()) {

                // cout << c;
                if (c == '\n') line++;
                if (c == '/') { // comentario ou divisao
                    entrada >> noskipws >> c;
                    if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);
                    // cout << c;
                    if (c == '*') {
                        int error = comments();
                        if (error != 0) return retorno_lexico(error);
                    }//fim if
					
					else {
                        // cout << "divide" << endl;
						return_C();
						return retorno_lexico(T_DIVIDE);

					}  // divisão

                }//fim if

                entrada >> noskipws >> c;
                if (!pertence(permitidos, c)) return retorno_lexico(INVALID_CHAR);
                // cout << c << endl;

            }//fim while

            if (entrada.eof()) return retorno_lexico(EOF_NO_LEX);
            // cout << "sai while"<< endl;
            // cout << c << endl;
            if (pertence(digitos, c)) {
                // cout << "numbers" << endl;
                lexema.append(1, c);
                token = number(lexema);
            }//if
            else if (pertence(letras, c)) {
                // cout << "words" << endl;                
                lexema.append(1, c);
                // cout << lexema << endl;
                token = words(lexema);
            }
            else if (c == '_') {
                lexema.append(1, c);
                entrada >> noskipws >> c;
                while (c == '_' && !entrada.eof()) {
                    lexema.append(1, c);
                    entrada >> noskipws >> c;
                }//fim while
                if ((pertence(letras, c) || pertence(digitos, c)) && !entrada.eof()){
                    lexema.append(1, c);
                    token = words(lexema);
                }
                else return retorno_lexico(LEX_ERROR);
            }
            else if (c == '='  && !entrada.eof()) {
                lexema.append(1, c);
                token = retorno_lexico(equals(lexema));

            } 
            else if (c == '\'' && !entrada.eof()) {
                lexema.append(1, c);
                token = strings(lexema);
            }
            else if ((c == '<' || c == '>' || c == '!') && !entrada.eof()) {
                lexema.append(1, c);
                switch (c) {
                    case '>':
                        token = retorno_lexico(greater_than(lexema));
                        break;
                    case '<':
                        token = retorno_lexico(lesser_than(lexema));
                        break;
                    case '!':
                        token = retorno_lexico(not_equal(lexema));
                        break;
                    default:
                        token = retorno_lexico(LEX_ERROR);
                        break;
                }
            }
            else {
                switch (c) {
                    case '(':
                        token = retorno_lexico(T_OPEN_PARENTESIS);
                        break;
                    case ')':
                        token = retorno_lexico(T_CLOSE_PARENTESIS);
                        break;
                    case ',':
                        token = retorno_lexico(T_COMMA);
                        break;
                    case ';':
                        token = retorno_lexico(T_SEMICOLON);
                        break;
                    case '+':
                        token = retorno_lexico(T_ADD);
                        break;
                    case '-':
                        token = retorno_lexico(T_SUBTRACT);
                        break;
                    case '*':
                        token = retorno_lexico(T_MULTIPLY);
                        break;
                    default:
                        token = retorno_lexico(LEX_ERROR);
                        break;
                }
            }
            // cout << c << endl;
            return token;
        }

};//fim classe
