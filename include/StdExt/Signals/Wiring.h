#ifndef _STD_EXT_SIGNALS_WIRING_H_
#define _STD_EXT_SIGNALS_WIRING_H_

#include "Subscription.h"
#include "Settable.h"
#include "../Utility.h"

namespace StdExt::Signals
{
	/**
	 * @brief
	 *  Object that will take the output of a Watchable and use it to set a Settable<T>.
	 */
	template<typename T>
	class Pipe : private Subscription<T>
	{
	private:
		EventHandler<T> mDestinationLink;

	public:
		Pipe()
		{
		}

		Pipe(const Watchable<T>& source, const Settable<T>& destination)
		{
			link(source, destination);
		}

		void link(const Watchable<T>& source, const Settable<T>& destination)
		{
			attach(source);
			mDestinationLink.bind(destination);

			Settable<T>* objToSet = cast_pointer<Settable<T>*>(mDestinationLink.source());
			
			if (objToSet)
				objToSet->setValue(value());
		}

	private:
		virtual void onUpdate(const T& val)
		{
			Settable<T>* objToSet = cast_pointer<Settable<T>*>(mDestinationLink.source());

			if (objToSet)
				objToSet->setValue(val);
		}
	};
}

#endif // !_STD_EXT_SIGNALS_WIRING_H_