//
//  main.cpp
//  MacTrans
//
//  Created by Travis Cobbs on 3/7/18.
//  Copyright © 2018 Travis Cobbs. All rights reserved.
//

#include <string>
#include <iostream>
#include "TransLoader.h"
#include "XliffUpdater.h"

int main(int argc, const char * argv[])
{
	if (argc > 3)
	{
		TransLoader loader(argv[1]);
		if (!loader.load())
		{
			std::cout << "error loading trans file " << argv[1] << "\n";
		}
		XliffUpdater updater(loader, argv[2], argv[3]);
		if (!updater.update())
		{
			std::cout << "error updating xliff " << argv[2] << " to " << argv[3] << "\n";
		}
	}
	else
	{
		std::cout << "Usage: MacTrans <Trans Filename> <English Xliff Filename> <Trans Xliff Filename>\n";
	}
	return 0;
}
