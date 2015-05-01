#include "commom.h"
#include <stdlib.h>
#include <time.h>
#include "boost/date_time/posix_time/posix_time.hpp" 
#include "boost/date_time/posix_time/posix_time_types.hpp"
#include <algorithm>

commom::commom(void)
{
	unsigned int seed = (unsigned int)::time(NULL);
	srand(seed);
}


commom::~commom(void)
{
}

Json::Value commom::json_array_remove(Json::Value &j, const unsigned int index)
{
	Json::Value ret;
	unsigned int cur = 0;
	for (unsigned int i = 0; i < j.size(); i++)
	{
		if (i != index)
		{
			ret[cur++] = j[i];
		}
	}
	return ret;
}

std::string	commom::tighten(const std::string &str)
{
	bool in_string = false;
	std::string tmp;
	for (std::string::const_iterator iter = str.begin(); iter != str.end(); iter++)
	{
		char ch = *iter;
		int a = (unsigned)(ch + 1);
		if (Between(a, 0, 256) && isspace(ch) && in_string == false)
		{
			continue;
		}

		tmp.insert(tmp.end(), ch);
		if (ch == '\"')
		{
			in_string = !in_string;
		}
	}
	return tmp;
}


//
//tm commom::localTime(time_t now)
//{
//	boost::posix_time::ptime p = boost::posix_time::from_time_t(now);
//	return boost::posix_time::to_tm(p);
//}
//
//tm commom::localTime(void)
//{
//	return localTime(time(0));
//}
//
//unsigned commom::nextDay(int offset, unsigned now)
//{
//	if (now == 0)
//	{
//		now = (unsigned)time(0);
//	}
//	now -= offset;
//	tm now_tm = localTime(now);
//	return now - now_tm.tm_hour * 3600 - now_tm.tm_min * 60 - now_tm.tm_sec + 24 * 3600 + offset;
//}
//
//unsigned commom::nextHalfHour(unsigned now)
//{
//	if (now == 0)
//	{
//		now = (unsigned)time(0);
//	}
//	tm now_tm = localTime(now);
//	return now - now_tm.tm_min * 60 + (now_tm.tm_min / 30 * 30 + 30) * 60;
//}
//
//unsigned commom::nextHour(unsigned now)
//{
//	if (now == 0)
//	{
//		now = (unsigned)time(0);
//	}
//	tm now_tm = localTime(now);
//	return now - now_tm.tm_min * 60 + 3600;
//}
//
//unsigned commom::nextWeek(int weekDay /* = 0 */, unsigned now /* = 0 */)
//{
//	if (now == 0)
//	{
//		now = (unsigned)time(0);
//	}
//	tm now_tm = localTime(now);
//	return now - now_tm.tm_wday * 24 * 3600 - now_tm.tm_hour * 3600 - now_tm.tm_min * 60 - now_tm.tm_sec + (7 + weekDay) * 24 * 3600;
//}

int commom::random()
{
	return rand();
}

void commom::random_array(int a[], int length)
{
	random_shuffle(a, a + length);
}

int commom::randomBetween(int low, int high)
{
	return random() % (high - low + 1) + low;
}

int commom::randomBetweenPlus(int low, int high)
{
	if (low >= high)
		return low;

	return random() % (high - low + 1) + low;
}

int commom::randomList(const Json::Value &rateList)
{
	static int base = 1000000;
	int rate = random() % 1000 * 1000 + random() % 1000 + 1;
	//rate = (rate + base) % base;
	int low = 1;
	int high = 1;
	for (unsigned i = 0; i < rateList.size(); i++)
	{
		low = high;
		high = high + (int)(rateList[i].asDouble() * base);
		if (Between(rate, low, high))
		{
			return i;
		}
	}
	return 0;
}

bool commom::randomOk(double rate)
{
	static int base = 1000000;
	int rateInt = (int)(rate * base);

	int v = random() % 1000 * 1000 + random() % 1000;
	return v < rateInt;
}

void commom::json_array_remove_null_member( Json::Value& json_array ) const
{
	if(json_array.type()!=Json::arrayValue) return;
	Json::Value new_array = Json::arrayValue;
	for (size_t i=0;i<json_array.size();++i)
	{
		if(!json_array[i].isNull())
			new_array.append(json_array[i]);
	}
	json_array = new_array;
}
bool commom::check_four_passin_class(const std::string& val_str, ClassType class_1,ClassType class_2,ClassType class_3,ClassType class_4)
{	
	Json::Value val_list;
	if(!struct_json_from_str(val_str, val_list))
		return false;

	bool result = true;
	result &= check_one_passin_class(val_list[0u],class_1);
	result &= check_one_passin_class(val_list[1u],class_2);
	result &= check_one_passin_class(val_list[1u],class_3);
	result &= check_one_passin_class(val_list[1u],class_4);
	return result;
}

bool commom::check_three_passin_class(const std::string& val_str, ClassType class_1,ClassType class_2,ClassType class_3)
{
	Json::Value val_list;
	if(!struct_json_from_str(val_str, val_list))
		return false;

	bool result = true;
	result &= check_one_passin_class(val_list[0u],class_1);
	result &= check_one_passin_class(val_list[1u],class_2);
	result &= check_one_passin_class(val_list[1u],class_3);
	return result;
}

bool commom::check_two_passin_class(const std::string& val_str, ClassType class_1,ClassType class_2)
{
	Json::Value val_list;
	if(!struct_json_from_str(val_str, val_list))
		return false;

	bool result = true;
	result &= check_one_passin_class(val_list[0u],class_1);
	result &= check_one_passin_class(val_list[1u],class_2);
	return result;
}

bool commom::check_one_passin_class(const std::string& val_str, ClassType class_type)
{
	Json::Value val;
	if(!struct_json_from_str(val_str, val))
		return false;

	return check_one_passin_class(val[0u],class_type);
}

bool commom::check_one_passin_class(Json::Value& val_one, ClassType class_type)
{
	if (val_one == Json::Value::null)
		return false;

	bool result = false;
	switch(class_type)
	{
	case int_value:
		result = val_one.isInt();
		break;
	case string_value:
		result = val_one.isString();
		break;
	default:
		result = false;
		break;
	}
	return result;
}

bool commom::struct_json_from_str(const std::string& val_utf8_str,Json::Value& resp_json)
{
	try
	{
		Json::Reader reader;
		if(!(reader.parse(val_utf8_str, resp_json)))
			return false;
	}
	catch (std::exception& e)
	{
		return false;
	}

	return true;
}
