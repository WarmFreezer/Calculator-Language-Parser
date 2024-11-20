#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;
class Parse
{

	/*
		CALCULATOR GRAMMAR

		program		->	stmt_list $$

		stmt_list	->	stmt stmt_list
					->	NULL

		stmt		->	id := expr
					->	read id
					->	write expr

		expr		->	term term_tail

		term_tail	->	add_op term term_tail
					->	NULL

		term		->	factor fact_tail

		fact_tail	->	mult_op fact fact_tail
					->	NULL

		factor		->	( expr )
					->	id
					->	number

		add_op		->	+
					->	-

		mult_op		->	*
					->	/
	*/

public:
	Parse(string input, string output) : outputFile(output), inputFile(input) {}
	//Set io files, constructor

	void start() //Start function: 
	{
		if (outputFile) //Confirm output file opens
		{
			cout << "Opened Output File\n";
			if (inputFile) //Confirms input file opens
			{
				cout << "Opened Input File\n";

				program(); //Runs program()

				outputFile << "\n--------------------------------\nErrors Generated: " << errorCount << "\n--------------------------------\n";
				//Prints number of errors at end of program execution

				inputFile.close(); //Close input file
				outputFile.close(); //Close output file
			}

			else //Terminates program since there is no input file
			{
				cout << "Could not open input file, terminated";
			}
		}
		
		else //Terminates program since there is no output file
		{
			cout << "Could not open output file, terminated";
		}
	}

	bool contains(string searchWord) //Function to be used when checking a token is equal to a search word
	{
		if (currentToken.find(searchWord) != string::npos) //Returns true if the token is found
		{
			return true;
		}

		else
		{
			return false;
		}
	}

	void match_(string token_case) //Match Function: ***Special***
								   //This special match function does not place an executing statement after the output
	{
		if (contains(token_case)) //Checks if current token is token case
		{
			if (token_case == "\tid" || token_case == "\tnumber") //If the token case is an id or number
			{
				string temp = "";			
				int i = 0;	
				while (currentToken[i] != '\t') //Creates a temporary variable to hold the user defined token, such as a number or id name
				{
					temp += currentToken[i];
					i++;
				}

				outputFile << "\n\tToken: " << temp << " consumed";	
			}

			else
			{
				outputFile << "\n\tToken: " << token_case << " consumed";
			}

			getline(inputFile, currentToken); //Gets next token in stream
		}

		else
		{
			outputFile << "\nExpected: " << token_case << ", currentToken: " << currentToken << "\n";
			errorCount++;
			return;
		}
	}

	void match(string token_case) //Match Function:
	{
		if (contains(token_case)) //Checks if current token is token case
		{
			if (token_case == "\tid" || token_case == "\tnumber") //If the token case is an id or number
			{
				string temp = "";
				int i = 0;
				while (currentToken[i] != '\t') //Creates a temporary variable to hold the user defined token, such as a number or id name
				{
					temp += currentToken[i];
					i++;
				}

				outputFile << "\n\tToken: " << temp << " consumed\nExecuting: ";
			}

			else
			{
				outputFile << "\n\tToken: " << token_case << " consumed\nExecuting: ";
			}

			getline(inputFile, currentToken); //Gets next token in stream
		}

		else
		{
			outputFile << "\nExpected: " << token_case << ", currentToken: " << currentToken << "\n";
			errorCount++;
			return;
		}
	}

	void program() //program -> stmt_list $$
	{
		outputFile << "Executing: program() ";
		getline(inputFile, currentToken); //Gets first token in stream
		if (contains("\tid") || contains("\tkeyword") || contains("$$")) //Checks for valid start tokens as defined by grammar
		{
			stmt_list();
			match_("$$");
		}

		else //Invalud start token used
		{
			outputFile << "\nInvalid start token: " << currentToken << "\n";	
			errorCount++;
			return;
		}
	}						

	void stmt_list() //stmt_list -> stmt stmt_list || NULL
	{
		outputFile << "stmt_list() ";
		if (contains("\tid") || contains("\tkeyword")) //Checks for valid statements
		{
			stmt();
			stmt_list();
		}

		else if (contains("$$")) //Checks for end of program symbol
		{
			//Skip function: Epsilon Production
		}

		else //Invalid token
		{
			outputFile << "\nExpected: id, keyword, or $$\n";
			errorCount++;
			return;
		}
	}

	void stmt() //stmt -> id := expr || read id || write expr
	{
		outputFile << "stmt() ";
		if (contains("\tid")) //Checks for id
		{
			match_("\tid");
			match(":=");
			expr();
		}

		else if (contains("read")) //Checks for read
		{
			match_("read");
			match("\tid");
		}

		else if (contains("write")) //Checks for write
		{
			match("write");
			expr();
		}

		else //Invalid token
		{
			outputFile << "Expected: id or keyword\n";
			errorCount++;
			return;
		}
	}

	void expr() //expr -> term term_tail
	{
		outputFile << "expr() ";
		if (contains("\tid") || contains("\tnumber") || contains("(")) //Checks for the start of an expression
		{
			term();
			term_tail();
		}

		else //Invalid token
		{
			outputFile << "Expected: id, number, (\n";
			errorCount++;
			return;
		}
	}

	void term_tail() //term_tail -> add_op term term_tail || NULL
	{
		outputFile << "term_tail() ";
		if (contains("+") || contains("-")) //Checks for an addition operators
		{
			add_op();
			term();
			term_tail();
		}

		else if (contains(")") || contains("\tid") || contains("\tkeyword") || contains("$$")) //Checks for end of expression
		{
			//Skip function: Epsilon Production
		}

		else //Invalid Token
		{
			outputFile << "\nExpected: ), id, keyword, +, or -\n";
			errorCount++;
			return;
		}
	}

	void term() //term -> factor fact_tailt
	{
		outputFile << "term() ";
		if (contains("\tid") || contains("\tnumber") || contains("(")) //Checks for id or number: start of factor
		{
			factor();
			fact_tail();
		}

		else //Invalid Token
		{
			outputFile << "\nExpected: id, number, or (number)";
			errorCount++;
			return;
		}
	}

	void fact_tail() //fact_tail -> mult_op fact fact_tail || NULL
	{
		outputFile << "fact_Tail() ";
		if (contains("*") || contains("/")) //Checks for multiplication operators
		{
			mult_op();
			factor();
			fact_tail();
		}

		else if (contains("+") || contains("-") || contains(")") || contains("\tid") || contains("\tkeyword") || contains("$$"))
			//Checks for end of factor
		{
			//Skip function: Epsilon Production	
		}

		else //Invalid token
		{
			outputFile << "\nExpected: *, /, +, -, ), id, keyword, or $$";
			errorCount++;
			return;
		}
	}

	void factor() //factor -> ( expr ) ||	id || number
	{
		outputFile << "factor()";
		if (contains("\tid")) //Checks for id
		{
			match("\tid");
		}

		else if (contains("\tnumber")) //Checks for number
		{
			match("\tnumber");
		}

		else if (contains("(")) //Checks for the start of a parenthesis block
		{
			match("(");
			expr();
			match(")");
		}

		else //Invalid token
		{
			outputFile << "\nExpected: id, number, or (";
			errorCount++;
			return;
		}
	}

	void add_op() //add_op -> + || -
	{
		outputFile << "add_op() ";
		if (contains("+")) //Checks for +
		{
			match("+");
		}

		else if (contains("-")) //Checks for -
		{
			match("-");
		}

		else //Invalid token
		{
			outputFile << "\nExpected: + or -";
			errorCount++;
			return;
		}
	}

	void mult_op() //mult_op -> * || /
	{
		outputFile << "mult_op() ";
		if (contains("*")) //Check for *
		{
			match("*");
		}

		else if (contains("/")) //Check for /
		{
			match("/");
		}

		else //Invalid Token
		{
			outputFile << "\nExpected: * or /";
			errorCount++;
			return;
		}
	}

private:
	fstream inputFile;
	fstream outputFile;
	string currentToken;
	int errorCount = 0;
};