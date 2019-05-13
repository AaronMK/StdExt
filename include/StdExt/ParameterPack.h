#ifndef _STD_EXT_PARAMETER_PACK_H_
#define _STD_EXT_PARAMETER_PACK_H_

#include "Collections/Vector.h"

#include "String.h"
#include "Type.h"
#include "Any.h"

namespace StdExt
{
	class ParameterPack
	{
	private:
		Collections::Vector<Any, 10, 32> mParams;

		template<typename ...args_t>
		void add(args_t...);

		template<>
		void add<>()
		{
		}

		template<typename p1_t, typename ...rest_t>
		void add(p1_t first_arg, rest_t ...rest)
		{
			mParams.emplace_back(make_any<p1_t>(first_arg));
			add(std::forward<rest_t>(rest)...);
		}

	public:
		template<typename ...args_t>
		ParameterPack(args_t ...args)
		{
			mParams.reserve(sizeof...(args));
			add(std::forward<args_t>(args)...);
		}

		size_t size() const
		{
			return mParams.size();
		}

		Any& operator[](size_t index)
		{
			return mParams[index];
		}

		const Any& operator[](size_t index) const
		{
			return mParams[index];
		}
	};
}

#endif // !_STD_EXT_PARAMETER_PACK_H_
