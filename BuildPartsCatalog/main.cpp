//
//  main.cpp
//  BuildPartsCatalog
//
//  Created by Travis Cobbs on 9/1/25.
//

#include "PartsCatalog.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <fstream>
#include <LDLoader/LDLMainModel.h>

int main(int argc, const char * argv[]) {
	PartsCatalog partsCatalog;
	if (!partsCatalog.build()) {
		return 1;
	}
	return 0;
}
