/**
 * @file
 *  Contains most of the classes used by the tasking module to interface with
 *  the underlying tasking system.  Most of these classes are not used directly,
 *  but are composed into SyncInterface implementations through utility functions,
 *  enabling some brevity in tasking code.
 */

#ifndef _STD_EXT_TASKING_SYNC_INTERFACE_H_
#define _STD_EXT_TASKING_SYNC_INTERFACE_H_

#include "../Callable.h"
#include "../CallableTraits.h"

#include <limits>
#include <optional>

namespace StdExt::Tasking
{
	enum class WaitState
	{
		/**
		 * @brief
		 *  Initial wait state before any interaction with with a SyncPoint.
		 */
		None,

		/**
		 * @brief
		 *  Item is still waiting for the precondition to be met, and it has not been cancelled
		 *  or timed-out, nor has the SyncPoint been destroyed.
		 */
		Waiting,

		/**
		 * @brief
		 *  Precondition has been met and atomicAction() has executed.
		 */
		PredicateSatisfied,

		/**
		 * @brief
		 *  A wait for the predicate condition timed out.
		 */
		Timeout,

		/**
		 * @brief
		 *  Item's precondition was not met before the SyncPoint was destroyed.
		 */
		Destroyed,

		/**
		 * @brief
		 *  Item's precondition was not met before being canceled.
		 */
		Canceled
	};

	/**
	 * @brief
	 *  Interface used by a SyncPoint to interact with client code.
	 * 
	 * @details
	 *  This interface brings the task management and program logic into a SyncPoint
	 *  for processing and callback interaction.  Implementations can be created
	 *  directly, but it is usually a tasking system that will determine the
	 *  implementations of suspend() and wake(), and more generic program
	 *  logic that will implement testPredicate() and atomicAction().
	 *
	 *  All protected member functions are called atomically with respect to other client
	 *  code interacting the SyncPoint.
	 *
	 *  There are utility templates and classes to compose the tasking and program
	 *  logic separately and combine them into a single %SyncInterface implementation
	 *  that can be passed into a SyncPoint for management and processing.
	 */
	class STD_EXT_EXPORT SyncInterface
	{
		friend class SyncPoint;
		friend class SyncAwaiter;

	public:
		static constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();

		SyncInterface();
		virtual ~SyncInterface();

		/**
		 * @brief
		 *  Read access to the wait state.  This should be used to check and react to the state after
		 *  passing this %SyncInterface to a SyncPoint::wait() command.
		 */
		WaitState waitState() const;

	protected:

		/**
		 * @brief
		 *  Called once when a wait request is made on a sync point.  The default implementation does nothing.
		 */
		virtual void initialize();

		/**
		 * @brief
		 *  Override runs a test of a predicate condition.
		 */
		virtual bool testPredicate() = 0;

		/**
		 * @brief
		 *  An action to take that needs to happen atomically with respect to other client code interacting
		 *  with a SyncPoint once result of the wait call can be determined.
		 * 
		 * @param state
		 *  If the pre-condition was satisfied, this will be WaitState::Complete.  Otherwise, the state
		 *  will be WaitState::Timeout, WaitState:Destroyed, or WaitState::Canceled.
		 */
		virtual void atomicAction(WaitState state) = 0;

		/**
		 * @brief
		 *  Called when the precondition (as determined by testPredicate()) is not met and a wait needs to
		 *  occur.  An implementation of this function should arrange for the calling context to be
		 *  suspended.  Any implementation should <b>NOT</b> block, or a deadlock could result.
		 */
		virtual void suspend() = 0;

		/**
		 * @brief
		 *  Called by in the context of the SyncPoint::trigger() call that creates the conditions to satisfy
		 *  testPredicate().  A call to this function should be safe immediately after a call to suspend().
		 */
		virtual void wake() = 0;

		/**
		 * @brief
		 *  Called by client code to block until other operations of the synchronization operation have completed.
		 */
		virtual void clientWait() = 0;

	private:
		size_t    wait_index;
		WaitState wait_state;
	};

	/**
	 * @brief
	 *  Encapsulates the operations of the SyncInterface that will typically be defined by client code
	 *  of the tasking system.
	 *
	 * @details
	 *  Utility classes and functions will combine %SyncActions implementations with SyncTasking implementations 
	 *  to create full SyncInterface types.  This allows easier code reuse without tightly coupling the the
	 *  program logic to a specific tasking system.
	 */
	class SyncActions
	{
	public:
		virtual void initialize();
		virtual bool testPredicate() = 0;
		virtual void atomicAction(WaitState state) = 0;
	};

	template<HasSignature<void> init_t, HasSignature<bool> predicate_t, HasSignature<void, WaitState> handler_t>
	class CallableSyncActions : public SyncActions
	{
	private:
		init_t      mInit;
		predicate_t mPredicate;
		handler_t   mHandler;

	public:
		CallableSyncActions(init_t&& init, predicate_t&& predicate, handler_t&& handler)
			: mInit( std::forward<init_t>(init) ),
			  mPredicate( std::forward<predicate_t>(predicate) ),
			  mHandler( std::forward<handler_t>(handler) )
		{
		}

		CallableSyncActions(predicate_t&& predicate, handler_t&& handler)
			requires DefaultConstructable<init_t>
			: mPredicate( std::forward<predicate_t>(predicate) ),
			  mHandler( std::forward<handler_t>(handler) )
		{
		}

		CallableSyncActions(predicate_t&& predicate)
			requires DefaultConstructable<init_t> && DefaultConstructable<handler_t>
			: mPredicate( std::forward<predicate_t>(predicate) )
		{
		}

		void initialize() final
		{
			mInit();
		}

		bool testPredicate() final
		{
			return mPredicate();
		}

		void atomicAction(WaitState state) final
		{
			mHandler(state);
		}
	};

	template<HasSignature<bool> predicate_t, HasSignature<void, WaitState> handler_t>
	CallableSyncActions(predicate_t&& predicate, handler_t&& handler) -> CallableSyncActions<NullCallable<>, predicate_t, handler_t>;

	template<HasSignature<bool> predicate_t>
	CallableSyncActions(predicate_t&& predicate) -> CallableSyncActions<NullCallable<>, predicate_t, NullCallable<WaitState>>;

	template<SubclassOf<SyncInterface> base_interface_t, SubclassOf<SyncActions> actions_t, typename... base_args_t>
	auto mixSyncActions(actions_t&& sync_actions, base_args_t... base_args)
	{
		class result_sync_t : public base_interface_t
		{
		private:
			actions_t mActions;

		public:
			result_sync_t(actions_t&& sync_actions, base_args_t... base_args)
				: base_interface_t( std::forward<base_args_t>(base_args)...),
				  mActions(std::forward<actions_t>(sync_actions))
			{
			}
			
			void initialize() final
			{
				mActions.initialize();
			}

			bool testPredicate() final
			{
				return mActions.testPredicate();
			}

			void atomicAction(WaitState state) final
			{
				mActions.atomicAction(state);
			}
		};

		return result_sync_t(std::forward<actions_t>(sync_actions), std::forward<base_args_t>(base_args)...);
	}

	/**
	 * @brief
	 *  Encapsulates the operations of the SyncInterface that will typically be defined by task management
	 *  code.
	 *
	 * @details
	 *  Utility classes and functions will combine SyncActions implementations with %SyncTasking implementations 
	 *  to create full SyncInterface types.  This allows easier code reuse without tightly coupling the the
	 *  program logic to a specific tasking system.
	 */
	class SyncTasking
	{
	public:
		virtual void suspend() = 0;
		virtual void wake() = 0;
		virtual void clientWait() = 0;
	};

	template<SubclassOf<SyncActions> actions_t, SubclassOf<SyncTasking> sync_tasking_t>
	class CombinedSyncInterface : public SyncInterface
	{
	private:
		actions_t      mActions;
		sync_tasking_t mSyncing;

	public:
		CombinedSyncInterface(actions_t&& actions, sync_tasking_t&& tasking_sync)
			: mActions( std::forward<actions_t>(actions) ),
			  mSyncing( std::forward<sync_tasking_t>(tasking_sync) )
		{
		}

		virtual ~CombinedSyncInterface()
		{
		}

		bool testPredicate() final
		{
			return mActions.testPredicate();
		}

		void atomicAction(WaitState state) final
		{
			mActions.atomicAction(state);
		}

		void suspend() final
		{
			mSyncing.suspend();
		}

		void wake() final
		{
			mSyncing.wake();
		}

		void clientWait() final
		{
			mSyncing.clientWait();
		}
	};

	template<
		HasSignature<void> init_t, HasSignature<bool> predicate_t, HasSignature<void, WaitState> action_t,
		HasSignature<void> suspend_t, HasSignature<void> wake_t, HasSignature<void> wait_t
	>
	class CallableSyncInterface : public SyncInterface
	{
	private:
		init_t      mInit;
		predicate_t mPredicate;
		action_t    mAction;
		suspend_t   mSuspend;
		wake_t      mWake;
		wait_t      mWait;

	public:
		CallableSyncInterface(
			init_t&& init_func, predicate_t&& predicate_func, action_t&& action_func,
			suspend_t&& suspend_func, wake_t&& wake_func, wait_t&& wait_func)
			: mInit( std::forward<init_t>(init_func) ),
			  mPredicate( std::forward<predicate_t>(predicate_func) ),
			  mAction( std::forward<action_t>(action_func) ),
			  mSuspend( std::forward<suspend_t>(suspend_func) ),
			  mWake( std::forward<wake_t>(wake_func) ),
			  mWait( std::forward<wait_t>(wait_func) )
		{
		}

		void initialize() final
		{
			mInit();
		}

		bool testPredicate() final
		{
			return mPredicate();
		}

		void atomicAction(WaitState state) final
		{
			mAction(state);
		}

		void suspend() final
		{
			mSuspend();
		}

		void wake() final
		{
			mWake();
		}

		void clientWait() final
		{
			mWait();
		}
	};

	/**
	 * @brief
	 */
	class STD_EXT_EXPORT AtomicTaskSync : public SyncInterface
	{
		std::optional<std::atomic_flag> mInternalFlag;
		std::atomic_flag* mFlag;

	public:
		AtomicTaskSync();
		AtomicTaskSync(std::atomic_flag* flag);

		virtual ~AtomicTaskSync();

		AtomicTaskSync(AtomicTaskSync&& other);
		AtomicTaskSync& operator=(AtomicTaskSync&& other);

		AtomicTaskSync(const AtomicTaskSync& other) = delete;
		AtomicTaskSync& operator=(const AtomicTaskSync& other) = delete;

		void suspend() override;
		void wake() override;

		void clientWait() override;
	};

	template<
		HasSignature<bool> predicate_t, HasSignature<void, WaitState> handler_t,
		SubclassOf<SyncTasking> tasking_t
	>
	auto makeSyncInterface(predicate_t&& predicate, handler_t&& handler, tasking_t&& tasking)
	{
		return CombinedSyncInterface(
			CallableSyncActions(
				std::forward<predicate_t>(predicate), std::forward<handler_t>(handler)
			),
			std::forward<tasking_t>(tasking)
		);
	}
}

#endif // !_STD_EXT_TASKING_SYNC_INTERFACE_H_