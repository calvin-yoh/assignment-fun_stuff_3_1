#include "../header/Reader.hpp"

#include <string>
#include <iostream>

using namespace std;

int main()
{
	cout << "Running. Press --q to quit" << endl;
	string input;
	do
	{

		Reader* reader = new Reader;
	
		cout << "$";
		getline(cin, input);
		if(input == "--q" || input == "exit")
		{
			break;
		}
		reader->readInput(input);
	}while(input != "--q" && input != "exit");	

	return 0;
}

