//
//  AlertHandler.hpp
//  BuildPartsCatalog
//
//  Created by Travis Cobbs on 9/7/25.
//

#ifndef AlertHandler_h
#define AlertHandler_h

#include <TCFoundation/TCObject.h>

class LDLError;
class TCProgressAlert;

class AlertHandler: public TCObject
{
public:
	AlertHandler();
protected:
	~AlertHandler();
	void dealloc();
	void ldlErrorCallback(LDLError *error);
	void progressAlertCallback(TCProgressAlert *alert);
	int lastProgress = -1;
};

#endif /* AlertHandler_h */
