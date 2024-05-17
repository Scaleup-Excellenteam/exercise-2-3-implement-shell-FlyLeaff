#pragma once

#include <iostream>
#include <string>
#include <vector>
using namespace std;	

class Command
{

	public:
		Command(string command);
		~Command();
		void parse(string& input);

		void run();

	private:
		string command;
		vector<string> args;




};