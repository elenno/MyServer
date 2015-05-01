#pragma once

#include "json/json.h"
#include <ctime>
#include <cstdio>
#include <cstdlib>
#include "time_helper.h"
#include "Glog.h"
#include <boost/thread/detail/singleton.hpp>
#define commom_sys boost::detail::thread::singleton<commom>::instance()

#define FalseReturn(X,R) \
	if (!(X)) \
	{\
		return R;\
	}

#define FalseContinue(X) \
	if (!(X)) \
	{\
		continue;\
	}

#define TrueReturn(X,R) \
	if (X)\
	{\
		return R; \
	}

#define String2JsonValue(S, J) \
	{\
		Json::Reader reader; \
		reader.parse(S, J); \
	}

#define CaseBreak(C, X)	\
	case C:	\
	{	\
		X;	\
		break;	\
	}


#define Between(X, L, H) \
	(((X) >= (L) && (X) <= (H)))

#define ForEach(TYPE, ITER, COLLECTION) \
	for (TYPE::iterator ITER = COLLECTION.begin(); ITER != COLLECTION.end(); ITER++)

#define ForEachC(TYPE, ITER, COLLECTION) \
	for (TYPE::const_iterator ITER = COLLECTION.begin(); ITER != COLLECTION.end(); ITER++)

#define GET_CLIENT_PARA_BEG	\
	Json::Value reqJson, respJson;	\
	int error = 0;	\
	String2JsonValue(recv_msg._json_str_utf8, reqJson);

#define GET_CLIENT_PARA_END	\
	if (error != 0)	\
	{	\
	respJson["msg"][0u] = error;	\
	}	\
	respond_str = respJson.toStyledString();	\
	//respond_str = commom_sys.tighten(respond_str);


class commom
{
public:
	commom(void);
	~commom(void);

	Json::Value json_array_remove(Json::Value &j, const unsigned int index);
	/// remove all members that have been set to null value. 
	/// once you were in a big loop, you can only "remove" the member by using "json_array[member_index] = Json::Value::null;"
	/// and then, use this helper function to remove all the members that had been set to null value. of cause, it cause memory copy.
	void		json_array_remove_null_member(Json::Value& json_array) const;
	std::string		tighten(const std::string &str);
	//static tm localTime(time_t now);
	//static tm localTime(void);
	//unsigned nextDay(int offset = 0, unsigned now = 0);
	//unsigned nextHalfHour(unsigned now = 0);
	//unsigned nextHour(unsigned now = 0);
	//unsigned nextWeek(int weekDay = 0, unsigned now = 0);

	int	random();
	void random_array(int a[], int length);
	int randomBetween(int low, int high);
	int randomBetweenPlus(int low, int high);
	bool randomOk(double rate);
	int randomList(const Json::Value &rateList);

	enum ClassType
	{
		int_value = 0,
		string_value = 1,
	};

	bool struct_json_from_str(const std::string& val_utf8_str, Json::Value& resp_json);
	bool check_four_passin_class(const std::string& val_str, ClassType class_1,ClassType class_2,ClassType class_3,ClassType class_4);
	bool check_three_passin_class(const std::string& val_str, ClassType class_1,ClassType class_2,ClassType class_3);
	bool check_two_passin_class(const std::string& val_str, ClassType class_1,ClassType class_2);
	bool check_one_passin_class(const std::string& val_str, ClassType class_type);

private:
	bool check_one_passin_class(Json::Value& val_one, ClassType class_type);
};


