#ifndef __CONNECTION_MANAGER_H__ 
#define __CONNECTION_MANAGER_H__ 
#include "connection.h"
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/detail/singleton.hpp>

#define httpConnMgr boost::detail::thread::singleton<my::http::ConnectionManager>::instance()

namespace my
{
	namespace http
	{
		class ConnectionManager
		{
		public:
			typedef std::map<int, Connection::ptr> ConnectionMap;

		public:
			ConnectionManager();
			~ConnectionManager();

		public:
			void start(Connection::ptr conn);
			void stop(int conn_id);
			void stop_all();
			Connection::ptr getConnection(int conn_id);

		private:
			int getNextConnId();

		private:
			ConnectionMap m_ConnectionMap;
			int m_nCurConnId;
			static const int HTTP_START_CONN_ID = 0;
			static const int HTTP_END_CONN_ID = 65535;
		};
	}
}

#endif