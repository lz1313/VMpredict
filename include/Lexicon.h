#include <vector>
#include <algorithm>
using namespace std;
struct Symbol
{
	int id;
	double el;
	Symbol()
	{
		id = 0;
		el = 0.0;
	}
	Symbol(int id)
	{
		this->id = id;
		this->el = 0.0;
	}
	Symbol(int id, double el)
	{
		this->id = id;
		this->el = el;
	}
	bool operator<(const Symbol rhs) const
	{
		return el < rhs.el;
	}
};

struct comp
{
	bool operator()(Symbol* a, Symbol* b)
	{
		return a->el > b->el;
	}
}comp;

class Lexicon
{
public:
	Lexicon(int id);
	~Lexicon();
	Symbol*& get(int i);
	vector<Symbol*>& get();
	void Sort(int N);
	int size(){ return _symbols.size(); }
private:
	vector<Symbol*> _symbols;
	int _id;
};

Lexicon::Lexicon(int id)
{
	_id = id;
}

Lexicon::~Lexicon()
{
}
vector<Symbol*>& Lexicon::get()
{
	return _symbols;
}
Symbol*& Lexicon::get(int i)
{
	return _symbols[i];
}
void Lexicon::Sort(int N)
{
	//sort
	sort(_symbols.begin(), _symbols.end(), comp);
	if (N<_symbols.size())
	{
		_symbols.resize(N);
	}	
	//normalize
	double sum = 0.0;
	for (int i = 0; i < _symbols.size(); i++)
	{
		sum += _symbols[i]->el;
	}
	for (int i = 0; i < _symbols.size(); i++)
	{
		_symbols[i]->el = _symbols[i]->el / sum;
	}
}