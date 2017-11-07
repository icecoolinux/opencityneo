#include "systime.h"

#include <sys/time.h>
#include <time.h>

SysTime::SysTime()
{
}

unsigned long SysTime::currentMs(){
	struct timeval now;
	
	gettimeofday(&now, NULL);

	unsigned long ms = ((unsigned long)now.tv_sec)*1000 + ((unsigned long)now.tv_usec)/1000;

	return ms;
}

void SysTime::start()
{
	msInit = currentMs();
}

unsigned long SysTime::stop()
{
	return currentMs()-msInit;
}
