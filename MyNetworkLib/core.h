#ifndef __CORE_H__
#define __CORE_H__

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/thread/detail/singleton.hpp>
#include <boost/thread.hpp>

#define core boost::detail::thread::singleton<my::Core>::instance()

namespace my
{
	class Core : private boost::noncopyable
	{
	public:
		typedef boost::shared_ptr<boost::asio::io_service> servicePtr;
		typedef boost::shared_ptr<boost::asio::io_service::work> workPtr;

	public:
		Core();
		~Core();

		void run();
		void stop();
		boost::asio::io_service& getService();
		boost::asio::io_service& getLogicService();
		boost::asio::io_service& getHttpService();

	private:
		size_t m_nPoolSize;
		size_t m_nNextIndex;
		servicePtr m_pLogicService;
		servicePtr m_pHttpService;
		workPtr m_pLogicWork;
		workPtr m_pHttpWork;
		std::vector<servicePtr> m_ServicePool;
		std::vector<workPtr> m_WorkPool;
		std::vector<boost::shared_ptr<boost::thread> > m_ServiceThreads;
	};
}


#endif