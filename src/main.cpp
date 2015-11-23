#include <set>
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
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
	vector<char *> args;
	bool IS_SUCCESS;
	bool EXEC_NEXT;
	string connector;
	string flag;
	string path;
};
struct cmdgroup {
	vector<command> cmd;
	bool EXEC_NEXT;
	bool IS_SUCCESS;
	string connector;
};
struct cmd_ptr {
	command *cmd_ptr;
	cmdgroup *grp_ptr;
};
vector<command> ParseTokens(vector<string> ParsedCmd) {
	string C_AND = "&&";
	string C_OR = "||";
	string C_COM = "#";
	string C_SEMI = ";";
	bool IS_BRACE = false;
	string FlagCmp = "efdEFD";
	string Flags, BadFlags = "";
	string path = "";
	vector<command> ret;
	int count = 0;
	string tmp[] = { "&&", "||",  ";"};
	set<string> connectors(tmp, tmp + sizeof(tmp) / sizeof(tmp[0]));
	vector<char *> temp;
	for(unsigned i = 0; i < ParsedCmd.size(); ++i) {
		if(ParsedCmd.at(i) == C_COM) {
			if(!temp.empty()) {
				ret.push_back(command());
				temp.push_back(0);
				ret.at(count).args=temp;
				ret.at(count).IS_SUCCESS = false;
				ret.at(count).EXEC_NEXT = false;
				ret.at(count).connector = C_COM;
				if(Flags != "") {
					ret.at(count).flag = Flags;
					Flags = "";
				}
				else if(BadFlags != "") {
					ret.at(count).flag = BadFlags;
					BadFlags = "";
				}		
			}
			return ret;
		}
		else if(connectors.find(ParsedCmd.at(i)) != connectors.end() && !IS_BRACE) {
			if(temp.empty()) {
				//first token is a connector, return an error
				ret.clear();
				ret.push_back(command());
				ret.at(0).connector = ParsedCmd.at(i);
				return ret; 
			}
			temp.push_back(0);
			ret.push_back(command());
			ret.at(count).path = path;
			path = "";
			ret.at(count).args = temp;
			ret.at(count).IS_SUCCESS = false;
			ret.at(count).EXEC_NEXT = false;
			if(Flags != "") {
					ret.at(count).flag = Flags;
					Flags = "";
			}
			else if(BadFlags != "") {
					ret.at(count).flag = BadFlags;
					BadFlags = "";
			}
			if(ParsedCmd.at(i) == C_AND) {
				ret.at(count).connector = C_AND;
			}
			else if(ParsedCmd.at(i) == C_OR) {
				ret.at(count).connector = C_OR;
			}
			/*
			else if(ParsedCmd.at(i) == C_COM) {
				ret.at(count).connector = C_COM;
			}
			*/
			else if(ParsedCmd.at(i) == C_SEMI) {
				ret.at(count).connector = C_SEMI;
			}
			temp.clear();
			++count;
		}
		//another condition, temp.at(0) == "test"
		else if((ParsedCmd.at(i).at(0) == '-') && Flags == "" 
		&& BadFlags == "" && (iequals(temp.at(0), "test") 
		|| iequals(temp.at(0), "[")) ) {
			if(ParsedCmd.at(i).size() == 2 && 
			FlagCmp.find(ParsedCmd.at(i).at(1)) != string::npos) {
				Flags = ParsedCmd.at(i).at(1);
			}
			else {
				BadFlags = ParsedCmd.at(i);
			}
		}
		else {
			if(ParsedCmd.at(i) == "]" && IS_BRACE) {
				IS_BRACE = false;
			}
			else {
				if(ParsedCmd.at(i) == "[") {
					IS_BRACE = true;
				}
				if(!temp.empty() && (iequals(temp.at(0), "[") || (iequals(temp.at(0), "test")))) {
					path += ParsedCmd.at(i);
				}
				temp.push_back(const_cast<char *>(ParsedCmd.at(i).c_str()));
			}
		}
	}
	if(IS_BRACE) {
		cout << "error missing ] " << endl;
	}
	else if(!temp.empty()) {
		temp.push_back(0);
		ret.push_back(command());
		ret.at(count).path = path;
		ret.at(count).args=temp;
		ret.at(count).IS_SUCCESS = false;
		ret.at(count).EXEC_NEXT = false;
		ret.at(count).connector = "";
		if(Flags != "") {
			ret.at(count).flag = Flags;
			Flags = "";
		}
		else if(BadFlags != "") {
			ret.at(count).flag = BadFlags;
			BadFlags = "";
		}
	}
	return ret;	
}
bool Exec_Test(command cmd) {
	struct stat Stat_;
	bool EXISTS = true;
	string path = cmd.path;
	if(stat(const_cast<char *>(path.c_str()), &Stat_) !=  0) {
		perror("Error with stat()");
		EXISTS = false;
	}
	if(cmd.flag == "f") {
		return EXISTS && S_ISREG(Stat_.st_mode);
	}
	else if(cmd.flag == "d") {
		return EXISTS && S_ISDIR(Stat_.st_mode);
	}
	else if(cmd.flag == "" || cmd.flag == "e") {
		return EXISTS;	
	}
	else {
		cerr << "Error: Incorrect or unsupported flags" << endl;	
		return false;
	}
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
int execute(vector<command> cmdlist) {
	pid_t c_pid, pid;
	int status;
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
					if(iequals(cmdlist.at(i).args[0], "test") ||
						iequals(cmdlist.at(i).args[0], "[")) {
						bool T_SUC = Exec_Test(cmdlist.at(i));
						if(!T_SUC) {
							cmdlist.at(i).IS_SUCCESS = false;
						}
						else if(T_SUC) {
							cmdlist.at(i).IS_SUCCESS = true;
						}
					}
					else {
						/*
  						for(unsigned j = 0; j< cmdlist.size(); ++j) {
							for(unsigned k = 0; k < cmdlist.at(j).args.size()-1; ++k) {
								cout << j << ".) " << cmdlist.at(j).args[k];
							}
						cout << endl;
						}	
						*/			
						execvp(cmdlist.at(i).args[0], 
							cmdlist.at(i).args.data());
						perror("Execvp Failed");
						exit(1);
					}
				}
				else if(c_pid > 0) {
					if( (pid=wait(&status)) < 0) {
						perror("Wait error");
						exit(1);
					}
				}
				/*
				if(iequals(cmdlist.at(i).args[0], "test") ||
						iequals(cmdlist.at(i).args[0], "[")) {
					cout << cmdlist.at(i).connector << endl;
				}
				*/
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
	return 0;
}

int main( int argc, char *argv[] ) {
	while(1) {
		vector<command> cmdlist;
		vector<string> ParsedCommands;
		vector<string> Commands;
		string CommandLine;
		string line = "";
		int parens = 0;
		int braces = 0;
		bool COMM = false;
		//pid_t c_pid, pid;
		//int status;
		BashPrompt();
		getline(cin, CommandLine);
		trim_if(CommandLine, is_any_of("\t "));
		split(Commands, CommandLine, is_any_of("\t "), token_compress_on);
		for(unsigned i = 0; i < Commands.size(); ++i) {
			if(Commands.at(i) != "#" && !COMM) {
				ParsedCommands.push_back(Commands.at(i));
			}
			else {
				COMM = true;
			}
			if(Commands.at(i) == "(") {
				++parens;
			}
			if(Commands.at(i) == "[") {
				++braces;
			}
			if(Commands.at(i) == ")") {
				--parens;
			}	
			if(Commands.at(i) == "]") {
				--braces;
			}
		}
		if(parens == 0 && braces == 0) {			
			cmdlist = ParseTokens(ParsedCommands);
			if(cmdlist.empty()) {
				//cout << "Commented\n";
			}
			else if(cmdlist.at(0).args.empty() || (iequals(cmdlist.back().connector,"||") || 
				iequals(cmdlist.back().connector,"&&") )) {
				cout << "Syntax Error near unexpected token " 
						<< cmdlist.at(0).connector << endl;
				cmdlist.clear();
			}
			/*
			for(unsigned j = 0; j< cmdlist.size(); ++j) {
				for(unsigned k = 0; k < cmdlist.at(j).args.size()-1; ++k) {
					cout << j << ".) " << cmdlist.at(j).args[k];
				}
				cout << endl;
			}
			*/
			execute(cmdlist);
		}	
		else {
			cerr << "Syntax Error. Missing closing Parentheses or braces " << endl;	
		}	
	}
	return 0;
}

