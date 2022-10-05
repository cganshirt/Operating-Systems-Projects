#include <tsh.h>
#include <iostream>
#include <stdio.h>
#include <bits/stdc++.h>
#include <string>

using namespace std;

void simple_shell::parse_command(char* cmd, char** cmdTokens) {
    char *string = strtok(cmd, "\n");
    char *token = strtok(string, " ");
    int counter = 0;
    while (token != NULL && counter < sizeof(cmdTokens)) {
        cmdTokens[counter] = token;
        token = strtok(NULL, " ");
        counter += 1;
    }
    cmdTokens[counter] = NULL;
}

void simple_shell::exec_command(char** argv) {
    int pid = fork();
    if(pid == 0) {
	    execvp(argv[0], argv);
        exit(0);
    } else {
        wait(NULL);
    }

}

bool simple_shell::isQuit(char* cmd) {
    istringstream ss(cmd);
    string word;
    ss >> word;
    if(word.compare("quit") == 0) {
        return true;
    }    
    return false;
}

