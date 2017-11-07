
#ifndef _OPENCITY_TIME_H_
#define _OPENCITY_TIME_H_ 1


class SysTime {
	private:
		unsigned long msInit;
	public:
		
		SysTime();
		
		static unsigned long currentMs();
		
		void start();
		unsigned long stop();
};

#endif
