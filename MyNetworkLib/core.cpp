#include "core.h"

my::Core::Core()
{
	m_ServicePool.clear();
	m_WorkPool.clear();
	m_nPoolSize = 4;
	m_nNextIndex = 0;
	
	for (int i = 0; i < m_nPoolSize; i++)
	{
		servicePtr ptr(new boost::asio::io_service());
		workPtr work(new boost::asio::io_service::work(*ptr));
		m_ServicePool.push_back(ptr);
		m_WorkPool.push_back(work);
	}
}

my::Core::~Core()
{
	m_ServicePool.clear();
	m_WorkPool.clear();
    m_nPoolSize = 0;
	m_nNextIndex = 0;
}

void my::Core::run()
{
	for (int i = 0; i < m_nPoolSize; i++)
	{
		boost::shared_ptr<boost::thread> thread(new boost::thread(boost::bind(&boost::asio::io_service::run, m_ServicePool[i])));
		m_ServiceThreads.push_back(thread);
	}
	for (int i = 0; i < m_nPoolSize; i++)
	{
		m_ServiceThreads[i]->detach();
	}
}

void my::Core::stop()
{
	for (int i = 0; i < m_nPoolSize; i++)
	{
		m_ServicePool[i]->stop();
	}
}

boost::asio::io_service& my::Core::getService()
{
	boost::asio::io_service& service = *(m_ServicePool[m_nNextIndex]);
	m_nNextIndex = (m_nNextIndex + 1) % m_nPoolSize;
	return service;
}