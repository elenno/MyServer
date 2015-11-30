#include "connection_manager.h"
#include "connection.h"

namespace my
{
	namespace http
	{
		ConnectionManager::ConnectionManager()
		{
			m_nCurConnId = HTTP_START_CONN_ID;
		}

		ConnectionManager::~ConnectionManager()
		{
			m_nCurConnId = HTTP_START_CONN_ID;
			stop_all();
		}

		void ConnectionManager::start( Connection::ptr conn )
		{
			conn->set_connection_id(getNextConnId());
			m_ConnectionMap[conn->get_connection_id()] = conn;
			m_ConnectionMap[conn->get_connection_id()]->start();
		}

		void ConnectionManager::stop(int conn_id)
		{
			ConnectionMap::iterator it = m_ConnectionMap.find(conn_id);
			if (it != m_ConnectionMap.end())
			{
				it->second->stop();
				m_ConnectionMap.erase(it);
			}
			
		}

		int ConnectionManager::getNextConnId()
		{
			int ret = m_nCurConnId;
			m_nCurConnId = (m_nCurConnId + 1) % HTTP_END_CONN_ID;
			return ret;
		}

		void ConnectionManager::stop_all()
		{
			try
			{
				ConnectionMap::iterator it = m_ConnectionMap.begin();
				for (; it != m_ConnectionMap.end(); ++it)
				{
					it->second->stop();
				}
				m_ConnectionMap.clear();
			}catch(std::exception& e)
			{

			}
		}

		Connection::ptr ConnectionManager::getConnection( int conn_id )
		{
			return m_ConnectionMap[conn_id];
		}

	}
}


