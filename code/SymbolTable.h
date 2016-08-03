#include <iostream>
#include <fstream>     //read the file 
#include <vector>      //for using library of vector.

using namespace std;

struct ST_Rocord    //all of the related information of a symbol
{
	string token;    //the lexeme of the token;
	int scope;       //scope number of the token;
	char type;       //type of the token;
	int location;    //location in the mips 
	string declartype;  //type of declaration. var or const.
};

//a vector that store the ST_Rocord struct, every element in the 
//vector V represent a set of information of a token;
vector<ST_Rocord> V;   

class sym{
public:
	vector<int>scopenum;     //once open a block, I will insert a scope number in this vector
	int BlockNum = 0;        //original scope number.
	sym(){}

	//insert symbols into a 2D vector. row: scope column:symbols.
	void insert(string token, int currentblocknum, int local, string dectyp){
		ST_Rocord newtoken;     //create a ST_Rocord struct
		newtoken.scope = currentblocknum;   
		newtoken.token = token;
		newtoken.location = local;
		newtoken.declartype = dectyp;
		V.push_back(newtoken);   //insert this set of token information in the vector V.
	}

	//display the charactors and strings stored in the 2D vector.
	void display()
	{
		cout << endl;
		for (int i = 0; i < BlockNum + 1; i++)
		{
			cout << "scope " << i << ":";
			for (vector<ST_Rocord>::const_iterator p = V.begin(); p != V.end(); p++){
				if (p->scope == i)
					cout << p->token<<" ";
			}
			cout << endl;
		}
	}

	//create an iterator to traverse the vector V, if you found the symbol in the current scope, the function
	//will return the symbol's information.
	ST_Rocord *find_in_Current(string token, int currentblocknum){
		for (vector<ST_Rocord>::iterator p = V.begin(); p != V.end(); p++){
			if (p->scope == currentblocknum&&p->token == token)
				return &*p;
		}
		return nullptr;
	}

	//create an iterator to traverse the vector V, if you found the symbol in all scopes, the function
	//will return the symbol's information.
	ST_Rocord *find_in_all(string token){
		for (vector<int>::iterator p = scopenum.begin(); p != scopenum.end(); p++){
			for (vector<ST_Rocord>::iterator q = V.begin(); q != V.end(); q++){
				if (q->scope == *p && q->token == token)
					return &*q;
			}
		}
		return nullptr;
	}
};



