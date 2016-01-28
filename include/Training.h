#include "Parameter.h"
#include "Lexicon.h"
#include "KB.h"
#include "STtable.h"
#include "Dictionary.h"
#include "CSVReader.h"

class Training
{
public:
	Training();
	~Training();
	void InitTopo();
	void DoTraining(std::string filename);
	void Dump(std::string path = "./KB");
	void ReCalculate(std::string path, std::string outpath);
	void process(std::string filename = "TrainingSet.csv")
	{
		string path = "./KB/"+filename;
		filename = "./data/"+filename;
		InitTopo();
		DoTraining(filename);
		Dump(path);
	}
private:
	STtable* _table;
	KB* _kb;
	Dictionary* _dic;
	Lexicon* _lex[MAX_LEX];
	int sentence_length;
};

Training::Training()
{
	_table = new STtable();
	_dic = new Dictionary();
	_kb = new KB();
	for (int i = 0; i < MAX_LEX; i++)
	{
		_lex[i] = new Lexicon(i);
	}
}

Training::~Training()
{
	delete _table;
	delete _kb;
	delete _dic;
	for (int i = 0; i < MAX_LEX; i++)
	{
		delete _lex[i];
	}
}
void Training::ReCalculate(std::string path, std::string outpath)
{
	_dic->readDic(path);
	_kb->initKB(KB_NUM);
	_kb->readKB(path);
	_kb->recalculate(_dic->Size());
	_kb->dumpKB(outpath);
}
void Training::Dump(std::string path)
{
	string command = "mkdir -p "+path;
	system(command.c_str());
	_table->dumpST(path);
	_kb->dumpKB(path);
	_dic->dumpDic(path);
}
void Training::InitTopo()
{
	int i, j, kb_count, distance;
	kb_count = 0;
	//KB among CPU usage
	for (distance = 1; distance <= NGRAM; distance++) {
		for (i = 0; i < DEFAULT_SENTENCE_LENGTH; i++) {
			for (j = i + 1; j < DEFAULT_SENTENCE_LENGTH; j++) {
				if (j - i == distance) {
					_table->source_target_array()[i][j] = kb_count;
					_table->source_target_array()[j][i] = kb_count + 1;
				}
			}
		}
		kb_count = kb_count + 2;
	}
	//KB among BW usage
	for (distance = 1; distance <= NGRAM; distance++) {
		for (i = DEFAULT_SENTENCE_LENGTH; i <2* DEFAULT_SENTENCE_LENGTH; i++) {
			for (j = i + 1; j <2* DEFAULT_SENTENCE_LENGTH; j++) {
				if (j - i == distance) {
					_table->source_target_array()[i][j] = kb_count;
					_table->source_target_array()[j][i] = kb_count + 1;
				}
			}
		}
		kb_count = kb_count + 2;
	}
	//KB among MEM usage
	for (distance = 1; distance <= NGRAM; distance++) {
		for (i = 2*DEFAULT_SENTENCE_LENGTH; i < 3*DEFAULT_SENTENCE_LENGTH; i++) {
			for (j = i + 1; j <3* DEFAULT_SENTENCE_LENGTH; j++) {
				if (j - i == distance) {
					_table->source_target_array()[i][j] = kb_count;
					_table->source_target_array()[j][i] = kb_count + 1;
				}
			}
		}
		kb_count = kb_count + 2;
	}
	//KB between CPU and BW
	for (distance = -1 * NGRAM; distance <= NGRAM; distance++) {
		for (i = 0; i < DEFAULT_SENTENCE_LENGTH; i++) {
			for (j = DEFAULT_SENTENCE_LENGTH; j < 2 * DEFAULT_SENTENCE_LENGTH; j++) {
				if ((j - DEFAULT_SENTENCE_LENGTH) - i == distance) {
					_table->source_target_array()[i][j] = kb_count;
					_table->source_target_array()[j][i] = kb_count + 1;
				}
			}
		}
		kb_count = kb_count + 2;
	}
	//KB between CPU and MEM
	for (distance = -1 * NGRAM; distance <= NGRAM; distance++) {
		for (i = 0; i < DEFAULT_SENTENCE_LENGTH; i++) {
			for (j = 2*DEFAULT_SENTENCE_LENGTH; j < 3 * DEFAULT_SENTENCE_LENGTH; j++) {
				if ((j -2* DEFAULT_SENTENCE_LENGTH) - i == distance) {
					_table->source_target_array()[i][j] = kb_count;
					_table->source_target_array()[j][i] = kb_count + 1;
				}
			}
		}
		kb_count = kb_count + 2;
	}
	//KB between BW and MEM
	for (distance = -1 * NGRAM; distance <= NGRAM; distance++) {
		for (i = DEFAULT_SENTENCE_LENGTH; i <2* DEFAULT_SENTENCE_LENGTH; i++) {
			for (j =2* DEFAULT_SENTENCE_LENGTH; j < 3 * DEFAULT_SENTENCE_LENGTH; j++) {
				if ((j - 2* DEFAULT_SENTENCE_LENGTH) - (i-DEFAULT_SENTENCE_LENGTH) == distance) {
					_table->source_target_array()[i][j] = kb_count;
					_table->source_target_array()[j][i] = kb_count + 1;
				}
			}
		}
		kb_count = kb_count + 2;
	}
	//KB between CPU and time
	for (distance = -1 * NGRAM; distance <= NGRAM; distance++) {
		for (i = 0; i < DEFAULT_SENTENCE_LENGTH; i++) {
			for (j =3* DEFAULT_SENTENCE_LENGTH; j < 4 * DEFAULT_SENTENCE_LENGTH; j++) {
				if ((j -3* DEFAULT_SENTENCE_LENGTH) - i == distance) {
					_table->source_target_array()[i][j] = kb_count;
					_table->source_target_array()[j][i] = kb_count + 1;
				}
			}
		}
		kb_count = kb_count + 2;
	}
	//KB between BW and time
	for (distance = -1 * NGRAM; distance <= NGRAM; distance++) {
		for (i = DEFAULT_SENTENCE_LENGTH; i < 2 * DEFAULT_SENTENCE_LENGTH; i++) {
			for (j = 3 * DEFAULT_SENTENCE_LENGTH; j < 4 * DEFAULT_SENTENCE_LENGTH; j++) {
				if ((j - 3 * DEFAULT_SENTENCE_LENGTH) - (i-DEFAULT_SENTENCE_LENGTH) == distance) {
					_table->source_target_array()[i][j] = kb_count;
					_table->source_target_array()[j][i] = kb_count + 1;
				}
			}
		}
		kb_count = kb_count + 2;
	}
	//KB between MEM and time
	for (distance = -1 * NGRAM; distance <= NGRAM; distance++) {
		for (i = 2*DEFAULT_SENTENCE_LENGTH; i < 3*DEFAULT_SENTENCE_LENGTH; i++) {
			for (j = 3 * DEFAULT_SENTENCE_LENGTH; j < 4 * DEFAULT_SENTENCE_LENGTH; j++) {
				if ((j - 3 * DEFAULT_SENTENCE_LENGTH) - (i-2*DEFAULT_SENTENCE_LENGTH) == distance) {
					_table->source_target_array()[i][j] = kb_count;
					_table->source_target_array()[j][i] = kb_count + 1;
				}
			}
		}
		kb_count = kb_count + 2;
	}
	//KB between CPU and weekday

	for (i = 0; i < DEFAULT_SENTENCE_LENGTH; i++) {
		for (j = 4 * DEFAULT_SENTENCE_LENGTH; j < 5 * DEFAULT_SENTENCE_LENGTH; j++) {
			if ((j - 4 * DEFAULT_SENTENCE_LENGTH) - i == 0) {
				_table->source_target_array()[i][j] = kb_count;
				_table->source_target_array()[j][i] = kb_count + 1;
			}
		}
	}
	//KB between BW and weekday

	for (i = DEFAULT_SENTENCE_LENGTH; i <2* DEFAULT_SENTENCE_LENGTH; i++) {
		for (j = 4 * DEFAULT_SENTENCE_LENGTH; j < 5 * DEFAULT_SENTENCE_LENGTH; j++) {
			if ((j - 4 * DEFAULT_SENTENCE_LENGTH) - (i-DEFAULT_SENTENCE_LENGTH) == 0) {
				_table->source_target_array()[i][j] = kb_count;
				_table->source_target_array()[j][i] = kb_count + 1;
			}
		}
	}
	//KB between MEM and weekday

	for (i = 2*DEFAULT_SENTENCE_LENGTH; i <3* DEFAULT_SENTENCE_LENGTH; i++) {
		for (j = 4 * DEFAULT_SENTENCE_LENGTH; j < 5 * DEFAULT_SENTENCE_LENGTH; j++) {
			if ((j - 4 * DEFAULT_SENTENCE_LENGTH) - (i-2*DEFAULT_SENTENCE_LENGTH) == 0) {
				_table->source_target_array()[i][j] = kb_count;
				_table->source_target_array()[j][i] = kb_count + 1;
			}
		}
	}
	kb_count = kb_count + 2;
	_kb->initKB(kb_count);
	std::cout << "num_KB = " << kb_count << "\n";
}
void Training::DoTraining(std::string filename)
{
	vector<string> inputWeekday;
	vector<string> inputTime;
	vector<string> inputCPU;
	vector<string> inputBW;
	vector<string> inputMEM;
	std::ifstream in(filename, std::ios::in);
	if (!in.is_open())
	{
		std::cout << "ERROR: cannot open " << filename << "\n";
		exit(1);
	}
	CSVRow row;
	in >> row;
	/*for (int i = 0; i < row.size(); i++)
	{
		cout << row[i] << " ";
	}
	cout << "\n";*/
	int size = 0;
	while (in >> row)
	{
		//cout<<size<<"\n";
		inputWeekday.push_back(row[2]);
		inputTime.push_back(row[3]);
		inputCPU.push_back(row[7]);
		inputBW.push_back(row[8]);
		inputMEM.push_back(row[9]);
		size++;
	}
	//cout << "done reading\n";
	for (int i = 0; i + DEFAULT_SENTENCE_LENGTH <= size; i += DEFAULT_SENTENCE_LENGTH - OVERLAP)
	{
		for (int j = 0; j < MAX_LEX; j++)
		{
			_lex[j]->get().clear();
		}
		int word_count = 0;
		//cout<<"starting at "<<i<<"\n";
		for (word_count = 0; word_count < DEFAULT_SENTENCE_LENGTH; word_count++) {
			//cout<<word_count<<":"<<inputCPU[i + word_count]<<"\n";
			_lex[word_count]->get().push_back(new Symbol(_dic->hash_word(inputCPU[i + word_count])));
		}
		for (; word_count < 2 * DEFAULT_SENTENCE_LENGTH; word_count++)
		{
			//cout << word_count << ":" << inputBW[i + word_count - DEFAULT_SENTENCE_LENGTH] << "\n";
			_lex[word_count]->get().push_back(new Symbol(_dic->hash_word(inputBW[i + word_count - DEFAULT_SENTENCE_LENGTH])));
		}
		for (; word_count < 3 * DEFAULT_SENTENCE_LENGTH; word_count++)
		{
			//cout << word_count << ":" << inputMEM[i + word_count -2* DEFAULT_SENTENCE_LENGTH] << "\n";
			_lex[word_count]->get().push_back(new Symbol(_dic->hash_word(inputMEM[i + word_count - 2* DEFAULT_SENTENCE_LENGTH])));
		}
		//cout<<"done hash usage\n";
		for (; word_count < 4 * DEFAULT_SENTENCE_LENGTH; word_count++)
		{
			//cout<<word_count<<":"<<inputTime[i + word_count - 3*DEFAULT_SENTENCE_LENGTH]<<"\n";
			_lex[word_count]->get().push_back(new Symbol(_dic->hash_word(inputTime[i + word_count -3* DEFAULT_SENTENCE_LENGTH])));
		}
		//cout<<"done hash time\n";
		for (; word_count < 5 * DEFAULT_SENTENCE_LENGTH; word_count++)
		{
			//cout<<word_count<<":"<<inputWeekday[i + word_count - 4 * DEFAULT_SENTENCE_LENGTH]<<"\n";
			_lex[word_count]->get().push_back(new Symbol(_dic->hash_word(inputWeekday[i + word_count - 4 * DEFAULT_SENTENCE_LENGTH])));
		}
		//cout<<"done hash weekday\n";
		//update
		for (int i = 0; i < MAX_LEX; i++) {
			for (int j = 0; j < MAX_LEX; j++) {
				if (_lex[i]->size() == 0 || _lex[j]->size() == 0) continue;
				if (_table->source_target_array()[i][j] == -1) continue;
				int source = _lex[i]->get(0)->id;
				int target = _lex[j]->get(0)->id;
				_kb->get(_table->source_target_array()[i][j])->incrementKL(source, target);
			}
		}
	}
	in.close();
}