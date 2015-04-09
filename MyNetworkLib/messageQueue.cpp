#include "messageQueue.h"

my::MessageQueue::MessageQueue(boost::recursive_mutex& mtx)
{
	m_QueMutex = mtx;
}

my::MessageQueue::~MessageQueue()
{

}



