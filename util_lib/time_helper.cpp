#include "time_helper.h"
#include <cstdio>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include "config.h"
#include <boost/thread/detail/singleton.hpp>

namespace na
{
	namespace time_helper
	{
		static const std::string time_zone_str = "time_zone";

		unsigned get_current_time(void)
		{
			return (unsigned)std::time(0);
		}

		unsigned calc_time(time_t start_time,int hour,int min,int sec )
		{
			time_t t = start_time + hour*3600 + min*60 + sec;			
			return (unsigned)t;
		}

		std::string print_time_string(time_t time,bool is_print)
		{
			boost::posix_time::ptime t = boost::posix_time::from_time_t(time);
			std::string s;
			s = to_simple_string(t);
			if(is_print) std::cout << s << std::endl;
			return s;
		}

		bool get_duration_time( time_t time_start, time_t time_end, int& hour, int& min, int& sec )
		{
			boost::posix_time::ptime ts = boost::posix_time::from_time_t(time_start);
			boost::posix_time::ptime te = boost::posix_time::from_time_t(time_end);
			boost::posix_time::time_duration td = te - ts;
			hour = td.hours();
			min = td.minutes() + hour*60;
			sec = td.seconds() + min*60;
			return true;
		}

		int local_time_stamp(time_t now_stamp)
		{
			if (now_stamp == 0)
				get_current_time();

			return now_stamp + (timeZone() * 3600);
		}

		tm localTime(time_t now)
		{
			boost::posix_time::ptime p = boost::posix_time::from_time_t(now + timeZone() * 3600) ;
			return boost::posix_time::to_tm(p);
		}

		tm localTime(void)
		{
			return localTime(get_current_time());
		}

		unsigned get_day_hour(unsigned offset_hour, unsigned now/* = 0*/)
		{
			if (now == 0)
			{
				now = get_current_time();
			}

			tm now_tm = localTime(now);
			return now - now_tm.tm_hour * 3600 - now_tm.tm_min * 60 - now_tm.tm_sec + offset_hour * 3600;
		}

		unsigned nextDay(int offset, unsigned now)
		{
			if (now == 0)
			{
				now = get_current_time();
			}
			now -= offset;
			tm now_tm = localTime(now);
			return now - now_tm.tm_hour * 3600 - now_tm.tm_min * 60 - now_tm.tm_sec + 24 * 3600 + offset;
		}

		unsigned nextHalfHour(unsigned now)
		{
			if (now == 0)
			{
				now = get_current_time();
			}
			tm now_tm = localTime(now);
			return now - now_tm.tm_min * 60 - now_tm.tm_sec + (now_tm.tm_min / 30 * 30 + 30) * 60;
		}

		unsigned nextHour(unsigned now)
		{
			if (now == 0)
			{
				now = get_current_time();
			}
			tm now_tm = localTime(now);
			return now - now_tm.tm_min * 60 + 3600;
		}

		unsigned nextWeek(int offset /* = 0 */, unsigned now /* = 0 */)
		{
			if (now == 0)
			{
				now = get_current_time();
			}
			now -= offset;
			tm now_tm = localTime(now);
			return now - now_tm.tm_wday * 24 * 3600 - now_tm.tm_hour * 3600 - now_tm.tm_min * 60 - now_tm.tm_sec + 7 * 24 * 3600 + offset;
		}

		unsigned next2Minute(unsigned now /* = 0 */)
		{
			if (now == 0)
			{
				now = get_current_time();
			}
			tm now_tm = localTime(now);
			return now - now_tm.tm_min * 60 - now_tm.tm_sec + (now_tm.tm_min / 2 + 1) * 2 * 60;
		}

		void showTime(unsigned t)
		{
			showTime(localTime(t));
		}

		void showTime(tm t)
		{
			printf("%02d-%02d-%02d-%02d-%02d-%02d\n", t.tm_year + 1900, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
		}

		int timeZone(void)
		{
			int time_zone = config_ins.get_config_prame(time_zone_str).asInt();
			if (time_zone == 0)
				time_zone = 8;
			return time_zone;
		}

		void sleep( int micro_secs )
		{
			boost::system_time time = boost::get_system_time();
			time += boost::posix_time::microsec(micro_secs); 
			boost::thread::sleep(time);
		}
	}

}





