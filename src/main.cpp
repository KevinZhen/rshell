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

queue<string> ParseConnectors( string cmdline ) {
	queue<string> connectors;
	for( unsigned i=0; i < cmdline.size(); ++i) {
		if(cmdline.at(i) == ';') {
			connectors.push_back(";");
		}
		else if(cmdline.at(i) == '#') {
			connectors.push_back("#");
		}
		else if(cmdline.at(i) == '|') {
			if( (i+1 < cmdline.size()) && cmdline.at(i+1) == '|' ) {
				connectors.push_back("||");
			}
		}
		else if((cmdline.at(i) == '&') && (i+1 < cmdline.size()) && cmdline.at(i+1) == '&') {
			connectors.push_back("&&");
		}
	}
	return connectors;	
};

vector<string> TokenizeCmds(string cmdline) {
	vector<string> ret;
	

int main( int argc, char *argv[] ) {
	while(1) {
		queue<string> ConnectorList;
		vector<char **> cmdlist;
		vector<strings> ParsedCommands
		string CommandLine;
		pid_t child_pid, pid;
		int status;
		
		BashPrompt();
		getline(cin, CommandLine);
		//get a vector of all connectors
		//tokenize CommandLine delimeter || && ; #
		//
	}
}

