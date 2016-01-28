#include "../include/Recall.h"

int main(int argc, char **argv)
{
	Recall *r = new Recall();
	string path = "./KB/"+string(argv[1]);
	r->process(path,argv[1]);
	return 0;
}