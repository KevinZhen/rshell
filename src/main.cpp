#include <set>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <vector>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost;
void BashPrompt() {
	char HostName[BUFSIZ];
	char* x = getlogin();
	if(NULL == x) {
		perror( "Error with getlogin()" );
		cout << "$ " << flush;
		return;
	}
	char* user = cuserid(x);
	int y = gethostname(HostName,sizeof(HostName));
	if(y == -1) {
		perror("Error with gethostname");
		cout << "$ " << flush;
		return;
	}
	cout << user << '@' << HostName << "$ " << flush;
}
struct command {
	//int argcount;
	//char **argvv;
	//vector<vector<char *> > arglist;
	vector<char *> args;
	bool IS_SUCCESS;
	bool EXEC_NEXT;
	string connector;
};
vector<command> ParseTokens(vector<string> ParsedCmd) {
	string C_AND = "&&";
	string C_OR = "||";
	string C_COM = "#";
	string C_SEMI = ";";
	vector<command> ret;
	int count = 0;
	string tmp[] = { "&&", "||",  ";", "#"};
	set<string> connectors(tmp, tmp + sizeof(tmp) / sizeof(tmp[0]));
	vector<char *> temp;
	for(unsigned i = 0; i < ParsedCmd.size(); ++i) {
		if(connectors.find(ParsedCmd.at(i)) != connectors.end() ) {
			if(temp.empty()) {
				//first token is a connector, return an error
				ret.clear();
				ret.push_back(command());
				ret.at(0).connector = ParsedCmd.at(i);
				return ret; 
			}
			temp.push_back(0);
			ret.push_back(command());
			ret.at(count).args=temp;
			ret.at(count).IS_SUCCESS = false;
			ret.at(count).EXEC_NEXT = false;
			if(ParsedCmd.at(i) == C_AND) {
				ret.at(count).connector = C_AND;
			}
			else if(ParsedCmd.at(i) == C_OR) {
				ret.at(count).connector = C_OR;
			}
			else if(ParsedCmd.at(i) == C_COM) {
				ret.at(count).connector = C_COM;
			}
			else if(ParsedCmd.at(i) == C_SEMI) {
				ret.at(count).connector = C_SEMI;
			}
			temp.clear();
			++count;
		} 
		else {
			temp.push_back(const_cast<char *>(ParsedCmd.at(i).c_str()));
		}
	}
	if(!temp.empty()) {
		temp.push_back(0);
		ret.push_back(command());
		ret.at(count).args=temp;
		ret.at(count).IS_SUCCESS = false;
		ret.at(count).EXEC_NEXT = false;
		ret.at(count).connector = "";
	}
	return ret;	
}
//trim white space from the commandLine
//tokenize the commandline 
	//push tokens onto a vector until a connector is found or end
	//create command struct with vector of tokens
	//push command struct onto a vector of command structs
//Loop through the vector of command structs
	//strcmp exit then exit after all previous commands are executed
	//execute command
	//set command bools 
	//Depending on command.connector
		//"" No more commands end loop
		//&& execute next command if success and skip next command if failure
		//|| execute next command if failure and skip if success
		//; Execute next command
		//# end loop all commands are treated as comments
int main( int argc, char *argv[] ) {
	while(1) {
		vector<command> cmdlist;
		vector<string> ParsedCommands;
		string CommandLine;
		pid_t c_pid, pid;
		int status;
		BashPrompt();
		getline(cin, CommandLine);
		trim_if(CommandLine, is_any_of("\t "));
		split(ParsedCommands, CommandLine, is_any_of("\t "), token_compress_on);
		cmdlist = ParseTokens(ParsedCommands);
		if(cmdlist.at(0).args.empty()) {
			cout << "Syntax Error near unexpected token " 
					<< cmdlist.at(0).connector << endl;
		}
		else {
			cout << cmdlist.at(0).args[0] << endl;
		}
		for(unsigned i = 0; i < cmdlist.size(); ++i) {
			if(i==0 || cmdlist.at(i-1).EXEC_NEXT) {
				if(iequals(cmdlist.at(i).args[0], "exit")) {
					exit(1);
				}
				c_pid = fork();
		
				if(c_pid < 0) {
					perror("Fork Failed");
					exit(1);						
				}
				else if(c_pid == 0) {
					execvp(cmdlist.at(i).args[0], cmdlist.at(i).args.data());
					perror("Execvp Failed");
				}
				else if(c_pid > 0) {
					if( (pid=wait(&status)) < 0) {
						perror("Wait error");
						exit(1);
					}
				}
				if(status == 0) {
					cmdlist.at(i).IS_SUCCESS = true;
				}
			}	
			if(iequals(cmdlist.at(i).connector, "||")) {
				if(!cmdlist.at(i).IS_SUCCESS) {
					cmdlist.at(i).EXEC_NEXT = true;
				}
				else {
					cmdlist.at(i).EXEC_NEXT = false;
					unsigned j = i+1;
					while(j < cmdlist.size() && 
						iequals(cmdlist.at(j).connector, "||")) {
						cmdlist.at(j).EXEC_NEXT = false;
						++j;
					}
					i = j;
					cmdlist.at(i).EXEC_NEXT = true;
				}
			}
			else if(iequals(cmdlist.at(i).connector, "&&")) {
				if(cmdlist.at(i).IS_SUCCESS) {
					cmdlist.at(i).EXEC_NEXT = true;
				}
			}
			else if(iequals(cmdlist.at(i).connector, ";")) {
				cmdlist.at(i).EXEC_NEXT = true;
			}

		}		
	}
	return 0;
}

