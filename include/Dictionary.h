#include <unordered_map>
#include <map>
#include <string>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <string>
using namespace std;

class Dictionary
{
public:
	Dictionary();
	~Dictionary();
	int Size(){ return size; }
	int hash_word(string s);
	void hash_word(string s, int loc);
	int get_hash(string s);
	void dumpDic(string path);
	void readDic(string path);
	void merge(Dictionary* d2);
	std::unordered_map<int, int>& old_new_transition(){ return _old_new_transition; }
	unordered_map<string, int>& Ldictionary() { return _Ldictionary; }
	map<int, string>& Rdictionary(){ return _Rdictionary; }
private:
	unordered_map<string, int> _Ldictionary;
	map<int, string> _Rdictionary;
	int size;
	std::unordered_map<int, int> _old_new_transition;
};

Dictionary::Dictionary()
{
	size = 0;
}

Dictionary::~Dictionary()
{
}
void Dictionary::merge(Dictionary* d2)
{
	for (unordered_map<string, int>::iterator i = d2->Ldictionary().begin(); i != d2->Ldictionary().end(); i++)
	{
		hash_word(i->first, i->second);
	}
}
void Dictionary::hash_word(string s, int loc)
{
	std::unordered_map<std::string, int >::iterator it = _Ldictionary.find(s);
	if (it != _Ldictionary.end())
	{
		_old_new_transition[loc] = it->second;
	}
	else
	{
		_Ldictionary[s] = size;
		_Rdictionary[size] = s;
		int new_loc = size;
		size++;
		_old_new_transition[loc] = new_loc;
	}
}
int Dictionary::hash_word(string s)
{
	std::unordered_map<std::string, int >::iterator it = _Ldictionary.find(s);
	if (it != _Ldictionary.end())
	{
		return it->second;
	}
	else
	{
		_Ldictionary[s] = size;
		_Rdictionary[size] = s;
		int loc = size;
		size++;
		return loc;
	}
}
int Dictionary::get_hash(string s)
{
	std::unordered_map<std::string, int >::iterator it = _Ldictionary.find(s);
	if (it != _Ldictionary.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}
void Dictionary::readDic(string path)
{
	ifstream in(path+"/dictionary.KB", std::ios::in);
	char str[128];
	int dic_size;
	in.getline(str, 128);
	sscanf(str, "dictionary size %d\n", &dic_size);
	for (int i = 0; i < dic_size; i++)
	{
		char ss1[64], ss2[64];
		strcpy(ss1, "\0");
		strcpy(ss2, "\0");
		in.getline(str, 128);

		int j;
		sscanf(str, "%d %s %s", &j, ss1, ss2);
		std::string s1(ss1), s2(ss2);
		if (strlen(ss2) != 0)
		{
			s1 += " " + s2;
		}
		size++;
		_Ldictionary[s1] = j;
		_Rdictionary[j] = s1;
	}
	in.close();
}
void Dictionary::dumpDic(string path)
{
	ofstream out(path+"/dictionary.KB", std::ios::out);
	cout << "dump dictionary\n";
	out << "dictionary size " << _Rdictionary.size() << "\n";
	for (map<int, string>::iterator i = _Rdictionary.begin(); i != _Rdictionary.end(); i++)
	{
		out << i->first << " " << i->second << "\n";
	}
	out.close();
}