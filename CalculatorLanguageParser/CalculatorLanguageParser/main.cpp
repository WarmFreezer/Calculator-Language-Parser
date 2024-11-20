#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "Parse.cpp"

using namespace std;
int main()
{
	Parse parse("input.txt", "output.txt");

	parse.start();

	return 0;
}



