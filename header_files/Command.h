#pragma once

#include <iostream>
#include <string>
#include <vector>
using namespace std;	

class Command
{

	public:

		static bool findExe(const string &command, string &fullPath);

	private:
		string command;
		vector<string> args;




};