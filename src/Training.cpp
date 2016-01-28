#include "../include/Training.h"

int main(int argc, char **argv)
{
	Training *t = new Training();
	t->process(argv[1]);
	string path = "./KB/"+string(argv[1]);
	t->ReCalculate(path,path);
	return 0;
}