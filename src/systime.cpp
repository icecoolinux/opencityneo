#include "systime.h"

#include <sys/time.h>
#include <time.h>

unsigned long SysTime::currentMs(){
	struct timeval now;
	
	gettimeofday(&now, NULL);

	unsigned long ms = ((unsigned long)now.tv_sec)*1000 + ((unsigned long)now.tv_usec)/1000;

	return ms;
}
