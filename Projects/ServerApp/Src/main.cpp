#include "ServerApplication.h"
//#include <vld.h>

int main(int argc, char** argv)
{
	ServerApplication* application;
	application = new ServerApplication();

	application->run();

	delete application;

	return 0;
}

