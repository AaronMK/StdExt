#ifndef _STD_EXT_UID_H_
#define _STD_EXT_UID_H_

#include "StdExt.h"

#include <stdint.h>

namespace StdExt
{
	/**
	 * @brief
	 *  An object that, upon construction, contains an integer ID that is unique
	 *  within the scope of the compilation unit.
	 */
	class STD_EXT_EXPORT UID final
	{
	public:
		using uid_t = int64_t;

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
