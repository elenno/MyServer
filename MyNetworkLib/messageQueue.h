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
		MessageQueue(boost::recursive_mutex& mtx);
		~MessageQueue();

		void push(Message& msg);
	    const Message& front();
		void pop();
		bool empty();

	private:
		std::queue<Message> m_MsgQueue;
		boost::recursive_mutex& m_QueMutex;
	};
}

#endif