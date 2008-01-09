#include "TCImageOptions.h"

TCImageOptions::TCImageOptions(bool compress /*= true*/):
m_compress(compress)
{
#ifdef _LEAK_DEBUG
	strcpy(className, "TCImageOptions");
#endif
}

TCImageOptions::~TCImageOptions(void)
{
}

void TCImageOptions::dealloc(void)
{
	TCObject::dealloc();
}

