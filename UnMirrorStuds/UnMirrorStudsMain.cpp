// UnMirrorStudsMain.cpp : Defines the entry point for the console application.
//

#include "stdio.h"
#include "UnMirrorStuds.h"

int main(int argc, char* argv[])
{
	UnMirrorStuds *unMirrorStuds = new UnMirrorStuds;
	int retValue = unMirrorStuds->run(argc, argv);

	unMirrorStuds->release();
	return retValue;
}

