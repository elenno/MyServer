#pragma once
#include <boost/noncopyable.hpp>
#include <boost/pool/object_pool.hpp>
#include <boost/smart_ptr.hpp>
namespace na
{
	namespace util
	{
		template <typename T,typename Arg1=size_t,typename Arg2=size_t,typename Arg3=size_t>
		class pool_obj
			:private boost::noncopyable
		{			
		public:
			
			virtual ~pool_obj(void){}
			typedef boost::shared_ptr<T>	ptr;

			/// apply a object from pool with different args.
			static ptr		create() 
							{ return ptr(pool_.construct(),release);}	
			static ptr		create(Arg1& a1) 
							{ return ptr(pool_.construct(a1),release);}
			static ptr		create(Arg1& a1,Arg2& a2) 
							{ return ptr(pool_.construct(a1,a2),release);}
			static ptr		create(Arg1& a1,Arg2& a2,Arg3& a3) 
							{ return ptr(pool_.construct(a1,a2,a3),release);}

		protected:
			static void		release(T* p) { pool_.destroy(p); }
			pool_obj(void){}			
			static	boost::object_pool<T>		pool_;
		private:
			
		};
		template  <typename T,typename Arg1,typename Arg2,typename Arg3>
		boost::object_pool< T >		pool_obj< T,Arg1,Arg2,Arg3 >::pool_;
		
	}
}


