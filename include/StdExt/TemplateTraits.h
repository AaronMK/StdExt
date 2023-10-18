#ifndef _STD_EXT_TEMPLATE_TRAITS_H_
#define _STD_EXT_TEMPLATE_TRAITS_H_

#include "Type.h"

namespace StdExt
{
	template<typename ...args_t>
	struct ParamTypes
	{
		template<size_t index>
		using type_at = nth_type_t<index, args_t...>;

		static constexpr size_t arg_count = sizeof...(args_t);
	};

	template<auto ...args>
	struct ParamValues
	{
		using Types = ParamTypes<
			decltype(args)...
		>;

		template<size_t index>
		using type_at = Types::type_at<index>;

		template<size_t index>
		static constexpr auto value_at = 
			std::get<index>(
				std::make_tuple(args...)
			);
	};

	static auto tv_pair = ParamValues<2, 3, 5.3>::value_at<2>;
}

#endif // !_STD_EXT_TEMPLATE_TRAITS_H_