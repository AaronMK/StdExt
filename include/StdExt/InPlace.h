#ifndef _STD_EXT_IN_PLACE_H_
#define _STD_EXT_IN_PLACE_H_

#include "Memory.h"

#include <typeindex>
#include <type_traits>

namespace StdExt
{
	/**
	 * @brief
	 *  %Container that provides in-place storage and type base access for types of
	 *  and sub-classes of base_t that, when properly aligned, occupy maxSize or
	 *  less memory.
	 *
	 * @tparam base_t
	 *  The base type of all objects that will be contained within this object.
	 *
	 * @tparam maxSize
	 *  The maximum size a properly alligned object can be for in-object storage.
	 *
	 * @tparam localOnly
	 *  If true, attempting to set the value of this object to something too big to
	 *  fit in the local storage will raise a std::bad_alloc exception.  If false
	 *  oversized values will be stored on the heap, while local storage optimizations
	 *  will still be provided objects small enough to fit in local storage.
	 */
	template<typename base_t, size_t maxSize, bool localOnly = false>
	class InPlace final
	{
	private:

		/**
		 * @brief
		 *  Base class for the internal container used by inplace.  The subclass actually used is
		 *  determined by whether or not the contained item can fit in the local buffer.  The maxSize
		 *  template parameter of InPlace determines how big a %Container can be, and provides all
		 *  data needed for subclasses.
		 */
		class Container
		{
		public:
			/**
			 * @brief
			 *  A pointer to the actual stored object.  If the container is empty, this will be null.  For
			 *  LocalContainer, this will point to <I>objData</I>.  For RemoteContainer, this will
			 *  point to heap allocated memory.
			 */
			base_t* objPtr;

			/**
			 * @brief
			 *  Memory for storage of the actual object when it can fit in this local memory.
			 */
			char objData[maxSize];

			Container()
			{
				objPtr = nullptr;
			}

			virtual ~Container()
			{
			}

			/**
			 * @brief
			 *  Constructs a %Container of the object's %Container sub-type at <I>destination</I>, and moves it's
			 *  contents into that newly created %Container.
			 */
			virtual void move(void* destination) = 0;

			/**
			 * @internal
			 *  
			 * @brief
			 *  Creates a %Container of the object's %Container sub-type at <I>destination</I>, and copies it's
			 *  contents into that newly created %Container.
			 */
			virtual void copy(void* destination) const = 0;

			/**
			 * @internal
			 *  
			 * @brief
			 *  Gets the std::type_index of the contained item.
			 */
			virtual std::type_index typeIndex() const = 0;

			/**
			 * @internal
			 *  
			 * @brief
			 *  Gets the std::type_info of the contained item.
			 */
			virtual const std::type_info& typeInfo() const = 0;
		};

		/**
		 * @brief
		 *  An placeholder used when the parent InPlace is empty.
		 */
		class NullContainer : public Container
		{
		public:
			NullContainer()
			{
				objPtr = nullptr;
			}

			virtual ~NullContainer()
			{
			}

			virtual void move(void* destination) override
			{
				new(destination) NullContainer();
			}

			virtual void copy(void* destination) const override
			{
				new(destination) NullContainer();
			}
			
			virtual std::type_index typeIndex() const override
			{
				return std::type_index(typeid(void));
			}

			virtual const std::type_info& typeInfo() const override
			{
				return typeid(void);
			}
		};

		/**
		 * @brief
		 *  Container for an object that InPlace stores on the heap when there is not enough
		 *  local memory.
		 */
		template<typename sub_t>
		class RemoteContainer : public Container
		{
		public:

			RemoteContainer()
			{
				objPtr = nullptr;
			}

			/**
			 * @brief
			 *  If <I>initialize</I> is true, <I>arguments</I> will be used as construction parameters
			 *  for a <I>sub_t</I>object to inhabit the container.  Otherwise, an empty container will be
			 *  created.
			 */
			template<typename... Args>
			RemoteContainer(bool initialize, Args&& ...arguments)
			{
				if (initialize)
					objPtr = new(_aligned_malloc(sizeof(sub_t), alignof(sub_t))) sub_t(std::forward<Args>(arguments)...);
				else
					objPtr = nullptr;
			}

			/**
			 * @brief
			 *  Also destructs any contained object.
			 */
			virtual ~RemoteContainer()
			{
				if (nullptr != objPtr)
				{
					std::destroy_at(reinterpret_cast<sub_t*>(objPtr));
					_aligned_free(objPtr);
				}
			}

			virtual void move(void* destination) override
			{
				
				if constexpr (std::is_move_constructible_v<sub_t>)
				{
					RemoteContainer<sub_t>* otherContainer = new(destination) RemoteContainer<sub_t>();

					otherContainer->objPtr = srcContainer->objPtr;
					objPtr = nullptr;
				}
				else
				{
					throw invalid_operation("Attempting move on a type that does not support it.");
				}
			}

			virtual void copy(void* destination) const override
			{
				if constexpr (std::is_copy_constructible_v<sub_t>)
				{
					new(destination) RemoteContainer<sub_t>(true, *reinterpret_cast<sub_t*>(srcContainer->objPtr));
				}
				else
				{
					throw invalid_operation("Attempting copy on a type that does not support it.");
				}
			}

			virtual std::type_index typeIndex() const override
			{
				return std::type_index(typeid(sub_t));
			}

			virtual const std::type_info& typeInfo() const override
			{
				return typeid(sub_t);
			}
		};

		/**
		 * @internal
		 *
		 * @brief
		 *  Container for an object that InPlace stores within its local memory.
		 */
		template<typename sub_t>
		class LocalContainer : public Container
		{
		public:

			/**
			 * @brief
			 *  If it will fit in local storage, <I>arguments</I> are used to contruct an object of
			 *  <I>sub_t</I> in the local storage, and <I>objPtr</I> will point to that object. 
			 *  Otherwise, the container will be empty and <I>objPtr</I> will be null.
			 */
			template<typename... Args>
			LocalContainer(Args ...arguments)
			{
				objPtr = reinterpret_cast<sub_t*>(&objData[0]);
				objPtr = align<sub_t>(objPtr, sizeof(objData));

				if (nullptr != objPtr)
					new(objPtr) sub_t(std::forward<Args>(arguments)...);
			}

			/**
			 * @brief
			 *  Also destructs any contained object.
			 */
			virtual ~LocalContainer()
			{
				if (nullptr != objPtr)
					std::destroy_at<sub_t>(reinterpret_cast<sub_t*>(objPtr));
			}

			virtual void move(void* destination) override
			{
				if constexpr (std::is_move_constructible_v<sub_t>)
				{
					sub_t* ptr = reinterpret_cast<sub_t*>(objPtr);
					new(destination) LocalContainer<sub_t>(std::move(*ptr));
				}
				else
				{
					throw invalid_operation("Attempting move on a type that does not support it.");
				}
			}

			virtual void copy(void* destination) const override
			{
				if constexpr (std::is_copy_constructible_v<sub_t>)
				{
					sub_t* ptr = reinterpret_cast<sub_t*>(objPtr);
					new(destination) LocalContainer<sub_t>(*ptr);
				}
				else
				{
					throw invalid_operation("Attempting copy on a type that does not support it.");
				}
			}

			virtual std::type_index typeIndex() const override
			{
				return std::type_index(typeid(sub_t));
			}

			virtual const std::type_info& typeInfo() const override
			{
				return typeid(sub_t);
			}
		};

		/**
		* @brief
		*  In memory where a Container derived wrapper for the stored value is kept in this %InPlace
		*  object.
		*/
		char mContainerMemory[sizeof(Container)];

		inline Container* container()
		{
			return reinterpret_cast<Container*>(&mContainerMemory[0]);
		}

		inline const Container* container() const
		{
			return reinterpret_cast<const Container*>(&mContainerMemory[0]);
		}

		#if defined(_DEBUG)
		/**
		 * @brief
		 *  A pointer to the container that wraps the value of this %InPlace object.  When null, this is
		 *  empty.  When non-null, it points to mContainerMemory, providing convenient access to the
		 *  container programatically and visibility in a debugger.
		 */
		Container* mContainer = nullptr;

		inline void initDebug()
		{
			mContainer = container();
		}
		#else
		inline void initDebug()
		{
		}
		#endif


	public:
		typedef InPlace<base_t, maxSize, localOnly> _My_Type;

		/**
		 * @brief
		 *  Makes an InPlace<base_t> object containing a <I>sub_t</I> constructed with
		 *  <I>arguments</I>.
		 */
		template<typename sub_t, typename... args_t>
		static _My_Type make(args_t ...arguments)
		{
			static_assert(std::is_base_of_v<base_t, sub_t>);

			_My_Type ret;
			ret.setValue<sub_t>(std::forward<args_t>(arguments)...);

			return ret;
		}

		/**
		 * @brief
		 *  Constructs an empty object.
		 */
		InPlace()
		{
			initDebug();
			new (container())NullContainer();
		}

		/**
		 * @brief
		 *  Copy constructor.  This will throw an excpetion if the object contained in
		 *  <I>other</I> is not copy constructable.
		 */
		InPlace(const _My_Type& other)
		{
			initDebug();
			other.container()->copy(container());
		}

		/**
		 * @brief
		 *  Move constructor.  This will throw an excpetion if the object contained in
		 *  <I>other</I> is not move constructable.
		 */
		InPlace(_My_Type&& other)
		{
			initDebug();
			other.container()->move(container());
		}

		/**
		 * @brief
		 *  Destructor also destructs the contained object.
		 */
		~InPlace()
		{
			std::destroy_at<Container>(container());
		}

		/**
		 * @brief
		 *  Sets the contained value.
		 */
		template<typename sub_t, typename... args_t>
		void setValue(args_t ...arguments)
		{

			if constexpr (AlignedBlockSize_v<sub_t> <= maxSize)
			{
				std::destroy_at<Container>(container());
				new(mContainerMemory) LocalContainer<sub_t>(std::forward<args_t>(arguments)...);
			}
			else if constexpr (!localOnly)
			{
				std::destroy_at<Container>(container());
				new(mContainerMemory) RemoteContainer<sub_t>(true, std::forward<args_t>(arguments)...);
			}
			else
			{
				throw std::bad_alloc();
			}
		}

		/**
		 * @brief
		 *  Empties the container, destructing any contained object.
		 */
		void clear()
		{
			if (container()->objPtr != nullptr)
			{
				std::destroy_at<Container>(container());
				new(mContainerMemory) NullContainer();
			}
		}

		/**
		 * @brief
		 *  Gets a pointer to the contained object. 
		 */
		base_t* get()
		{
			return container()->objPtr;
		}
		

		/**
		 * @brief
		 *  Gets a pointer to the contained object. 
		 */
		const base_t* get() const
		{
			return container()->objPtr;
		}

		/**
		 * @brief
		 *  Pointer based dereferencing of stored object. 
		 */
		base_t* operator->()
		{
			return container()->objPtr;
		}

		/**
		 * @brief
		 *  Pointer based dereferencing of stored object.
		 */
		const base_t* operator->() const
		{
			return container()->objPtr;
		}

		/**
		 * @brief
		 *  Dereferencing of stored object. 
		 */
		base_t& operator*()
		{
			return *container()->objPtr;
		}

		/**
		 * @brief
		 *  Dereferencing of stored object. 
		 */
		const base_t& operator*() const
		{
			return *container()->objPtr;
		}

		/**
		 * @brief
		 *  Assignment operator.  This will throw an excpetion if the object contained in
		 *  <I>other</I> is not copy constructable.
		 */
		InPlace& operator=(const _My_Type& other)
		{
			std::destroy_at<Container>(container());
			other.container()->copy(mContainerMemory);

			return *this;
		}

		/**
		 * @brief
		 *  Move assignment operator.  This will throw an excpetion if the object contained in
		 *  <I>other</I> is not move constructable.
		 */
		InPlace& operator=(_My_Type&& other)
		{
			std::destroy_at<Container>(container());
			other.container()->move(mContainerMemory);

			return *this;
		}

		/**
		 * @brief
		 *  Gets the type_index of the constained object, or returns the type_index of void if
		 *  empty.
		 */
		std::type_index typeIndex() const
		{
			return container()->typeIndex();
		}

		/**
		 * @brief
		 *  Gets the type_info of the constained object, or returns the type_info of void if
		 *  empty.
		 */
		const std::type_info& typeInfo() const
		{
			return container()->typeInfo();
		}

		/**
		 * @brief
		 *  Attempts to dynamically cast the contained object to <I>sub_t</I>.
		 */
		template<typename sub_t>
		sub_t* cast()
		{
			return dynamic_cast<sub_t*>(container()->objPtr);
		}

		/**
		 * @brief
		 *  Attempts to dynamically cast the contained object to <I>sub_t</I>.
		 */
		template<typename sub_t>
		const sub_t* cast() const
		{
			return dynamic_cast<const sub_t*>(container()->objPtr);
		}

		/**
		 * @brief
		 *  Pointer conversion.
		 */
		operator base_t*()
		{
			container()->objPtr;
		}

		/**
		 * @brief
		 *  Pointer conversion.
		 */
		operator const base_t*() const
		{
			container()->objPtr;
		}

		/**
		 * @brief
		 *  True if this container is not empty.
		 */
		operator bool() const
		{
			return (nullptr == container()->objPtr);
		}
	};
}

#endif // _STD_EXT_IN_PLACE_H_