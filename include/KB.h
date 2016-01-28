#include <unordered_map>
#include <vector>
#include <string>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include <cmath>
using namespace std;
#include "Parameter.h"
#define mix(a,b,c) \
{ \
	a = a - b;  a = a - c;  a = a ^ (c >> 13); \
	b = b - c;  b = b - a;  b = b ^ (a << 8);  \
	c = c - a;  c = c - b;  c = c ^ (b >> 13); \
	a = a - b;  a = a - c;  a = a ^ (c >> 12); \
	b = b - c;  b = b - a;  b = b ^ (a << 16); \
	c = c - a;  c = c - b;  c = c ^ (b >> 5);  \
	a = a - b;  a = a - c;  a = a ^ (c >> 3);  \
	b = b - c;  b = b - a;  b = b ^ (a << 10); \
	c = c - a;  c = c - b;  c = c ^ (b >> 15); \
}
	/////////////////////////////////////////////////////////////
	// converters to and from string

	template <typename T>
	std::string ToString(T t)
	{
		ostringstream temp;
		temp << t;
		return temp.str();
	}

	template <typename T>
	T FromString(const std::string& s)
	{
		T t;
		istringstream temp(s);
		temp >> t;
		return t;
	}									
struct Key
{
	int source;
	int target;
	Key(int s, int t)
	{
		source = s;
		target = t;
	}
	bool operator==(const Key &other) const
	{
		return (source == other.source && target == other.target);
	}
};
struct KeyHasher
{
	int operator()(const Key &k) const
	{
		int c = 1;
		int a = k.source;
		int b = k.target;
		mix(a, b, c)
		return c;
	}
};

class KL
{
public:
	KL(int id);
	~KL();
	void readKL(string path);
	void dumpKL(string path);
	void incrementKL(int source, int target, double value = 1.0);
	void mergeKL(KL* kl2, std::unordered_map<int, int> old_new_transition);
	void calculate(int dic_size);
	unordered_map<Key, double, KeyHasher>& KLINK(){ return _KL; }
private:
	unordered_map<Key, double, KeyHasher> _KL;
	int id;
};
KL::KL(int id_)
{
	id = id_;
}

KL::~KL()
{
}
void KL::calculate(int dic_size)
{
	double *column_value = new double[dic_size];
	for (int i = 0; i < dic_size; i++)
	{
		column_value[i] = 0;
	}
	for (unordered_map<Key, double, KeyHasher>::iterator it = _KL.begin(); it != _KL.end(); ++it)
	{
		column_value[it->first.target] += it->second;
	}
	for (unordered_map<Key, double, KeyHasher>::iterator it = _KL.begin(); it != _KL.end(); ++it)
	{
		_KL[Key(it->first.source, it->first.target)] = log((it->second) /( column_value[it->first.target]) / P0);
	}
	delete column_value;
}
void KL::mergeKL(KL* kl2, std::unordered_map<int, int> old_new_transition)
{
	for (unordered_map<Key, double, KeyHasher>::iterator it = kl2->KLINK().begin(); it != kl2->KLINK().end(); ++it)
	{
		incrementKL(old_new_transition[it->first.source], old_new_transition[it->first.target], it->second);
	}
}
void KL::incrementKL(int source, int target, double value)
{
	_KL[Key(source, target)]+=value;
	//cout << "source:" << source << " target:" << target << " value:" << _KL[Key(source, target)] << "\n";
}
void KL::readKL(string path)
{
	ifstream in(path+"/KL" + ToString<int>(id) + ".KB", std::ios::in);
	char buffer[128];
	int kb_count;
	in.getline(buffer, 128);
	sscanf(buffer, "KB %d\n", &kb_count);
	//std::cout << "KB " << kb_count << "\n";
	if (kb_count != id)
	{
		throw new exception();
	}
	while (!in.eof())
	{
		in.getline(buffer, 128);

		int source, target;
		double value;
		sscanf(buffer, "%d %d %lf\n", &source, &target, &value);
		_KL[Key(source, target)] = value;
	}
	in.close();
}
void KL::dumpKL(string path)
{
	//cout << "dumping KL ID =" << id << "\n";
	ofstream out(path+"/KL" + ToString<int>(id) + ".KB", std::ios::out);
	out << "KB " << id << "\n";
	for (unordered_map<Key, double, KeyHasher>::iterator it = _KL.begin(); it != _KL.end(); ++it)
	{
		out << it->first.source << " " << it->first.target << " " << it->second << "\n";
	}
}


class KB
{
public:
	KB();
	~KB();
	int size(){ return kb_size; }
	void initKB(int count);
	void readKB(string path);
	void dumpKB(string path);
	void mergeKB(KB* kb2, std::unordered_map<int, int> old_new_transition);
	void recalculate(int dic_size);
	KL*& get( int i);
private:
	vector<KL*> _KB;
	int kb_size;
};

KB::KB()
{
}

KB::~KB()
{
}
void KB::recalculate(int dic_size)
{
	thread **t = new thread*[kb_size];
	for (int i = 0; i < kb_size; i++)
	{
		t[i] = new thread(&KL::calculate, _KB[i],dic_size);
	}
	for (int i = 0; i < kb_size; i++)
	{
		t[i]->join();
	}
}
KL*& KB::get(int i)
{
	if (_KB.size() == 0 || i > _KB.size())
	{
		throw new exception();
	}
	else
	{
		return _KB[i];
	}
}
void KB::mergeKB(KB* kb2, std::unordered_map<int, int> old_new_transition)
{
	if (this->kb_size!=kb2->size())
	{
		throw std::exception();
	}
	thread **t = new thread*[kb_size];
	for (int i = 0; i < kb_size; i++)
	{
		t[i] = new thread(&KL::mergeKL, _KB[i], kb2->get(i),old_new_transition);
	}
	for (int i = 0; i < kb_size; i++)
	{
		t[i]->join();
	}
}
void KB::initKB(int count)
{
	kb_size = count;
	for (int i = 0; i < kb_size; i++)
	{
		KL* kl = new KL(i);
		_KB.push_back(kl);
	}
}
void KB::readKB(string path)
{
	thread **t = new thread*[kb_size];
	for (int i = 0; i < kb_size; i++)
	{
		t[i] = new thread(&KL::readKL,_KB[i], path);
	}
	for (int i = 0; i < kb_size; i++)
	{
		t[i]->join();
	}
}
void KB::dumpKB(string path)
{
	cout << "dump KB size = " << kb_size<<"\n";
	thread **t = new thread*[kb_size];
	for (int i = 0; i < kb_size; i++)
	{
		t[i] = new thread(&KL::dumpKL,_KB[i],path);
	}
	for (int i = 0; i < kb_size; i++)
	{
		t[i]->join();
	}
	cout << "done dump KB" << kb_size<<"\n";
}
