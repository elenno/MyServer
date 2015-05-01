#pragma once
#include <time.h>
#include <string>
#include <boost/thread/thread_time.hpp>
#include "Glog.h"
namespace na
{
	namespace time_helper
	{
		static const time_t ONE_DAY_SECOND = 86400;
		
		std::string				print_time_string(time_t t,bool is_print = false);
		unsigned				get_current_time(void);
		unsigned				calc_time(time_t start_time,int hour,int min,int sec);
		bool					get_duration_time(time_t time_start, time_t time_end, int& hour, int& min, int& sec);

		tm						localTime(time_t now);
		tm						localTime(void);
		int						local_time_stamp(time_t now_stamp = 0);

		unsigned				get_day_hour(unsigned hour, unsigned now = 0);
		unsigned				nextDay(int offset = 0, unsigned now = 0); //next offset time!
		unsigned				nextHalfHour(unsigned now = 0);
		unsigned				nextHour(unsigned now = 0);
		unsigned				nextWeek(int offset = 0, unsigned now = 0);
		unsigned				next2Minute(unsigned now = 0);

		void					showTime(unsigned t);
		void					showTime(tm t);

		int						timeZone(void);
		//microsecond 
		void					sleep(int micro_secs);
	}
}


class time_logger
{
	boost::system_time start_time;
	boost::system_time end_time;
	std::string str;
public:
	time_logger(const char* s)
	{
		start_time = boost::get_system_time();
		str = s;
	}
	~time_logger()
	{
		end_time = boost::get_system_time();
		float delta = (float)((end_time - start_time).total_nanoseconds()/1000000);
		if(delta > 50)
		{
			LogD << str << "\t" << delta << " ms" << LogEnd;
		}
	}
};


// example 
//#pragma once
//#include <time_helper.h>
//#include <iostream>
//void main()
//{
//	time_t t = na::time_helper::get_current_time();
//	time_t t1 = na::time_helper::calc_time(t,24,20,10);
//	na::time_helper::print_time_string(t,true);
//	na::time_helper::print_time_string(t1,true);
//	int hour,min,sec;
//	na::time_helper::get_duration_time(t,t1,hour,min,sec);
//	std::cout << "hours:\t" <<hour << "\tmins:\t" << min << "\tsecs:\t" << sec << std::endl;
//}


// output:
//		2011-Dec-13 05:08:43
//		2011-Dec-14 05:28:53
//		hours:  24      mins:   1460    secs:   87610
//		Çë°´ÈÎÒâ¼ü¼ÌÐø. . .
