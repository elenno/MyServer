#pragma once

namespace na
{
	namespace util
	{
		enum GLogColor
		{
			//Black=0,
			#ifdef _WIN_LOG_
			Pink=13,
			Green=10,
			Red=12,
			Yellow=14,
			White=7
			#else
			Pink=35,
			Green=32,
			Red=31,
			Yellow=33,
			White=37
			#endif
		};
	}
}