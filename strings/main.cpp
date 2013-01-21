#include <iostream>
#include <string>

using namespace std;

int main(int argc, char **argv)
{
	string myString = "YAY ";
	stringstream conv;
	int age = 10;
	string output;
	conv << age;
	conv >> output;
	myString += output;
	cout << "STRING: " << myString.c_str() << endl;
	return 0;
}
