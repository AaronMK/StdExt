#ifndef _STD_EXT_IN_PLACE_H_
#define _STD_EXT_IN_PLACE_H_

#include "Number.h"
#include "Utility.h"
#include "Platform.h"
#include "Concepts.h"
#include "Callable.h"

#include "Memory/TaggedPtr.h"

#include <algorithm>
#include <typeindex>
#include <type_traits>



#ifdef _MSC_VER
#	pragma warning( push )
#	pragma warning( disable: 26495 )
#endif

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
	 *  The maximum size a properly aligned object can be for in-object storage.
	 *  This defaults to sizeof(base_t)
	 *
	 * @tparam localOnly
	 *  If true, attempting to set the value of this object to something too big to
	 *  fit in the local storage will raise a std::bad_alloc exception.  If false
	 *  oversized values will be stored on the heap, while local storage optimizations
	 *  will still be provided objects small enough to fit in local storage.
	 */
	template<Class base_t, size_t maxSize, bool localOnly = false>
	class InPlace final
	{
	private:

		using _My_Type = InPlace<base_t, maxSize, localOnly>;

		static constexpr size_t _local_align = alignof(size_t);
		static constexpr size_t _local_size  = std::max(sizeof(size_t), maxSize);

		static constexpr bool _canAllocLocal(size_t _size, size_t _alignment = 1)
		{
			return can_place_aligned(_size, _alignment, _local_size, _local_align);
		}

		/**
		 * @brief
		 *  Base class for the internal container used by InPlace.  The subclass actually used is
		 *  determined by whether or not the contained item can fit in the local buffer.  The maxSize
		 *  template parameter of InPlace determines how big a %Container can be, and provides all
		 *  data needed for subclasses.
		 */
		class ITypeActions
		{
		public:
			/**
			 * @brief
			 *  Function for custom movement of the contents of one buffer to another.
			 */
			 virtual void move(_My_Type&& from, _My_Type& to) const
			 {
				 to.clear();
			 }

			/**
			 * @brief
			 *  Function for custom copying of the contents of one buffer to another.
			 */
			 virtual void copy(const _My_Type& from, _My_Type& to) const
			 {
				 to.clear();
			 }

			/**
			 * @brief
			 *  Destroys the contained item.
			 */
			virtual void destroy(void* obj) const
			{
			};

			/**
			 * @brief
			 *  Gets the std::type_index of the contained item.
			 */
			virtual std::type_index typeIndex() const
			{
				return std::type_index(typeid(void));
			}

			/**
			 * @brief
			 *  Gets the std::type_info of the contained item.
			 */
			virtual const std::type_info& typeInfo() const
			{
				return typeid(void);
			}

			/**
			 * @brief
			 *  True if the item can be moved.
			 */
			virtual bool canMove() const
			{
				return true;
			}

			/**
			* @brief
			*  True if the item can be copied.
			*/
			virtual bool canCopy() const
			{
				return true;
			}
		};

		template<typename T>
		class TypeActions : public ITypeActions
		{
			static constexpr bool is_local = _canAllocLocal(sizeof(T), alignof(T));
			static constexpr bool movable = std::is_move_constructible_v<T>;
			static constexpr bool copyable = std::is_copy_constructible_v<T>;

		public:
			virtual void move(_My_Type&& from, _My_Type& to) const override
			{
				if constexpr( !movable )
				{
					throw invalid_operation("Attempting to move an InPlace<T> which has unmovable contents.");
				}
				else if constexpr ( is_local )
				{
					T& from_ref = access_as<T&>(from._bufData());

					to.setValue<T>(std::move(from_ref));
					from.clear();
				}
				else
				{
					if ( !to._bufIsLocal() )
						free_aligned(to.mAlignmentData.ptr());

					to.mAlignmentData = from.mAlignmentData;
					from.mAlignmentData.pack(0, nullptr);

					to.mTypeActions.template set< TypeActions<T> >();
					from.mTypeActions.template set<ITypeActions>();
				}
			}

			virtual void copy(const _My_Type& from, _My_Type& to) const override
			{
				if constexpr( !copyable )
				{
					throw invalid_operation("Attempting to copy an InPlace<T> which has non-copyable contents.");
				}
				else
				{
					const T& from_ref = access_as<const T&>(from._bufData());
					to.setValue<T>(from_ref);
				}
			}

			virtual void destroy(void* obj) const override
			{
				destruct_at<T>( access_as<T*>(obj) );
			}

			virtual std::type_index typeIndex() const override
			{
				return std::type_index(typeid(T));
			}

			virtual const std::type_info& typeInfo() const override
			{
				return typeid(T);
			}

			virtual bool canMove() const override
			{
				return movable;
			}

			virtual bool canCopy() const override
			{
				return copyable;
			}
		};

		#if defined(STD_EXT_DEBUG)
		/**
		 * @brief
		 *  A pointer to the contained item for debugger visibility.  When null, this object
		 *  is empty.
		 */
		base_t* mContainedItem = nullptr;
		#endif

		VTable<ITypeActions> mTypeActions;

		/**
		 * @brief
		 *  Local storage for objects that fit within _local_size bytes at _local_align
		 *  alignment.
		 */
		alignas(_local_align) char mLocalBuffer[_local_size];

		/**
		 * @brief
		 *  The tag holds the alignment of the active allocation; the pointer addresses the
		 *  start of the contained object.
		 */
		TaggedPtr<uint8_t, void*> mAlignmentData;

		template<Integral T>
		static uint8_t _toU8(T val)
		{
			if constexpr ( Config::Debug )
				return Number::convert<uint8_t>(val);
			else
				return static_cast<uint8_t>(val);
		}

		const void* _bufData() const noexcept
		{
			return mAlignmentData.ptr();
		}

		void* _bufData() noexcept
		{
			return mAlignmentData.ptr();
		}

		bool _bufIsLocal() const noexcept
		{
			const char* ptr = (const char*)mAlignmentData.ptr();
			return ( nullptr == ptr || (ptr >= &mLocalBuffer[0] && ptr < &mLocalBuffer[_local_size]) );
		}

		void _bufClear() noexcept
		{
			if ( !_bufIsLocal() )
				free_aligned(mAlignmentData.ptr());

			mAlignmentData.pack(0, nullptr);
		}

		void* _bufResize(size_t _size, size_t _alignment)
		{
			if ( !_bufIsLocal() )
				free_aligned(mAlignmentData.ptr());

			void* data_start = &mLocalBuffer[0];
			size_t local_buffer_size = _local_size;
			void* local_aligned_ptr = std::align(_alignment, _size, data_start, local_buffer_size);

			if ( local_aligned_ptr )
			{
				mAlignmentData.pack(_toU8(_alignment), local_aligned_ptr);
				return local_aligned_ptr;
			}

			void* new_ptr = alloc_aligned(_size, _alignment);
			mAlignmentData.pack(_toU8(_alignment), new_ptr);
			return new_ptr;
		}

		template<typename T>
		struct insertable
		{
			static constexpr bool value =
				( !localOnly || _canAllocLocal(sizeof(T), alignof(T)) ) &&
				SubclassOf<T, base_t>;
		};

	public:

		/**
		 * Passes for type T if the type can be inserted into this inplace object.  It must
		 * be a subclass of base_t.  If the container is local only, it must be able to fit
		 * properly aligned in the local space.
		 */
		template<typename T>
		static constexpr bool insertable_v = insertable<T>::value;

		/**
		 * @brief
		 *  Constructs an empty object.
		 */
		InPlace()
		{
			mAlignmentData.pack(0, nullptr);
			mTypeActions.template set<ITypeActions>();
		}

		/**
		 * @brief
		 *  Copy constructor.  This will throw an exception if the object contained in
		 *  <I>other</I> is not copy constructable.
		 */
		InPlace(const _My_Type& other)
			: InPlace()
		{
			other.mTypeActions->copy(other, *this);

			#if defined(STD_EXT_DEBUG)
			mContainedItem = access_as<base_t*>(_bufData());
			#endif
		}

		/**
		 * @brief
		 *  Move constructor.  This will throw an exception if the object contained in
		 *  <I>other</I> is not move constructable.
		 */
		InPlace(_My_Type&& other)
			: InPlace()
		{
			auto other_actions = other.mTypeActions;
			other_actions->move(std::move(other), *this);

			#if defined(STD_EXT_DEBUG)
			mContainedItem = access_as<base_t*>(_bufData());
			#endif
		}

		/**
		 * @brief
		 *  Destructor also destructs the contained object.
		 */
		~InPlace()
		{
			mTypeActions->destroy( _bufData() );
			_bufClear();
		}

		/**
		 * @brief
		 *  Sets the contained value.
		 */
		template<typename sub_t, typename... args_t>
			requires insertable_v<sub_t>
		void setValue(args_t ...arguments)
		{
			mTypeActions->destroy( _bufData() );

			auto next_data = _bufResize(sizeof(sub_t), alignof(sub_t));
			new(next_data) sub_t(std::forward<args_t>(arguments)...);
			mTypeActions.template set< TypeActions<sub_t> >();

			#if defined(STD_EXT_DEBUG)
			mContainedItem = access_as<base_t*>(_bufData());
			#endif
		}

		/**
		 * @brief
		 *  Returns a true if the container is empty.
		 */
		bool isEmpty() const
		{
			return (get() == nullptr);
		}

		/**
		 * @brief
		 *  Empties the container, destructing any contained object.
		 */
		void clear()
		{
			mTypeActions->destroy(_bufData());

			mTypeActions.template set<ITypeActions>();
			_bufClear();

			#if defined(STD_EXT_DEBUG)
			mContainedItem = nullptr;
			#endif
		}

		/**
		 * @brief
		 *  Gets a pointer to the contained object.
		 */
		base_t* get()
		{
			return access_as<base_t*>(_bufData());
		}

		/**
		 * @brief
		 *  Gets a pointer to the contained object.
		 */
		const base_t* get() const
		{
			return access_as<const base_t*>(_bufData());
		}

		/**
		 * @brief
		 *  Pointer based dereferencing of stored object.
		 */
		base_t* operator->()
		{
			return get();
		}

		/**
		 * @brief
		 *  Pointer based dereferencing of stored object.
		 */
		const base_t* operator->() const
		{
			return get();
		}

		/**
		 * @brief
		 *  Dereferencing of stored object.
		 */
		base_t& operator*()
		{
			return access_as<base_t&>(_bufData());
		}

		/**
		 * @brief
		 *  Dereferencing of stored object.
		 */
		const base_t& operator*() const
		{
			return access_as<const base_t&>(_bufData());
		}

		/**
		 * @brief
		 *  Assignment operator.  This will throw an exception if the object contained in
		 *  <I>other</I> is not copy constructable.
		 */
		InPlace& operator=(const _My_Type& other)
		{
			other.mTypeActions->copy(other, *this);

			#if defined(STD_EXT_DEBUG)
			mContainedItem = access_as<base_t*>(_bufData());
			#endif

			return *this;
		}

		/**
		 * @brief
		 *  Move assignment operator.  This will throw an exception if the object contained in
		 *  <I>other</I> is not move constructable.
		 */
		InPlace& operator=(_My_Type&& other)
		{
			auto other_actions = other.mTypeActions;
			other_actions->move(std::move(other), *this);

			#if defined(STD_EXT_DEBUG)
			mContainedItem = access_as<base_t*>(_bufData());
			#endif

			return *this;
		}

		/**
		 * @brief
		 *  Gets the type_index of the contained object, or returns the type_index of void if
		 *  empty.
		 */
		std::type_index typeIndex() const
		{
			return mTypeActions->typeIndex();
		}

		/**
		 * @brief
		 *  Gets the type_info of the contained object, or returns the type_info of void if
		 *  empty.
		 */
		const std::type_info& typeInfo() const
		{
			return mTypeActions->typeInfo();
		}

		/**
		 * @brief
		 *  Attempts to dynamically cast the contained object to <I>sub_t</I>.
		 */
		template<typename sub_t>
		sub_t* cast()
		{
			return dynamic_cast<sub_t*>(get());
		}

		/**
		 * @brief
		 *  Attempts to dynamically cast the contained object to <I>sub_t</I>.
		 */
		template<typename sub_t>
		const sub_t* cast() const
		{
			return dynamic_cast<const sub_t*>(get());
		}

		/**
		 * @brief
		 *  Pointer conversion.
		 */
		operator base_t*()
		{
			return *get();
		}

		/**
		 * @brief
		 *  Pointer conversion.
		 */
		operator const base_t*() const
		{
			return *get();
		}

		/**
		 * @brief
		 *  True if this container is not empty.
		 */
		operator bool() const
		{
			return (nullptr != get());
		}

		/**
		 * @brief
		 *  True if the object in the container can be moved.  If this is false,
		 *  any attempts to move this %InPlace object will throw an exception.
		 */
		bool canMove() const
		{
			return mTypeActions->canMove();
		}

		/**
		 * @brief
		 *  True if the object in the container can be copied.  If this is false,
		 *  any attempts to copy this %InPlace object will throw an exception.
		 */
		bool canCopy() const
		{
			return mTypeActions->canCopy();
		}
	};

	template<typename T>
	static constexpr float InPlace_Overhead()
	{
		constexpr float ipSize = (float)sizeof(StdExt::InPlace<T, sizeof(T)>);
		constexpr float tSize = (float)sizeof(T);

		if constexpr ( Config::Debug )
			return (ipSize - tSize - (float)sizeof(void*)) / tSize;
		else
			return (ipSize - tSize) / tSize;
	}
}


#ifdef _MSC_VER
#	pragma warning( pop )
#endif

#endif // _STD_EXT_IN_PLACE_H_
