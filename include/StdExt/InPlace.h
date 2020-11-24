#ifndef _STD_EXT_IN_PLACE_H_
#define _STD_EXT_IN_PLACE_H_

#include "Memory.h"
#include "Number.h"
#include "Utility.h"
#include "Platform.h"
#include "Concepts.h"
#include "FunctionPtr.h"

#include <algorithm>
#include <typeindex>
#include <type_traits>

namespace StdExt
{
	/**
	 * @brief
	 *  A buffer that will reserve _local_size_ space within its context to use
	 *  as storage when size and alignement parameters will allow, and use
	 *  a heap allocation otherwise.
	 */
	template<size_t local_size, size_t local_align = 1>
	class InPlaceBuffer final
	{
	public:
		static constexpr size_t min_align = alignof(size_t);
		static constexpr size_t max_align = 0x8000;

		static constexpr size_t real_local_align = std::max(alignof(size_t), local_align);
		static constexpr size_t real_local_size = std::max(sizeof(size_t), local_size);

	private:

		/**
		 * @brief
		 *  Store the data of the buffer when the size and alignment requirements allow.  Otherwise
		 *  the size of the buffer allocated on the heap is stored here.
		 */
		alignas(real_local_align) char mBuffer[real_local_size];

		/**
		 * @brief
		 *  The tag is the alignment of the data, and the pointer addresses the beginning
		 *  of the buffer.
		 */
		TaggedPtr<uint16_t, void*> mAlignmentData;

		template<size_t other_local_size, size_t other_local_align>
		void move_from(InPlaceBuffer<other_local_size, other_local_align>&& other)
		{
			if ( !isLocal() )
				freeAligned(mAlignmentData.ptr());

			if ( !canAllocLocal(other.size(), other.alignment()) && !other.isLocal() )
			{
				mAlignmentData = other.mAlignmentData;
				other.mAlignmentData.pack(0, nullptr);
			}
			else
			{
				resize(other.size(), other.alignment());
				memmove(data(), other.data(), other.size());
				other.resize(0);
			}
		}

		template<size_t other_local_size, size_t other_local_align>
		void copy_from(const InPlaceBuffer<other_local_size, other_local_align>&& other)
		{
			resize(other.size(), other.alignment());
			memcpy(data(), other.data(), other.size());
		}

		template<Integral T>
		static uint16_t to_u16(T val)
		{
			if constexpr ( Config::Debug )
			{
				return Number::convert<uint16_t>(val);
			}
			else
			{
				static_cast<uint16_t>(val);
			}
		}

	public:

		/**
		 * @brief
		 *  Returns true if the passed size and alignment requirements could be satisfied in the
		 *  local storage of this instance.
		 */
		static constexpr bool canAllocLocal(size_t _size, size_t _alignment = 1)
		{
			return canPlaceAligned(_size, _alignment, real_local_size, real_local_align);
		}
	
		InPlaceBuffer()
		{
			mAlignmentData.pack(0, nullptr);
		}

		/**
		 * @brief
		 *  Constructs a new %InPlaceBuffer with the same parameters as _other_,
		 *  moving the contents from _other_ into the newly constructed buffer as if
		 *  by memmove().
		 */
		InPlaceBuffer(InPlaceBuffer&& other)
			: InPlaceBuffer()
		{
			move_from(std::move(other));
		}

		InPlaceBuffer(size_t start_size, size_t block_alignment = 0)
			: InPlaceBuffer()
		{
			resize(start_size, block_alignment);
		}

		~InPlaceBuffer()
		{
			if ( !isLocal() )
				freeAligned(mAlignmentData.ptr());
		}

		InPlaceBuffer& operator=(InPlaceBuffer&& other)
		{
			move_from(std::move(other));
			return *this;
		}

		InPlaceBuffer(const InPlaceBuffer&) = delete;

		/**
		 * @brief
		 *  Returns true if the buffer is in memory local to the object.
		 */
		bool isLocal() const
		{
			char* ptr = (char*)mAlignmentData.ptr();
			return ( nullptr == ptr || (ptr >= &mBuffer[0] && ptr < &mBuffer[real_local_size]));
		}

		void* data() const
		{
			return mAlignmentData.ptr();
		}

		size_t size() const
		{	
			char* data_ptr = (char*)mAlignmentData.ptr();

			return (nullptr == data_ptr ) ?
				0 :
				isLocal() ?
					( &mBuffer[real_local_size] - data_ptr ) :
					access_as<const size_t&>(&mBuffer[0]);
		}

		size_t alignment() const
		{
			return mAlignmentData.tag();
		}

		void clear()
		{
			if ( !isLocal() )
				freeAligned(mAlignmentData.ptr());

			mAlignmentData.pack(0, nullptr);
		}

		/**
		 * @brief
		 *  Resizes the buffer without copying existing data.
		 */
		void* resize(size_t _size, size_t _alignment = 0)
		{
			if ( (0 != _alignment && !isPowerOf2(_alignment)) || _alignment > max_align )
				throw std::invalid_argument("'_alignment' must be a power of 2 and not greater than 0x8000.");

			size_t current_alignment =  alignment();
			bool current_local = isLocal();

			_alignment = ( 0 == _alignment ) ? std::max<size_t>(1, current_alignment) : _alignment;

			if ( _size == size() && _alignment == current_alignment )
				return mAlignmentData.ptr();

			void* data_start = &mBuffer[0];
			size_t local_buffer_size = real_local_size;
			void* local_aligned_ptr = std::align(_alignment, _size, data_start, local_buffer_size);

			if ( !current_local )
				freeAligned(mAlignmentData.ptr());

			if ( local_aligned_ptr )
			{
				mAlignmentData.pack(to_u16(_alignment), local_aligned_ptr);
				return local_aligned_ptr;
			}
			else
			{
				mAlignmentData.pack(to_u16(_alignment), allocAligned(_size, _alignment));
				access_as<size_t&>(&mBuffer[0]) = _size;
			}

			return mAlignmentData.ptr();
		}

		/**
		 * @brief
		 *  Resizes the allocation and copies data to a new location if needed.
		 */
		void* realloc(size_t _size)
		{
			if ( nullptr == mAlignmentData.ptr() && 0 != _size )
				return resize(_size);

			void* buffer_start = &mBuffer[0];
			size_t local_buffer_size = real_local_size;
			size_t alignment = mAlignmentData.tag();
			bool current_local = isLocal();

			void* local_aligned_ptr = std::align(alignment, _size, buffer_start, local_buffer_size);

			if ( local_aligned_ptr && current_local )
			{
				return mAlignmentData.ptr();
			}
			else if ( !current_local && local_aligned_ptr == nullptr )
			{
				void* next_data = reallocAligned(mAlignmentData.ptr(), _size, alignment);
				mAlignmentData.setPtr(next_data);

				return next_data;
			}
			else if ( current_local )
			{
				void* next_buffer = allocAligned(_size, alignment);
				memcpy(next_buffer, mAlignmentData.ptr(), std::min(_size, size()));

				mAlignmentData.setPtr(next_buffer);
				access_as<size_t&>(&mBuffer[0]) = _size;

				return next_buffer;
			}
			else
			{
				size_t old_size = access_as<const size_t&>(&mBuffer[0]);
				memcpy(local_aligned_ptr, mAlignmentData.ptr(), std::min(_size, old_size));

				freeAligned(mAlignmentData.ptr());
				mAlignmentData.setPtr(local_aligned_ptr);

				return local_aligned_ptr;
			}
		}
	};

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
	template<Class base_t, size_t maxSize, bool localOnly = false>
	class InPlace final
	{
	private:
		using _My_Type = InPlace<base_t, maxSize, localOnly>;
		using buffer_t = InPlaceBuffer<maxSize, alignof(base_t)>;

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
			 *  Function for custom movement of the contents of one buffer to another.
			 */
			virtual void copy(const _My_Type& from, _My_Type& to) const
			{
				to.clear();
			}

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
			static constexpr bool is_local = buffer_t::canAllocLocal(sizeof(T), alignof(T));
			static constexpr bool movable = std::is_move_constructible_v<T>;
			static constexpr bool copyable = std::is_copy_constructible_v<T>;

		public:
			virtual void move(_My_Type&& from, _My_Type& to) const override
			{
				if constexpr( !movable )
				{
					throw_exception<invalid_operation>("Attempting to move an InPlace<T> which has unmovable contents.", __FILE__, __LINE__);
				}
				else if constexpr ( is_local )
				{
					T& from_ref = access_as<T&>(from.mContainerMemory.data());

					to.setValue<T>(std::move(from_ref));
					from.clear();
				}
				else
				{
					to.mContainerMemory = std::move(from.mContainerMemory);
					to.mTypeActions.set< TypeActions<T> >();

					from.mTypeActions.set<ITypeActions>();
				}
			}

			virtual void copy(const _My_Type& from, _My_Type& to) const override
			{
				if constexpr( !copyable )
				{
					throw_exception<invalid_operation>("Attempting to copy an InPlace<T> which has non-copyable contents.", __FILE__, __LINE__);
				}
				else
				{
					const T& from_ref = access_as<const T&>(from.mContainerMemory.data());
					to.setValue<T>(from_ref);
				}
			}

			/**
			 * @brief
			 *  Gets the std::type_index of the contained item.
			 */
			virtual std::type_index typeIndex() const override
			{
				return std::type_index(typeid(T));
			}

			/**
			 * @brief
			 *  Gets the std::type_info of the contained item.
			 */
			virtual const std::type_info& typeInfo() const override
			{
				return typeid(T);
			}

			/**
			 * @brief
			 *  True if the item can be moved.
			 */
			virtual bool canMove() const override
			{
				return movable;
			}

			/**
			* @brief
			*  True if the item can be copied.
			*/
			virtual bool canCopy() const override
			{
				return copyable;
			}
		};

		#if defined(STD_EXT_DEBUG)
		/**
		 * @brief
		 *  A pointer to the container that wraps the value of this %InPlace object.  When null, this is
		 *  empty.  When non-null, it points to mContainerMemory, providing convenient access to the
		 *  container programatically and visibility in a debugger.
		 */
		base_t* mContainedItem = nullptr;
		#endif
		
		VTable<ITypeActions> mTypeActions;
		buffer_t mContainerMemory;

	public:
		
		template<typename T>
		struct insertable
		{
			static constexpr bool value = 
				( !localOnly || buffer_t::canAllocLocal(sizeof(T), alignof(T)) ) &&
				SubclassOf<T, base_t>; 
		};


		/**
		 * Passes for type T if the type can be insterted into this inplace object.  It must
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
			mTypeActions.set<ITypeActions>();
		}

		/**
		 * @brief
		 *  Copy constructor.  This will throw an excpetion if the object contained in
		 *  <I>other</I> is not copy constructable.
		 */
		InPlace(const _My_Type& other)
			: InPlace()
		{
			other.mTypeActions->copy(other, *this);
		}

		/**
		 * @brief
		 *  Move constructor.  This will throw an excpetion if the object contained in
		 *  <I>other</I> is not move constructable.
		 */
		InPlace(_My_Type&& other)
			: InPlace()
		{
			auto other_actions = other.mTypeActions;
			other_actions->move(std::move(other), *this);
		}

		/**
		 * @brief
		 *  Destructor also destructs the contained object.
		 */
		~InPlace()
		{
			destruct_at( access_as<base_t*>(mContainerMemory.data()) );
		}

		/**
		 * @brief
		 *  Sets the contained value.
		 */
		template<typename sub_t, typename... args_t>
			requires insertable_v<sub_t>
		void setValue(args_t ...arguments)
		{
			destruct_at( access_as<base_t*>(mContainerMemory.data()) );

			auto next_data = mContainerMemory.resize(sizeof(sub_t), alignof(sub_t));
			new(next_data) sub_t(std::forward<args_t>(arguments)...);
			mTypeActions.set< TypeActions<sub_t> >();
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
			destruct_at(get());
			
			mTypeActions.set<ITypeActions>();
			mContainerMemory.resize(0);
		}

		/**
		 * @brief
		 *  Gets a pointer to the contained object. 
		 */
		base_t* get()
		{
			return access_as<base_t*>(mContainerMemory.data());
		}

		/**
		 * @brief
		 *  Gets a pointer to the contained object. 
		 */
		const base_t* get() const
		{
			return access_as<const base_t*>(mContainerMemory.data());
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
			return access_as<base_t&>(mContainerMemory.data());
		}

		/**
		 * @brief
		 *  Dereferencing of stored object. 
		 */
		const base_t& operator*() const
		{
			return access_as<const base_t&>(mContainerMemory.data());
		}

		/**
		 * @brief
		 *  Assignment operator.  This will throw an excpetion if the object contained in
		 *  <I>other</I> is not copy constructable.
		 */
		InPlace& operator=(const _My_Type& other)
		{
			other.mTypeActions->copy(other, *this);
			return *this;
		}

		/**
		 * @brief
		 *  Move assignment operator.  This will throw an excpetion if the object contained in
		 *  <I>other</I> is not move constructable.
		 */
		InPlace& operator=(_My_Type&& other)
		{
			auto other_actions = other.mTypeActions;
			mTypeActions->move(std::move(other), *this);

			return *this;
		}

		/**
		 * @brief
		 *  Gets the type_index of the constained object, or returns the type_index of void if
		 *  empty.
		 */
		std::type_index typeIndex() const
		{
			return mTypeActions->typeIndex();
		}

		/**
		 * @brief
		 *  Gets the type_info of the constained object, or returns the type_info of void if
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
			return (nullptr == get());
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

#endif // _STD_EXT_IN_PLACE_H_