#include <fstream>

using namespace std;

void writeTemplate(char* cfgtemplate, const wchar_t* filename)
{
	ofstream out;
	out.open(filename);
	out << cfgtemplate;
	out.close();

	return;
}
