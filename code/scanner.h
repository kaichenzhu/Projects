#include <iostream>    //for outputing
#include <fstream>     //read the file
#include <tuple>       //this structure can store a 3D data.
#include <string>      //for outputing the string

using namespace std;

string BufferStr;  //BufferStr stores the charactors read from the file LPAS.txt
string Lexeme;    //Lexeme caches the string and digits, which is read from the BufferStr, temporarily.
char Buffer[4096];   //Buffer[4096] chaches each line of the LPAS.txt.
int i = 0;       //i is a flag in the string BufferStr, BufferStr[i] represents ith charactor in string BufferStr.
int j = 0;         //j is a flag in the struct token, Num[j] and Lexeme[j] represent jth number or lexeme in them.
int k = -1;        //k can added by itself, which can help us get next pair in token:findtoken.
enum State{ S1, S2, S3, S4, S5, S6, S7, S8, S9, S10, S11 }; //DFA state
enum State CurrentState = S1;  //initiate state;

//insert charactors from file into the string BufferStr. 
void readfile(){
	ifstream File("LPAS.txt");
	while (!File.eof()){
		File.getline(Buffer, 4096);
		for (int i = 0; i < strlen(Buffer); i++){
			BufferStr += Buffer[i];
		}
	}
}

class Error{
private:
	string ErrRoutine;    //there are some bugs exist in the report of error routine
	string ErrToken;      //there are some bugs exist in the report of error token
	                      //I can make an inprovement in the future.
public:
	void setErrRout(string routine){
		ErrRoutine = routine;
	}
	void setErrTok(string token){
		ErrToken = token;
	}
	void show(){
		cout << "terminating parse in routine: " << ErrRoutine << " on token: " << ErrToken;
		system("pause");
		exit(0);
	}
	Error(){}
	Error(string error){
		cerr << endl << error << endl;
	}
}; Error error;

//get the next charator in BufferStr.
char getToken(){
	return BufferStr[i++];
}

//token stores the token number, token and lexeme. 
struct token{
	int Num[4096];
	string Token[4096];
	string Lexeme[4096];

	void push(int num, string token, string lexeme){
		Num[j] = num;
		Token[j] = token;
		Lexeme[j] = lexeme;
		j++;
	}
};

token findtoken;   //initiate a token struct.

//scan the token from string Buffer
void scanner(){
	while (i < BufferStr.length()){
		//token in this place is a charactor got from the file.
		char token = getToken();
		switch (CurrentState)
		{
        //S1:read state, the scanner is ready to read a new token.
		case S1:
			//if read a space, scanner will skip it.
			if (token == ' '){
				CurrentState = S1;
			}
			//if read an alphabet, scanner will go to the state2, and stores 
			//the charactor in a string lexeme.
			if ((token > 64 && token < 91) || (token>96 && token < 123)){
				CurrentState = S2;
				Lexeme += token;
			}
			//if read a number, scanner will go to the state3, and stores 
			//the charactor in a string lexeme.
			if (token > 47 && token < 58){
				CurrentState = S3;
				Lexeme += token;
			}
			//if find some token that only has one charactor, scanner will 
			//insert this token in the struct findtokn.
			if (token == ')'){
				findtoken.push(6, ")", ")");
			}
			if (token == ';'){
				findtoken.push(8, ";", ";");
			}
			if (token == '.'){
				findtoken.push(9, ".", ".");
			}
			if (token == '+' || token == '-'){
				Lexeme = token;
				findtoken.push(10, "ADDOPTOK", Lexeme);
				Lexeme = "";
			}
			if (token == '*' || token == '/'){
				Lexeme = token;
				findtoken.push(11, "MULOPTOK", Lexeme);
				Lexeme = "";
			}
			if (token == '<' || token == '>' || token == '='){
				Lexeme = token;
				findtoken.push(12, "RELOPTOK", Lexeme);
				Lexeme = "";
			}
			//if the scanner read a ':', it will go to the S6 to judge whether it 
			//is a ':' or ":=".
			if (token == ':'){
				CurrentState = S6;
			}
			//if the scanner read a '(', it will go to the S7 to judge whether it 
			//is a '(' or some comments.
			if (token == '('){
				CurrentState = S7;
			}
			break;
		case S2:
			//if read an alphabet, scanner will stay in the state2, and stores 
			//the charactor in the string lexeme again.
			while ((token > 64 && token < 91) || (token>96 && token < 123) || token>47 && token < 58){
				Lexeme += token;
				token = getToken();
			}
			//if lexeme is a key word, scanner will insert it into the struct findtoken, and clear
			//the string lexeme in order to read the next token.
			if (Lexeme == "Program"){
				findtoken.push(13, "PROGTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "begin"){
				findtoken.push(14, "BEGTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "end"){
				findtoken.push(15, "ENDTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "const"){
				findtoken.push(16, "CONSTTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "var"){
				findtoken.push(17, "VARTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "while"){
				findtoken.push(18, "WHILETOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "do"){
				findtoken.push(19, "DOTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "if"){
				findtoken.push(20, "IFTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "then"){
				findtoken.push(21, "THENTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "integer" || Lexeme == "float" || Lexeme == "boolean"){
				findtoken.push(22, "BASTYPETOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "mod" || Lexeme == "div" || Lexeme == "and"){
				findtoken.push(11, "MULOPTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "or"){
				findtoken.push(10, "ADDOPTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "not"){
				findtoken.push(23, "NOTTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "write" || Lexeme == "writeln"){
				findtoken.push(24, "WRITETOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "read"){
				findtoken.push(25, "READTOK", Lexeme);
				Lexeme = "";
			}
			else if (Lexeme == "true" || Lexeme == "false"){
				findtoken.push(2, "LITTOK", Lexeme);
				Lexeme = "";
			}
			//if the lexeme is a ID token instead of a keyword, scanner will insert 
			//it into the struct findtoken, and then clear the lexeme.
			else{
				findtoken.push(1, "IDTOK", Lexeme);
				Lexeme = "";
			}
			//because the scanner is ready to read a new token, so, it return to the S1.
			CurrentState = S1;
			//we use the next charactor got from the file to judge if the read of ID token is over
			//after insert the Id token, we should over read this charactor again.
			i--;
			break;
		case S3:
			//if this charactor is still a number, we still add this number in the lexeme. 
			if (token > 47 && token < 58){
				Lexeme += token;
				CurrentState = S3;
			}
			//if the scanner read a '.' after a number, this token may be a real number, then 
			//the scanner will go to the state 4 to judge if this token is a real number.
			else if (token == '.'){
				Lexeme += token;
				CurrentState = S4;
			}
			//if the scanner read some other charactor, this token must be a integer, then 
			//scanner will store it in the struct findtoken, and clear the lexeme.
			//If scanner scan a alphabet after a number, it will report an error,
			//in future work, I should make an improvement.
			else{
				findtoken.push(2, "LITTOK", Lexeme);
				Lexeme = "";
				CurrentState = S1;
				i--;
			}
			break;
		case S4:
			//if scanner read a number, scanner will store it in the lexeme and then go to S5.
			if (token > 47 && token < 58){
				Lexeme += token;
				CurrentState = S5;
			}
			else{
				//because there should be a number after the charactor '.', if scanner don't read 
				// a number, this must be an illegal token.
				Error("it'an illegal token!");
				CurrentState = S1;
				i--;
			}
			break;
		case S5:
			//in this state,
			//we already have a real number in the lexeme. if the scanner still read a 
			//number, it will store it in the lexeme.
			if (token > 47 && token < 58){
				Lexeme += token;
				token = getToken();
				CurrentState = S5;
			}
			//if the next charactor of the real number isn't a number, scanner should finish reading
			//reading the real number and start to scan the next token.
			else {
				findtoken.push(2, "LITTOK", Lexeme);
				Lexeme = "";
				CurrentState = S1;
				i--;
			}
			break;
		case S6:
			//before scanner go to this state, it has read a ':', so, if it read a '=',
			//this token should be a assign token, otherwise it just be a ':'.
			if (token == '='){
				findtoken.push(4, "ASTOK", ":=");
				CurrentState = S1;
			}
			//because scanner need the next charactor got from the file to judge whether it has read 
			//a ':', so, it should return the last read. So, i should decrease by 1.
			else{
				findtoken.push(7, ":", ":");
				CurrentState = S1;
				i--;
			}
			break;
		case S7:
			//if the scanner read a'*' after '(', then it will go to the S10 to judge if this token
			//is a comment.
			if (token == '*'){
				CurrentState = S10;
			}
			//if the scanner read a'*' after '(', then it will go to the S8 to judge if this token
			//is a string.
			else if (token == '"'){
				Lexeme += token;
				findtoken.push(5, "(", "(");
				CurrentState = S8;
			}
			//if the scanner other charactor after '(', then this token is just a '('.
			else{
				findtoken.push(5, "(", "(");
				CurrentState = S1;
				i--;
			}
			break;
		case S8:
			//if the scanner read '"', it must have finished scanning this string token,
			//then it will go to the S9.
			if (token == '"'){
				Lexeme += token;
				CurrentState = S9;
			}
			//else the scanner will go on insert string in the string token.
			else{
				Lexeme += token;
				CurrentState = S8;
			}
			break;
		case S9:
			//if the canner read a '"', it will repeat this process untill it read a ')'.
			if (token == '"'){
				Lexeme += token;
				CurrentState = S9;
			}
			//if the scanner read a ')' after the '"', it will insert this string and the token ')'
			//in the struct findtoken .
			else if (token == ')'){
				findtoken.push(3, "STRLITTOK", Lexeme);
				findtoken.push(6, ")", ")");
				CurrentState = S1;
				Lexeme = "";
			}
			//if scanner read other charactor, it will return S8.
			else{
				CurrentState = S8;
			}
			break;
		case S10:
			//if the scanner read '"', it must have finished scanning this string token,
			//then it will go to the S9.
			if (token == '*'){
				CurrentState = S11;
			}
			//else the scanner will go on insert string in the string token.
			else{
				CurrentState = S10;
			}
			break;
		case S11:
			//if the canner read a '*', it will repeat this process untill it read a ')'.
			if (token == '*'){
				CurrentState = S11;
			}
			//if the scanner read a ')' after the '*', it will insert this string and the token ')'
			//in the struct findtoken .
			else if (token == ')'){
				CurrentState = S1;
			}
			//if scanner read other charactor, it will return S10.
			else{
				CurrentState = S10;
			}
			break;
		}
	}
}

//return the next token in the token struct:findtoken.
tuple<int, string, string> FindToken(){
	k++;
	return make_tuple(findtoken.Num[k], findtoken.Token[k], findtoken.Lexeme[k]);
}