#pragma once

#include <string>

namespace na
{
	namespace util
	{
		
//#define getString(X) global##X
//#define NAME(value) #value
		enum GLogLevel
		{
			GLog_ALL=1,//��͵ȼ��ģ���������־��¼
			GLog_TRACE=2,//����
			GLog_DEBUG=3,//����
			GLog_INFO=4,//��Ϣ
			GLog_WARN=5,//����
			GLog_ERROR=6,//����
			GLog_FATAL=7,//��������
			GLog_SYSTEM,//ϵͳ����
			GLog_OFF//����ߵȼ��ģ��ر�������־��¼

		};
	}
}