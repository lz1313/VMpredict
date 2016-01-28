#include "Parameter.h"
#include "Lexicon.h"
#include "KB.h"
#include "STtable.h"
#include "Dictionary.h"
#include "CSVReader.h"

class Recall
{
public:
	Recall();
	~Recall();
	void process(string path = "./KB", string filename = "./TestingSet.csv");
private:
	vector<string> inputWeekday;
	vector<string> inputTime;
	vector<string> refCPU;
	vector<string> refBW;
	vector<string> refMEM;
	vector<string> predictCPU;
	vector<string> predictBW;
	vector<string> predictMEM;
	int size;
	STtable* _table;
	KB* _kb;
	Dictionary* _dic;
	Lexicon* _lex[MAX_LEX];
	double locate_kb(int kb_count, int source, int target);
	void ParseInput(string filename);
	vector<int> ConstructLexicon(int offset);
	void RecallAlgo(vector<int> unknown_lex);
	void outputSentence();
	void readDB(string path = "./KB");
};

Recall::Recall()
{
	_table = new STtable();
	_dic = new Dictionary();
	_kb = new KB();
	for (int i = 0; i < MAX_LEX; i++)
	{
		_lex[i] = new Lexicon(i);
	}
}

Recall::~Recall()
{
	delete _table;
	delete _kb;
	delete _dic;
	for (int i = 0; i < MAX_LEX; i++)
	{
		delete _lex[i];
	}
}
void Recall::process(string path, string filename)
{
	readDB(path);
	ParseInput("./data/"+filename);
	cout << "done parsing\n";
	for (int i = 0; i < DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i++)
	{
		predictBW.push_back(refBW[i]);
		predictCPU.push_back(refCPU[i]);
		predictMEM.push_back(refMEM[i]);
	}
	int linecount;
	for (linecount = 0; linecount + DEFAULT_SENTENCE_LENGTH <= size; linecount += PREDICTION_WINDOW)
	{
		cout << linecount << "\n";
		RecallAlgo(ConstructLexicon(linecount));
		outputSentence();
	}
	ofstream out("./output/" + filename,std::ios::out);
	if (!out.is_open())
	{
		std::cout << "ERROR: cannot open output" << filename << "\n";
		exit(1);
	}
	out << "WEEKDAY,TIME,CPU,PREDICTCPU,BW,PREDICTBW,MEM,PREDICTMEM\n";
	for (int i = DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i < linecount + DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i++)
	{
		out << inputWeekday[i] << "," << inputTime[i] << "," << refCPU[i] << "," << predictCPU[i]<<","
		    << refBW[i] << "," << predictBW[i] <<","<< refMEM[i] << "," << predictMEM[i] << "\n";
	}
	out.close();
}
void Recall::readDB(string path)
{
	_dic->readDic(path);
	_kb->initKB(KB_NUM);
	_kb->readKB(path);
	_table->readST(path);
}
void Recall::ParseInput(string filename)
{
	std::ifstream in(filename,std::ios::in);
	if (!in.is_open())
	{
		std::cout << "ERROR: cannot open " << filename << "\n";
		exit(1);
	}
	CSVRow row;
	size = 0;
	while (in >> row)
	{
		/*if (size >= 500)
		{
			break;
		}*/
		//cout<<size<<"\n";
		inputWeekday.push_back(row[2]);
		inputTime.push_back(row[3]);
		refCPU.push_back(row[7]);
		refBW.push_back(row[8]);
		refMEM.push_back(row[9]);
		size++;

	}
	in.close();
}
vector<int> Recall::ConstructLexicon(int offset)
{
	std::vector<int> unknown_lex;
	int lex_count = 0;
	//hash usage
	for (int i = 0; i < DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i++, lex_count++)
	{
		int hash_input;
		hash_input = _dic->get_hash(refCPU[offset + i]);
		if (hash_input != -1)
		{
			_lex[lex_count]->get().push_back(new Symbol(hash_input));
		}
	}
	for (int i = DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i < DEFAULT_SENTENCE_LENGTH; i++, lex_count++)
	{
		unknown_lex.push_back(lex_count);
	}
	for (int i = 0; i < DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i++, lex_count++)
	{
		int hash_input;
		hash_input = _dic->get_hash(refBW[offset + i]);
		if (hash_input != -1)
		{
			_lex[lex_count]->get().push_back(new Symbol(hash_input));
		}
	}
	for (int i = DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i < DEFAULT_SENTENCE_LENGTH; i++, lex_count++)
	{
		unknown_lex.push_back(lex_count);
	}
	for (int i = 0; i < DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i++, lex_count++)
	{
		int hash_input;
		hash_input = _dic->get_hash(refMEM[offset + i]);
		if (hash_input != -1)
		{
			_lex[lex_count]->get().push_back(new Symbol(hash_input));
		}
	}
	for (int i = DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i < DEFAULT_SENTENCE_LENGTH; i++, lex_count++)
	{
		unknown_lex.push_back(lex_count);
	}
	//hash time
	for (int i = 0; i < DEFAULT_SENTENCE_LENGTH; i++, lex_count++)
	{
		int hash_input = _dic->get_hash(inputTime[i + offset]);
		if (hash_input != -1)
		{
			_lex[lex_count]->get().push_back(new Symbol(hash_input));
		}
	}
	//hash weekday
	for (int i = 0; i < DEFAULT_SENTENCE_LENGTH; i++, lex_count++)
	{
		int hash_input = _dic->get_hash(inputWeekday[i + offset]);
		if (hash_input != -1)
		{
			_lex[lex_count]->get().push_back(new Symbol(hash_input));
		}
	}
	std::cout << "\n";
	return unknown_lex;
}
void Recall::RecallAlgo(vector<int> unknown_lex)
{
	std::cout << "initialize el_array\n";
	for (int source_count = 0; source_count < MAX_LEX; source_count++)
	{
		if (_lex[source_count]->size() == 0)
		{
			for (int i = 0; i < _dic->Size(); i++)
			{
				_lex[source_count]->get().push_back(new Symbol(i, (double)1.0 / (1.0 * _dic->Size())));
			}
		}
		else
		{
			for (int i = 0; i < _lex[source_count]->size(); i++)
			{
				_lex[source_count]->get(i)->el = (double)1.0 / (1.0 * _lex[source_count]->size());
			}
		}
	}
	//recall algorithm
	for (int N = MAX_AMBIGUOUS; N > 0; N--)
	{
		int iteration_count = 0;
		bool converged = false;
		while (!converged)
		{
			converged = true;
			for (int unknown_count = unknown_lex.size() - 1; unknown_count >= 0; unknown_count--)
			{
				int target_lex = unknown_lex[unknown_count];
				std::cout << "target lex: " << target_lex << " size: " << _lex[target_lex]->size() << "\n";
				int target_size = _lex[target_lex]->size();
				for (int target_count = 0; target_count < target_size; target_count++)
				{
					int bgapflag[MAX_LEX];
					for (int i = 0; i < MAX_LEX; i++)
					{
						bgapflag[i] = 1;
					}
					for (int source_count = 0; source_count < MAX_LEX; source_count++)
					{
						int kb = this->_table->source_target_array()[source_count][target_lex];
						if (kb == -1) continue;
						for (int source_id = 0; source_id < _lex[source_count]->size(); source_id++)
						{
							int source_sym = _lex[source_count]->get(source_id)->id;
							int target_sym = _lex[target_lex]->get(target_count)->id;
							double kb_value = locate_kb(kb, source_sym, target_sym);
							if (kb_value == -1000)
							{
								continue;
							}
							double weight = 1.0;
							/*if (source_count >=DEFAULT_SENTENCE_LENGTH&&source_count<=DEFAULT_SENTENCE_LENGTH*2)
							{
							weight*=10.0;
							}
							if (source_count<DEFAULT_SENTENCE_LENGTH)
							{
							//weight/=10.0;
							}*/
							_lex[target_lex]->get(target_count)->el += kb_value * _lex[source_count]->get(source_id)->el * weight + BANDGAP * bgapflag[source_count];
							bgapflag[source_count] = 0;
						}
					}
				}
				vector<Symbol*> temp(_lex[target_lex]->get());
				_lex[target_lex]->Sort(N);
				for (int i = 0; i < _lex[target_lex]->size(); ++i)
				{
					cout << _dic->Rdictionary()[_lex[target_lex]->get(i)->id] << ":" << _lex[target_lex]->get(i)->el << "\n";
				}
				//see if converged
				//std::cout << "check converge\n";
				if (temp.size() != _lex[target_lex]->size()) converged = false;
				else
				{
					for (int i = 0; i < temp.size(); i++)
					{
						if (_lex[target_lex]->get(i)->id != temp[i]->id)
						{
							converged = false;
							break;
						}
					}
				}
			}
			iteration_count++;
			//if converged
			if (iteration_count >= MAX_ITERATION) converged = true;
		}
	}
}
double Recall::locate_kb(int kb_count, int source, int target)
{
	std::unordered_map<Key, double, KeyHasher>::iterator it = _kb->get(kb_count)->KLINK().find(Key(source, target));
	if (it != _kb->get(kb_count)->KLINK().end())
	{
		return it->second;
	}
	else
	{
		return -1000.0;
	}
}
void Recall::outputSentence()
{

	for (int i = DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i < DEFAULT_SENTENCE_LENGTH; i++)
	{
		predictCPU.push_back(_dic->Rdictionary()[_lex[i]->get(0)->id]);
	}


	for (int i = 2*DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i < 2*DEFAULT_SENTENCE_LENGTH; i++)
	{
		predictBW.push_back(_dic->Rdictionary()[_lex[i]->get(0)->id]);
	}

	for (int i = 3*DEFAULT_SENTENCE_LENGTH - PREDICTION_WINDOW; i < 3*DEFAULT_SENTENCE_LENGTH; i++)
	{
		predictMEM.push_back(_dic->Rdictionary()[_lex[i]->get(0)->id]);
	}
	for (int i = 0; i < MAX_LEX; i++)
	{
		_lex[i]->get().clear();
	}
}