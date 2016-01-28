#include "Parameter.h"
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;
class STtable
{
public:
	STtable();
	~STtable();
	void readST(string path);
	void dumpST(string path);
	int** source_target_array(){ return _source_target_array; }
private:
	int** _source_target_array;
};
STtable::STtable()
{
	_source_target_array = new int*[MAX_LEX];
	for (int i = 0; i < MAX_LEX; i++)
	{
		_source_target_array[i] = new int[MAX_LEX];
		for (int j = 0; j < MAX_LEX; j++) {
			_source_target_array[i][j] = -1;
		}
	}
}

STtable::~STtable()
{
	delete _source_target_array;
}
void STtable::readST(string path)
{
	ifstream in(path+"/table.KB", std::ios::in);
	char str[512];
	for (int i = 0; i < MAX_LEX; i++) {
		for (int j = 0; j < MAX_LEX; j++) {
			in.getline(str, 512);
			int s, t, v;
			sscanf(str, "source lex %d target lex %d kb %d\n", &s, &t, &v);
			_source_target_array[s][t] = v;
		}
	}
}
void STtable::dumpST(string path)
{
	ofstream out(path+"/table.KB", std::ios::out);
	for (int i = 0; i < MAX_LEX; i++) {
		for (int j = 0; j < MAX_LEX; j++) {
			//if (source_target_array[i][j]==-1)
			//continue;
			out << "source lex " << i << " target lex " << j << " kb " << _source_target_array[i][j] << "\n";
		}
	}
	out.close();
}