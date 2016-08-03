#include"ParserCodeGen.h"
#include <Windows.h>   //for calling the system instruction

void clearcode(){
	ofstream codefile("codefile.txt");  //create a new file to cover the previous version
}

void main(){
	int result;
	cout << "1.open a text file for writing code" << endl;
	cout << "2.compile the code" << endl;
	cout << "3.show the mips code" << endl;
	cout << "4.run code in Mars" << endl;
	cout << "5.close the compiler" << endl;
	int i = -1;
	cin >> i;
	while (i != 5){
		switch (i)
		{
		case 1:
			result = system("LPAS.txt");
			cin >> i;
			break;
		case 2:
			clearcode();
			readfile();
			scanner();
			findtoken.push(0, "$", "EOF");
			Token = FindToken();
			Program(get<0>(Token));
			if (get<1>(Token) == "$"){
				printf("successful parse!");
				ST_Roc.display();
			}
			else{
				clearcode();
				Error("expected eof");
			}
			cin >> i;
			break;
		case 3:
			result = system("codefile.txt");
			cin >> i;
			break;
		case 4:
			cout << "you should copy the code in the codefile.txt to Mars4_5" << endl;
			cout << "there is a Mars4.5 in the file, you can open it and copy the mips code(from codefile.txt) to it." << endl;
			cin >> i;
			break;
		default:
			cout << "wrong input, please input an integer between 1 and 4" << endl;
			cin >> i;
			break;
		}
	}
}