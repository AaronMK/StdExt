#ifndef _STD_EXT_MEMORY_H_
#define _STD_EXT_MEMORY_H_

#include "StdExt.h"

#ifndef NOMINMAX
#	define NOMINMAX
#endif

#include "Exceptions.h"

#include <type_traits>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>
#include <memory>


namespace StdExt
{
	template<typename T, typename U>
	T* align(U*& ptr, std::size_t* space)
	{
		void* vPtr = ptr;

		if (std::align(alignof(T), sizeof(T), vPtr, *space))
		{
			ptr = reinterpret_cast<U*>(vPtr);
			return reinterpret_cast<T*>(vPtr);
		}

		return nullptr;
	}

	template<typename T, typename U>
	T* align(U*& ptr, std::size_t space)
	{
		return align<T>(ptr, &space);
	}

	template<typename... _Types>
	struct AlignedBlockSize;

	template<>
	struct AlignedBlockSize<>
	{
		static const size_t value = 1;
	};

	/**
	 * @brief
	 *  The value member of this struct contains the smallest block size
	 *  for which std::align would be guarenteed at succeeding for all
	 *  type parameters.
	 */
	template<typename _This, typename... _Rest>
	struct AlignedBlockSize<_This, _Rest...>
	{
		static constexpr size_t value = std::max(
			sizeof(_This) + alignof(_This) - 1,
			AlignedBlockSize<_Rest...>::value);
	};

	template<typename... _types>
	constexpr size_t AlignedBlockSize_v = AlignedBlockSize<_types...>::value;

	/**
	 * @brief
	 *  Container that provides in-place storage and type base access for types of
	 *  and sub-classes of base_t that, when properly aligned, occupy maxSize or
	 *  less memory.
	 */
	template<typename base_t, size_t maxSize, bool localOnly = false>
	class InPlace final
	{
	private:
		class Container
		{
		public:
			base_t* objPtr;
			char objData[maxSize];

			Container()
			{
				objPtr = nullptr;
			}

			virtual ~Container()
			{
			}

			virtual void move(void* destination) = 0;

			virtual void copy(void* destination) = 0;
		};

		template<typename sub_t>
		class RemoteContainer : public Container
		{
		private:
			
			template<bool canMove>
			class MoveFunc;

			template<>
			class MoveFunc<true>
			{
			public:
				static void move(RemoteContainer<sub_t>* container, void* destination)
				{
					RemoteContainer<sub_t>* otherContainer = new(destination) RemoteContainer<sub_t>(false);

					otherContainer->objPtr = container->objPtr;
					container->objPtr = nullptr;
				}
			}; 
			
			template<>
			class MoveFunc<false>
			{
			public:
				static void move(RemoteContainer<sub_t>* container, void* destination)
				{
					throw invalid_operation("Attempting move on a type that does not support it.");
				}
			};

			template<bool canCopy>
			class CopyFunc;

			template<>
			class CopyFunc<true>
			{
			public:
				static void copy(RemoteContainer<sub_t>* container, void* destination)
				{
					new(destination) RemoteContainer<sub_t>(true, *reinterpret_cast<sub_t*>(container->objPtr));
				}
			};
			
			template<>
			class CopyFunc<false>
			{
			public:
				static void copy(RemoteContainer<sub_t>* container, void* destination)
				{
					throw invalid_operation("Attempting copy on a type that does not support it.");
				}
			};

			static constexpr bool canCopy = std::is_copy_constructible<sub_t>::value;
			static constexpr bool canMove = std::is_move_constructible<sub_t>::value;

		public:
			template<typename... Args>
			RemoteContainer(bool initialize, Args&& ...arguments)
			{
				if (initialize)
					objPtr = new(_aligned_malloc(sizeof(sub_t), alignof(sub_t))) sub_t(arguments...);
				else
					objPtr = nullptr;
			}

			template<>
			RemoteContainer(bool initialize, sub_t&& other)
			{
				if (initialize)
					objPtr = new(_aligned_malloc(sizeof(sub_t), alignof(sub_t))) sub_t(std::move(other));
				else
					objPtr = nullptr;
			}

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
				MoveFunc<canMove>::move(this, destination);
			}

			virtual void copy(void* destination) override
			{
				CopyFunc<canCopy>::copy(this, destination);
			}
		};



		template<typename sub_t>
		class LocalContainer : public Container
		{
		private:

			template<bool canMove>
			class MoveFunc;

			template<>
			class MoveFunc<true>
			{
			public:
				static void move(LocalContainer<sub_t>* container, void* destination)
				{
					sub_t* ptr = reinterpret_cast<sub_t*>(container->objPtr);
					new(destination) LocalContainer<sub_t>(std::move(*ptr));
				}
			};

			template<>
			class MoveFunc<false>
			{
			public:
				static void move(LocalContainer<sub_t>* container, void* destination)
				{
					throw invalid_operation("Attempting move on a type that does not support it.");
				}
			};

			template<bool canCopy>
			class CopyFunc;

			template<>
			class CopyFunc<true>
			{
			public:
				static void copy(LocalContainer<sub_t>* container, void* destination)
				{
					sub_t* ptr = reinterpret_cast<sub_t*>(container->objPtr);
					new(destination) LocalContainer<sub_t>(*ptr);
				}
			};
			
			template<>
			class CopyFunc<false>
			{
			public:
				static void copy(LocalContainer<sub_t>* container, void* destination)
				{
					throw invalid_operation("Attempting copy on a type that does not support it.");
				}
			};

			static constexpr bool canCopy = std::is_copy_constructible<sub_t>::value;
			static constexpr bool canMove = std::is_move_constructible<sub_t>::value;

		public:
			template<typename... Args>
			LocalContainer(Args&& ...arguments)
			{
				objPtr = reinterpret_cast<sub_t*>(&objData[0]);
				objPtr = align<sub_t>(objPtr, sizeof(objData));

				if (nullptr != objPtr)
					new(objPtr) sub_t(arguments...);
			}

			template<>
			LocalContainer(sub_t&& other)
			{
				objPtr = reinterpret_cast<sub_t*>(&objData[0]);
				objPtr = align<sub_t>(objPtr, sizeof(objData));

				if (nullptr != objPtr)
					new(objPtr) sub_t(std::move(other));
			}

			virtual ~LocalContainer()
			{
				if (nullptr != objPtr)
					std::destroy_at<sub_t>(reinterpret_cast<sub_t*>(objPtr));
			}

			virtual void move(void* destination) override
			{
				MoveFunc<canMove>::move(this, destination);
			}

			virtual void copy(void* destination) override
			{
				CopyFunc<canCopy>::copy(this, destination);
			}
		};

		Container* mContainer = nullptr;
		char mContainerMemory[sizeof(Container)];

	public:
		typedef InPlace<base_t, maxSize, localOnly> _My_Type;

		template<typename sub_t, typename... args_t>
		static _My_Type make(args_t&& ...arguments)
		{
			static_assert(std::is_base_of_v<base_t, sub_t>);

			_My_Type ret;
			ret.setValue<sub_t>(arguments...);

			return ret;
		}

		InPlace()
		{
			mContainer = nullptr;
		}

		InPlace(const _My_Type& other)
		{
			if (nullptr != other.mContainer)
			{
				mContainer = reinterpret_cast<Container*>(&mContainerMemory[0]);
				other.mContainer->copy(mContainer);
			}
		}

		InPlace(_My_Type&& other)
		{
			if (nullptr != other.mContainer)
			{
				mContainer = reinterpret_cast<Container*>(&mContainerMemory[0]);
				other.mContainer->move(mContainer);
			}
		}

		~InPlace()
		{
			if (nullptr != mContainer)
				std::destroy_at<Container>(mContainer);
		}

		template<typename sub_t, typename... args_t>
		void setValue(args_t&& ...arguments)
		{
			clear();

			mContainer = new(mContainerMemory) LocalContainer<sub_t>(arguments...);

			if (nullptr == mContainer->objPtr)
			{
				std::destroy_at<Container>(mContainer);
				mContainer = nullptr;

				if (localOnly)
					throw std::bad_alloc();
				else
					mContainer = new(mContainerMemory) RemoteContainer<sub_t>(true, arguments...);
			}
		}

		void clear()
		{
			if (mContainer != nullptr)
			{
				std::destroy_at<Container>(mContainer);
				mContainer = nullptr;
			}
		}

		base_t* operator->()
		{
			return mContainer->objPtr;
		}

		const base_t* operator->() const
		{
			return mContainer->objPtr;
		}

		base_t& operator*()
		{
			return *mContainer->objPtr;
		}

		const base_t& operator*() const
		{
			return *mContainer->objPtr;
		}

		InPlace& operator=(const _My_Type& other)
		{
			if (nullptr != other.mContainer)
			{
				mContainer = reinterpret_cast<Container*>(&mContainerMemory[0]);
				other.mContainer->copy(mContainer);
			}

			return *this;
		}

		InPlace& operator=(_My_Type&& other)
		{
			clear();

			if (nullptr != other.mContainer)
			{
				mContainer = reinterpret_cast<Container*>(&mContainerMemory[0]);
				other.mContainer->move(mContainer);
			}

			return *this;
		}

		template<typename sub_t>
		sub_t* cast()
		{
			return (nullptr == mContainer) ? nullptr : dynamic_cast<sub_t*>(mContainer->objPtr);
		}
		
		template<typename sub_t>
		const sub_t* cast() const
		{
			return (nullptr == mContainer) ? nullptr : dynamic_cast<const sub_t*>(mContainer->objPtr);
		}

		operator base_t*()
		{
			return (nullptr == mContainer) ? nullptr : mContainer->objPtr;
		}

		operator const base_t*() const
		{
			return (nullptr == mContainer) ? nullptr : mContainer->objPtr;
		}

		operator bool() const
		{
			return (nullptr == mContainer);
		}
	};
}

#endif // _STD_EXT_MEMORY_H_