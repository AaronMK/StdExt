#ifndef _STD_EXT_UID_H_
#define _STD_EXT_UID_H_

#include "StdExt.h"

#include <stdint.h>

namespace StdExt
{
	class STD_EXT_EXPORT UID final
	{
	public:
		typedef int64_t uid_t;

		UID(const UID&) = delete;
		UID& operator=(const UID&) = delete;

		UID();
		UID(UID&& other);

		~UID();

		UID& operator=(UID&& other);

		operator uid_t() const;

	private:
		uid_t mValue;
	};
}

#endif // !_STD_EXT_UID_H_
