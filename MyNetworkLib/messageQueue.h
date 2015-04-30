#ifndef __MESSAGE_QUEUE_H__
#define __MESSAGE_QUEUE_H__

#include "message.h"
#include <boost/thread.hpp>
#include <queue>

namespace my
{
	class MessageQueue
	{
	public:
		MessageQueue();
		~MessageQueue();

		void push(NetMessage& msg);
	    const NetMessage& front();
		void pop();
		bool empty();

	private:
		std::queue<NetMessage> m_MsgQueue;
		boost::recursive_mutex m_QueMutex;
	};
}

#endif