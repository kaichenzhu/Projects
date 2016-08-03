#include "scanner.h"   //scanner 
#include "SymbolTable.h"  //symbol table 
#include <sstream> //for conversing the type:integer to type:string 

tuple<int, string, string>Token;   //Token information: Token number; Token; Lexeme.
sym ST_Roc;        //create an object of symbol table calss:ST_Roc 
int CurrentOffset = 0;
string operat;     //operations: +,-,>....... 
static int curlabel = 1;   //for create different string name
string label;     //string name in mips:StrLabel1,StrLabel2.etc.
ST_Rocord St_ptr;   //record the information of the id got form the symbol table and it's a struct.
int CopyofCurrOff;   //store the currentoffset, assign it to currentoffset after closing a block;
vector<string> FloatData;    //sotre float data. For example: StrLabel1 .float 2.22;

//define a struct which can record the location and type of the expression
struct Exp_Rocord{
	int location;
	char type;
};

// look at a literal token and decide its type for getting the type of the symbol and Little token.
char getType(string token) {
	char ch;
	if (token[0] == 't' || token[0] == 'f')  ch = 'b';
	else if (token.find('.') != -1) ch = 'f';
	else ch = 'i';
	return ch;
}

//write the mips code into the codefile.txt
void CodeGen(string mips){
	ofstream codefile("codefile.txt", ios::app);
	codefile << mips << endl;
}

void PrologGen(){
	ofstream codefile("codefile.txt", ios::app);
	codefile << "#Prolog:" << endl;
	codefile << ".text" << endl;
	codefile << ".globl  main" << endl;
	codefile << "main:" << endl;
	codefile << "move  $fp  $sp" << endl;
	codefile << "la  $a0  ProgStart" << endl;
	codefile << "li  $v0 4" << endl;
	codefile << "syscall" << endl;
	codefile << "#End of Prolog" << endl;
}

void PostlogGen(){
	ofstream codefile("codefile.txt", ios::app);
	codefile << "#Postlog:" << endl;
	codefile << "la  $a0  ProgEnd" << endl;
	codefile << "li  $v0  4" << endl;
	codefile << "syscall" << endl;
	codefile << "li  $v0  10" << endl;
	codefile << "syscall" << endl;
	codefile << ".data" << endl;
	//traverse the vector to get all of the float data and write them into the mips file;
	//For example:StrLabel1 .float 2.22
	//            StrLabel2 .float 32.1;
	vector<string>::iterator it = FloatData.begin();
	for (; it != FloatData.end(); it++)
		codefile << *it << endl;
	codefile << "ProgStart:  .asciiz \"Program Start\\n\" " << endl;
	codefile << "ProgEnd:   .asciiz	 \"Program  End\\n\" " << endl;
	codefile.close();
}

//each time I call this function, I will get a new string StrLabel1,StrLabel2,StrLabel3........
string GenStringLabel(){
	stringstream CurLabel;
	CurLabel << curlabel;
	string str = "StrLabel" + CurLabel.str();
	curlabel++;
	return str;
}

//compare the tokennumber with the specified number, If match, then Token get the new value.
void Match(int tokennumber){
	if (tokennumber == 14){
		ST_Roc.BlockNum += 1;
		ST_Roc.scopenum.push_back(ST_Roc.BlockNum);
		//assign the currentoffste to CopyofCurrOff. 
		CopyofCurrOff = CurrentOffset;
	}
	if (tokennumber == 15){
		ST_Roc.scopenum.pop_back();
		//assign the CopyofCurrOff to currentoffste.
		CurrentOffset = CopyofCurrOff;
	}
	if (get<0>(Token) == tokennumber){
		Token = FindToken();
	}
	else{
		error.setErrTok(get<1>(Token));
	}
}

void Program(int tokennumber);
void CONSTPART(int tokennumber);
void VARPART(int tokennumber);
void moreconstdecls(int tokennumber);
void morevardecls(int tokennumber);
void vardecl(int tokennumber);
void constdecl(int tokennumber);
void morestats(int tokennumber);
void statmt(int tokennumber);
void assignstat(int tokennumber);
void ifstat(int tokennumber);
void readstat(int tokennumber);
void writestat(int tokennumber, Exp_Rocord &ex);
void whilest(int tokennumber);
void blockst(int tokennumber);   
void writeexp(int tokennumber, Exp_Rocord &ex);
void express(int tokennumber, Exp_Rocord &ex);
void expprime(int tokennumber, Exp_Rocord &ex);
void term(int tokennumber, Exp_Rocord &ex);
void termprime(int tokennumber, Exp_Rocord &ex);
void relfactor(int tokennumber, Exp_Rocord &ex);
void factorprime(int tokennumber, Exp_Rocord &ex);
void factor(int tokennumber, Exp_Rocord &ex);
void idnonterm(int tokennumber, ST_Rocord &rec);
/*Tokennumber:1:IDTOK 2:LITTOK 3:STRLITTOK 4:ASTOK 5:( 
              6:) 7:: 8:; 9:. 10:ADDOPTOK
			  11:MULOPTOK 12:RELOPTOK 13:PROGTOK 14:BEGTOK 15:ENDTOK
			  16:CONSTTOK 17:VARTOK 18:WHILETOK 19:DOTOK 20:IFTOK 
			  21:THENTOK 22:BASTYPETOK 23:NOTTOK 24:WRITETOK 25:READTOK*/
//1 program :PROGTOK IDTOK ¡®(¡®¡®)¡¯CONSTPART VARPART BEGTOK stat morestats  ENDTOK IDTOK'.'
void Program(int tokennumber){
	error.setErrRout("Program");
	switch (tokennumber)
	{
	case 13:
		PrologGen();
		printf("1 ");
		ST_Roc.scopenum.push_back(ST_Roc.BlockNum);
		Match(13);
		Match(1);
		Match(5);
		Match(6);
		Match(8);
		CONSTPART(get<0>(Token));
		VARPART(get<0>(Token));
		Match(14);
		statmt(get<0>(Token));
		morestats(get<0>(Token));
		Match(15);
		Match(9);
		PostlogGen();
		break;
	default:
		error.show();
		break;
	}
}

//2 | 3 CONSTPART	:  CONSTTOK  constdecl moreconsdecls | <empty>
void CONSTPART(int tokennumber){
	error.setErrRout("CONSTPART");
	switch (tokennumber)
	{
	case 16:
		printf("2 ");
		Match(16);
		constdecl(get<0>(Token));
		moreconstdecls(get<0>(Token));
		break;
	default:
		printf("3 ");
		break;
	}
}

//4|5 VARPAET	:  VARTOK vardecl morevardecls  |  <empty>
void VARPART(int tokennumber){
	error.setErrRout("VARPART");
	switch (tokennumber)
	{
	case 17:
		printf("4 ");
		Match(17);
		vardecl(get<0>(Token));
		morevardecls(get<0>(Token));
		break;
	default:
		printf("5 ");
		break;
	}
}

//6|7 moreconstdecls  :  constdecl moreconsdecls   |    <empty>
void moreconstdecls(int tokennumber){
	error.setErrRout("moreconstdecls");
	switch (tokennumber)
	{
	case 1:
		printf("6 ");
		printf("moreconsdecls\n");
		constdecl(get<0>(Token));
		moreconstdecls(get<0>(Token));
		break;
	default:
		printf("7 ");
		break;
	}
}

//8|9 morevardecls	: vardecl morevardecls  |    <empty>
void morevardecls(int tokennumber){
	error.setErrRout("morevardecls");
	switch (tokennumber)
	{
	case 1:
		printf("8 ");
		vardecl(get<0>(Token));
		morevardecls(get<0>(Token));
		break;
	default:
		printf("9 ");
		break;
	}
}

//10 vardecl   :   IDTOK ':' BASTYPETOK  ';'
void vardecl(int tokennumber){
	error.setErrRout("vardecl");
	switch (tokennumber)
	{
	case 1:
		printf("10 ");
		if (ST_Roc.find_in_Current(get<2>(Token), ST_Roc.scopenum.back())){
			Error("Same variable has been declared in this block! ");
		}
		else{
			//ST_Roc is the object of class symbol table, insert is the method of class sym which can 
			//insert the (lexemes,scope number,current offset,and type of declaration) into the vector V.
			ST_Roc.insert(get<2>(Token), ST_Roc.scopenum.back(), CurrentOffset, "Var");
			CurrentOffset -= 4;
		}
		Match(1);
		Match(7);
		//inset the type of the token to the symbol table. V is a vector which stores the  
		//information of the declared symbols.
		V.back().type = get<2>(Token)[0];
		Match(22);
		Match(8);
		break;
	default:
		error.show();
		break;
	}
}

//11 constdecl	:  IDTOK '=' LITTOK ';'
void constdecl(int tokennumber){
	error.setErrRout("constdecl");
	switch (tokennumber)
	{
	case 1:
		printf("11 ");
		if (ST_Roc.find_in_Current(get<2>(Token), ST_Roc.scopenum.back())){
			Error("Same variable has been declared in this block! ");
		}
		else{
			//ST_Roc is the object of class symbol table, insert is the method of class sym which can 
			//insert the (lexemes,scope number,current offset,and type of declaration) into the vector V.
			ST_Roc.insert(get<2>(Token), ST_Roc.scopenum.back(), CurrentOffset, "Const");
			CurrentOffset -= 4;
		}
		Match(1);
		Match(12);
		Match(2);
		Match(8);
		break;
	default:
		error.show();
		break;
	}
}

//12|13 morestats  :   ¡®;¡± statmt   morestats    |    <empty>
void morestats(int tokennumber){
	error.setErrRout("morestats");
	switch (tokennumber)
	{
	case 8:
		printf("12 ");
		Match(8);
		statmt(get<0>(Token));
		morestats(get<0>(Token));
		break;
	default:
		printf("13 ");
		break;
	}
}

//14|15|16|17|18|19 statmt :  assignstat|  ifstat|  readstat|  writestat
//| blockst | whilest
void statmt(int tokennumber){
	error.setErrRout("statmt");
	Exp_Rocord ex;
	switch (tokennumber)
	{
	case 1:
		printf("14 ");
		assignstat(get<0>(Token));
		break;
	case 20:
		printf("15 ");
		ifstat(get<0>(Token));
		break;
	case 25:
		printf("16 ");
		readstat(get<0>(Token));
		break;
	case 24:
		printf("17 ");
		writestat(get<0>(Token), ex);
		break;
	case 17:
	case 14:
		printf("18 ");
		blockst(get<0>(Token));
		break;
	case 18:
		printf("19 ");
		whilest(get<0>(Token));
		break;
	default:
		error.show();
		break;
	}
}

//20 assignstat  :  idnonterm  ASTOK express
void assignstat(int tokennumber){
	error.setErrRout("assignstat");
	Exp_Rocord ex;      //Exp_Rocord is a strut which records the location and type of the expression.
	stringstream ex_location;   //for converting the type of ex.location to string.
	stringstream ST_location;   //for converting the type of location of the symbols to string.
	switch (tokennumber)
	{
	case 1:
		printf("20 ");
		idnonterm(get<0>(Token), St_ptr);  //now ST_Ptr ¨C if not null ¨C has type and location
		ST_location << St_ptr.location;
		Match(4);
		express(get<0>(Token), ex);     // now ex has a type and a location
		ex_location << ex.location;   //load the location of the expression to stringstream ex_location.
		if (St_ptr.type != ex.type) Error("types mismatch in assignment");
		if (ex.type == 'i'){
			CodeGen("lw,$t0," + ex_location.str() + "($fp)");
			CodeGen("sw,$t0," + ST_location.str() + "($fp)");
		}
		if (ex.type == 'f'){
			CodeGen("l.s $f1 " + ex_location.str() + "($fp)");
			CodeGen("s.s $f1 " + ST_location.str() + "($fp)");
		}
		break;
	default:
		error.show();
		break;
	}
}

//21 ifstat  :  IFTOK express THENTOK  stat 
void ifstat(int tokennumber){
	Exp_Rocord ex;
	error.setErrRout("ifstat");
	string label1;   
	switch (tokennumber)
	{
	case 20:
		printf("21 ");
		Match(20);
		express(get<0>(Token), ex);
		label = GenStringLabel();
		CodeGen("beq $t0 $0 " + label);
	    label1 = label;
		Match(21);
		statmt(get<0>(Token));
		CodeGen(label1 + ": ");
		break;
	default:
		error.show();
		break;
	}
}

//22 readstat   :  READTOK '(' idnonterm ')' 
void readstat(int tokennumber){
	stringstream ST_location;
	error.setErrRout("readstat");
	switch (tokennumber)
	{
	case 25:
		printf("22 ");
		Match(25);
		Match(5);
		idnonterm(get<0>(Token), St_ptr);
		if (St_ptr.declartype == "Var"){
			ST_location << St_ptr.location;
			if (St_ptr.type == 'i'){
				CodeGen("li $v0 5");
				CodeGen("syscall");
				CodeGen("sw,$v0," + ST_location.str() + "($fp)");
			}
			else if (St_ptr.type == 'f'){
				CodeGen("li $v0 6");
				CodeGen("syscall");
				CodeGen("s.s $f0," + ST_location.str() + "($fp)");
			}
			else if (St_ptr.type == 'b'){
				Error("Do not allow read boolean.");
			}
		}
		else if (St_ptr.declartype == "Const"){
			Error("Do not allow read into const value.");
		}
		Match(6);
		break;
	default:
		error.show();
		break;
	}
}

//23 writestat   :  WRITETOK '('  writeexp ')' 
void writestat(int tokennumber, Exp_Rocord &ex){
	error.setErrRout("writestat");
	bool isWritelin = false;
	stringstream ex_location;
	if (get<2>(Token) == "writeln")
		isWritelin = true;
	switch (tokennumber)
	{
	case 24:
		printf("23 ");
		Match(24);
		Match(5);
		writeexp(get<0>(Token), ex);
		ex_location << ex.location;
		if (ex.type == 's'){
			CodeGen("la $a0 " + label); //compiler has stored the string in the label.
			CodeGen("li $v0 4");
			CodeGen("syscall");
		}
		if (ex.type == 'i'){
			CodeGen("lw $a0 " + ex_location.str() + "($fp)");
			CodeGen("li $v0 1");
			CodeGen("syscall");
		}
		if (ex.type == 'f'){
			CodeGen("l.s $f12 " + ex_location.str() + "($fp)");
			CodeGen("li $v0 2");
			CodeGen("syscall");
		}
		if (ex.type == 'b'){
			Error("Don't allow write the boolean type");
		}
		Match(6);
		break;
	default:
		error.show();
		break;
	}
}

//24 whilest  :  WHILETOK express DOTOK stat
void whilest(int tokennumber){
	Exp_Rocord ex;
	string TopWhile;
	error.setErrRout("whilest");
	switch (tokennumber)
	{
	case 18:
		printf("24 ");
		TopWhile = GenStringLabel();
		CodeGen(TopWhile + ": ");
		Match(18);
		express(get<0>(Token), ex);
		label = GenStringLabel();
		CodeGen("beq $t0 $0 " + label);
		Match(19);
		statmt(get<0>(Token));
		CodeGen("j " + TopWhile);
		CodeGen(label + ": ");
		break;
	default:
		error.show();
		break;
	}
}

//25 blockst    :  varpart BEGINTOK   stats   ENDTOK 
void blockst(int tokennumber){
	error.setErrRout("blockst");
	switch (tokennumber)
	{
	case 17:
	case 14:
		printf("25 ");
		VARPART(get<0>(Token));
		Match(14);
		statmt(get<0>(Token));
		morestats(get<0>(Token));
		Match(15);
		break;
	default:
		error.show();
		break;
	}
}

//26|27 writeexp   :  STRLITTOK  |  express
void writeexp(int tokennumber, Exp_Rocord &ex){
	error.setErrRout("writeexp");
	switch (tokennumber)
	{
	case 3:
		printf("26 ");
		CodeGen(".data");
		label = GenStringLabel();    //assign the string to label.
		CodeGen(label + ": .asciiz" + get<2>(Token));
		CodeGen(".text");
		ex.type = 's';
		Match(3);
		break;
	case 23:
	case 1:
	case 2:
		printf("27 ");
		express(get<0>(Token), ex);
		break;
	}
}

//28 express   :  term expprime   
void express(int tokennumber, Exp_Rocord &ex){
	error.setErrRout("express");
	switch (tokennumber)
	{
	case 23:
	case 1:
	case 2:
	case 5:
		printf("28 ");
		term(get<0>(Token), ex);
		//pass ex to expprime.
		expprime(get<0>(Token), ex);
		break;
	default:
		error.show();
		break;
	}
}

//29|30 expprime    :  ADDOPTOK  term expprime   |  <empty>  
void expprime(int tokennumber, Exp_Rocord &ex){
	error.setErrRout("expprime");
	Exp_Rocord lhs;     //left operand.
	string oper;        //the operation in mips.
	string op;          //the operation got from the scanner.
	stringstream lhs_location;  //for converting the type of the lhs.location to string.
	stringstream ex_location; //for converting the type of the expression.location to string.
	stringstream CurrOff;    //for converting the type of the current offset to string.
	switch (tokennumber){
	case 10:
		printf("29 ");
		op = get<2>(Token);
		lhs = ex;  //get the left operand
		Match(10);
		term(get<0>(Token), ex);   //get the right operand
		lhs_location << lhs.location;
		ex_location << ex.location;
		CurrOff << CurrentOffset;
		if ((lhs.type == 'i' && ex.type == 'i') || (lhs.type == 'b' && ex.type == 'b')){
			if (op == "+") oper = "add";
			else if (op == "-") oper = "sub";
			else if (op == "or") oper = "or";
			CodeGen("lw,$t0," + lhs_location.str() + "($sp)");
			CodeGen("lw,$t1," + ex_location.str() + "($sp)");
			CodeGen(oper + ",$t0 $t0 $t1");
			CodeGen("sw,$t0," + CurrOff.str() + "($sp)");
			ex.location = CurrentOffset;
			CurrentOffset -= 4;
		}
		else if ((lhs.type == 'f' || lhs.type == 'i') && (ex.type == 'f' || ex.type == 'i')){
			if (op == "+") oper = "add.s";
			else if (op == "-") oper = "sub.s";
			if (lhs.type == 'f' && ex.type == 'f'){
				CodeGen("l.s $f1," + lhs_location.str() + "($sp)");
				CodeGen("l.s $f2," + ex_location.str() + "($sp)");
			}
			else if (lhs.type == 'f' && ex.type == 'i'){
				CodeGen("l.s $f1," + lhs_location.str() + "($sp)");
				CodeGen("lwc1 $f0 " + ex_location.str() + "($sp)");
				CodeGen("cvt.s.w $f2 $f0");
			}
			else if (lhs.type == 'i' && ex.type == 'f'){
				CodeGen("l.s $f1," + ex_location.str() + "($sp)");
				CodeGen("lwc1 $f0 " + lhs_location.str() + "($sp)");
				CodeGen("cvt.s.w $f2 $f0");
			}
			CodeGen(oper + " $f1 $f2 $f1");
			CodeGen("s.s $f1," + CurrOff.str() + "($sp)");
			ex.type = 'f';
			ex.location = CurrentOffset;
			CurrentOffset -= 4;
		}
		else Error("type mismatch in expressprime");
		expprime(get<0>(Token), ex);
		break;
	default:
		printf("30 ");
		break;
	}
}

//31 term    :  relfactor termprime
void term(int tokennumber, Exp_Rocord &ex){
	error.setErrRout("term");
	switch (tokennumber)
	{
	case 23:
	case 1:
	case 2:
	case 5:
		printf("31 ");
		relfactor(get<0>(Token), ex);
		termprime(get<0>(Token), ex);
		break;
	default:
		error.show();
		break;
	}
}

//32|33 termprime :  MULOPTOK  relfactor termprime  |  <empty> 
void termprime(int tokennumber, Exp_Rocord &ex){
	error.setErrRout("termprime");
	Exp_Rocord lhs;      //left operand 
	string oper;        //the operation in mips.
	string op;          //the operation got from the scanner.
	stringstream lhs_location;     //for converting the type of the lhs.location to string.
	stringstream ex_location; //for converting the type of the expression.location to string.
	stringstream CurrOff;    //for converting the type of the current offset to string.
	switch (tokennumber)
	{
	case 11:
		printf("32 ");
		op = get<2>(Token);
		lhs = ex;
		Match(11);
		relfactor(get<0>(Token), ex);
		lhs_location << lhs.location;
		ex_location << ex.location;
		CurrOff << CurrentOffset;
		if ((lhs.type == 'i' && ex.type == 'i') || (lhs.type == 'b' && ex.type == 'b')){
			if (op == "*") oper = "mul";
			else if (op == "/") oper = "div";
			else if (op == "mod") oper = "div";
			else if (op == "div") oper = "div";
			else if (op == "and") oper = "and";
			CodeGen("lw,$t0," + lhs_location.str() + "($sp)");
			CodeGen("lw,$t1," + ex_location.str() + "($sp)");
			CodeGen(oper + ",$t0 $t0 $t1");
			if (op == "mod")
				CodeGen("mfhi $t0");
			CodeGen("sw,$t0," + CurrOff.str() + "($sp)");
			ex.type = lhs.type;
			ex.location = CurrentOffset;
			CurrentOffset -= 4;
		}
		else if ((lhs.type == 'f' || lhs.type == 'i') && (ex.type == 'f' || ex.type == 'i')){
			if (op == "*") oper = "mul.s";
			else if (op == "/") oper = "div.s";
			else if (op == "div") oper = "div.s";
			if (lhs.type == 'f' && ex.type == 'f'){
				CodeGen("l.s $f1," + lhs_location.str() + "($sp)");
				CodeGen("l.s $f2," + ex_location.str() + "($sp)");
			}
			else if (lhs.type == 'f' && ex.type == 'i'){
				CodeGen("l.s $f1," + lhs_location.str() + "($sp)");
				CodeGen("lwc1 $f0 " + ex_location.str() + "($sp)");
				CodeGen("cvt.s.w $f2 $f0");
			}
			else if (lhs.type == 'i' && ex.type == 'f'){
				CodeGen("l.s $f1," + ex_location.str() + "($sp)");
				CodeGen("lwc1 $f0 " + lhs_location.str() + "($sp)");
				CodeGen("cvt.s.w $f2 $f0");
			}
			CodeGen(oper + " $f1 $f2 $f1");
			CodeGen("s.s $f1 " + CurrOff.str() + "($sp)");
			ex.type = 'f';
			ex.location = CurrentOffset;
			CurrentOffset -= 4;
		}
		else Error("type mismatch in termprime");
		termprime(get<0>(Token), ex);
		break;
	default:
		printf("33 ");
		break;
	}
}

//34 relfactor   :  factor factorprime
void relfactor(int tokennumber, Exp_Rocord &ex){
	error.setErrRout("relfactor");
	switch (tokennumber)
	{
	case 23:
	case 1:
	case 2:
	case 5:
		printf("34 ");
		factor(get<0>(Token), ex);
		factorprime(get<0>(Token), ex);
		break;
	default:
		error.show();
		break;
	}
}

//35|36 factorprime   :  RELOPTOK  factor |  <empty>
void factorprime(int tokennumber, Exp_Rocord &ex){
	error.setErrRout("factorprime");
	Exp_Rocord lhs;
	string oper;
	string op;
	stringstream lhs_location;
	stringstream ex_location;
	stringstream CurrOff;
	switch (tokennumber)
	{
	case 12:
		printf("35 ");
		lhs = ex;
		op = get<2>(Token);
		Match(12);
		factor(get<0>(Token), ex);
		lhs_location << lhs.location;
		ex_location << ex.location;
		CurrOff << CurrentOffset;
		label = GenStringLabel();
		if (lhs.type == 'i' && ex.type == 'i'){
			if (op == ">") oper = "bgt";
			else if (op == "<") oper = "blt";
			else if (op == "=") oper = "beq";
			CodeGen("lw,$t0," + lhs_location.str() + "($sp)");
			CodeGen("lw,$t1," + ex_location.str() + "($sp)");
			CodeGen(oper + " $t0 $t1 " + label);
			CodeGen("li,$t0,0");
			CodeGen("sw,$t0," + CurrOff.str() + "($fp)");
			CodeGen(label + ": ");
			/*CodeGen("li,$t0,1");
			CodeGen("sw,$t0," + CurrOff.str() + "($fp)");*/
			ex.type = 'i';
			ex.location = CurrentOffset;
			CurrentOffset -= 4;
		}
		if (lhs.type == 'f' && ex.type == 'f'){
			if (op == ">") oper = "c.gt.s";
			else if (op == "<") oper = "c.lt.s";
			else if (op == "=") oper = "c.eq.s";
			CodeGen("l.s $f1," + lhs_location.str() + "($sp)");
			CodeGen("l.s $f2," + ex_location.str() + "($sp)");
			CodeGen(oper + " $f1 $f2 " + label);
			CodeGen("li $t0 0");
			CodeGen("sw $t0 " + CurrOff.str() + "($fp)");
			CodeGen(label + ": ");
			CodeGen("li $t0 1");
			CodeGen("sw $t0 " + CurrOff.str() + "($fp)");
			ex.type = 'f';
			ex.location = CurrentOffset;
			CurrentOffset -= 4;
		}
		if (lhs.type == 'b' && ex.type == 'b'){
			CodeGen("lw,$t0," + lhs_location.str() + "($sp)");
			CodeGen("lw,$t1," + ex_location.str() + "($sp)");
			CodeGen("beq $t0 $t0 $t1");
			CodeGen("sw,$t0," + CurrOff.str() + "($sp)");
			ex.type = 'b';
			ex.location = CurrentOffset;
			CurrentOffset -= 4;
		}
		break;
	default:
		printf("36 ");
		break;
	}
}

//37|38|39|40factor :  NOTTOK   factor|  idnonterm|  LITTOK
//| '('  express  ')'
void factor(int tokennumber, Exp_Rocord &ex){
	stringstream t_lexeme;
	stringstream ex_loc;
	error.setErrRout("factor");
	switch (tokennumber)
	{
    //generate code to negate the exp in factor
	case 23:
		printf("37 ");
		Match(23);
		factor(get<0>(Token), ex);
		if (ex.type == 'b'){
			CodeGen("not $t0 $t0");
		}
		break;
	//make an e:ExpRecType record from the ST_Ptr
	case 1:
		printf("38 ");
		idnonterm(get<0>(Token), St_ptr);
		ex.location = St_ptr.location;
		ex.type = St_ptr.type;
		break;
	//generate code to load the literal into a register
	//generate code to store the literal in some offset
	//make an ExpRec that contains the type and this offset
	case 2:
		printf("39 ");
		ex.location = CurrentOffset;
		CurrentOffset -= 4;
		ex.type = getType(get<2>(Token));
		t_lexeme << get<2>(Token);
		ex_loc << ex.location;
		//if the Token is ture, set the lexeme to 1 
		if (get<2>(Token) == "true"){
			t_lexeme.str("");
			t_lexeme << 1;
		}
		//if the Token is false, set the lexeme to 0 
		if (get<2>(Token) == "false"){
			t_lexeme.str("");
			t_lexeme << 0;
		}
		//CodeGen of the appropriate li into some register
		//CodeGen sw register into ex.loc
		if (ex.type == 'i' || ex.type == 'b'){
			CodeGen("li,$t0," + t_lexeme.str());
			CodeGen("sw,$t0," + ex_loc.str() + "($fp)");
		}
		if (ex.type == 'f'){
			label = GenStringLabel();
			FloatData.push_back(label + ": .float " + t_lexeme.str());
			CodeGen("l.s $f1 " + label);
			CodeGen("s.s $f1 " + ex_loc.str() + "($fp)");
		}
		Match(2);
		break;
	case 5:
		printf("40 ");
		Match(5);
		express(get<0>(Token), ex);
		Match(6);
		break;
	default:
		error.show();
		break;
	}
}

//41 idnonterm    :  IDTOK
//look up tok.lexeme in all active scopes (will have to search down the stack)
//if not found, NonFatalError
//if found, return either(a) ptr, (b)type, loc.var / cons or(c) copy of the
//symbol table record
void idnonterm(int tokennumber, ST_Rocord &ST_Ptr){
	error.setErrRout("idnonterm");
	switch (tokennumber)
	{
	case 1:
		printf("41 ");
		if (ST_Roc.find_in_all(get<2>(Token))){    //if compiler finds the token in the symbol table 
			ST_Ptr = *ST_Roc.find_in_all(get<2>(Token));  //compiler will assign the information to the struct:ST_Rocord.
		}
		else{
			Error("This variable has not been declared!");
			system("pause");
			exit(0);
		}
		Match(1);
		break;
	default:
		error.show();
		break;
	}
}