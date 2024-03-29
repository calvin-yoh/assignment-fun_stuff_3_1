#include "../header/Reader.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

void Reader::checkPrio(const char check, int& returnValue)
{
	if(check == '"')
	{
		returnValue++;
	}else if(check == '#')
	{
		returnValue += 4;
	}else if(check == ';')
	{
		returnValue += 2;
	}else if(check == ' ')
	{	
		returnValue += 3;
	}
	
}


void Reader::splitString(const string vect, vector<string>& output)
{
	string word = "";
	int prioCheck = 0;
	for(int i = 0; i < vect.size(); i++)
	{	
		prioCheck = 0;
		checkPrio(vect.at(i), prioCheck);
		switch(prioCheck)
		{
			case 1: 
				i++;
                        	while(vect.at(i) != '"')
                        	{
                                	word = word + vect.at(i);
                                	i++;
                        	}
                        	output.push_back(word);
				word = "";
				break;
		

			case 2:
				if(word == " ")
				{
					word = vect.at(i);
					output.push_back(word);
					word = "";
				}
				else
				{
					output.push_back(word);
					word = vect.at(i);
					output.push_back(word);
					word = "";
				}	
				break;
			case 3:
				if(word != "")
				{ 
					output.push_back(word);
				}		
				word = "";
				break;
			case 4: 
				if(i != vect.size() -1)
				{
					if(vect.at(i+1) == ' ')
					{
						i+= 2;	
						word = '#';				
					}
				}
				while(i != vect.size() && vect.at(i) != ' ')
				{
					word = word + vect.at(i);
					i++;
				}
				output.push_back(word);
				word = "";
				break;
			default:
				word = word + vect.at(i);
				if(i == vect.size() - 1)
				{
					output.push_back(word);
					word = "";	
				} 					
		};	
	}
	
}

void Reader::execute(char* input[], int& status)
{	
		pid_t pid = fork();
		if(pid == -1)
		{
			perror("fork");
		}
		else if(pid == 0)
		{
			status = 0;
			this->working = false;
			if(execvp(input[0], input) == -1)
			{
				perror("exec");
			}	
		}else		
		if(pid > 0)
		{
			if(wait(0) == -1)
			{	
				perror("wait failed");
			}
		}
}

void Reader::testExecute(const char* path, string flag, int& status)
{
	struct stat buf;
	stat(path, &buf);
	if(flag == "-d")
	{
		if(S_ISDIR(buf.st_mode))
		{
			cout << "(True)" << endl;
			status = 1;	
		}
		else
		{
			cout << "(False)" << endl;
			status = 0;
		}
	}	
	else
	{
		if(S_ISREG(buf.st_mode))
		{
			cout << "(True)" << endl;
			status = 1;
		}
		 else
                {
                        cout << "(False)" << endl;
			status = 0;                
		}
	}
}

void Reader::removePar(vector<string>& input)
{
	for(int i = 0; i < input.size(); i++)
	{
		int x = 0;

		while(x < input.at(i).size())
		{
   			if(input.at(i).at(x) == '(' || input.at(i).at(x) == ')')
    			{
        			input.at(i).erase(x,1);
    			}
    			else
			{
				x++;
			}
		}
	}
}


void Reader::readInput(string input)
{
	int status = 1;
	vector<string> argv;
	splitString(input, argv);
	removePar(argv);
	int countArgs = 0;
	char* argsOne[2];
	char* argsTwo[3];
	argsOne[1] = NULL;
	argsTwo[2] = NULL;
	string flag;
	int countTest = 0;
	for(int i = 0; i < argv.size(); i++)
	{
		countArgs = 0;
		countTest = 0;
		if(argv.at(i) == "test" || argv.at(i) == "[")
		{
			if(argv.at(i) == "[")
			{
				countTest = 1;
			}
			i++;
			if(i < argv.size() && (argv.at(i) == "-e" || (argv.at(i) == "-d" || (argv.at(i) == "-f"))))
			{
				flag = argv.at(i);
				i++;
			}
			else
			{
				flag = "-e";
			}
			testExecute(argv.at(i).c_str(), flag, status);
			i++;
			i += countTest;		
			goto mylabel;
		} 
		while(countArgs + i != argv.size() && (argv.at(countArgs + i) != ";" && argv.at(countArgs + i) != "&&" && argv.at(countArgs + i) != "||" && argv.at(countArgs + i).at(0)  != '#'))
                        {
                                countArgs++;
                        }
		if(countArgs == 1)
                        {
                                argsOne[0] = (char*)argv.at(i).c_str();
                       		
                        }else if(countArgs == 2)
                        {
                                argsTwo[0] = (char*)argv.at(i).c_str();
                                argsTwo[1] = (char*)argv.at(i+1).c_str();
			}
		if(countArgs == 1)
                {
                        execute(argsOne, status);
			//cout << "called  countArgsOne" << endl;
                }else if(countArgs == 2)
                {
                        execute(argsTwo, status);
			//cout << "called countArgsTwo" << endl;
                }
		mylabel: 
		if(countArgs + i != argv.size() && argv.at(countArgs + i) == "||")
                {
                        if(status != 0)
                        {
                                while(countArgs + i + 1 != argv.size() && (argv.at(countArgs + i + 1) != ";" || argv.at(countArgs + i + 1) != "&&" || argv.at(countArgs + i + 1) != "||"))
                                {
                                        i++;
                                }
                        }
                }else if(countArgs + i != argv.size() && argv.at(countArgs + i) == "&&")
                {
                        if(status == 0)
                        {
				while(countArgs + i + 1 != argv.size() && (argv.at(countArgs + i + 1) != ";" || argv.at(countArgs + i + 1) != "&&" || argv.at(countArgs + i + 1) != "||"))
                                {
                                        i++;
                                }

                        }

                }
                i += countArgs;
	//	cout << status << end;
	}
}

bool Reader::doInput(string input)
{
	readInput(input);
	return this->working;
}



