/**
 * @file
 *  Contains most of the classes used by the tasking module to interface with
 *  the underlying tasking system.  Most of these classes are not used directly,
 *  but are composed into SyncInterface implementations through utility functions,
 *  enabling some brevity in tasking code.
 */

#ifndef _STD_EXT_TASKING_SYNC_INTERFACE_H_
#define _STD_EXT_TASKING_SYNC_INTERFACE_H_

#include "../CallableTraits.h"

#include <limits>

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
	 *  implementations of markForSuspend() and wake(), and more generic program
	 *  logic that will implement testPredicate() and atomicAction().
	 * 
	 *  There are utility templates and classes to compose the tasking and program
	 *  logic separately and combine them into a single %SyncInterface implementation
	 *  that can be passed into a SyncPoint for management and processing.
	 */
	class SyncInterface
	{
		friend class SyncPoint;

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
		virtual void markForSuspend() = 0;

		/**
		 * @brief
		 *  Called by in the context of the SyncPoint::trigger() call that creates the conditions to satisfy
		 *  testPredicate().  A call to this function should be safe immediately after a call to markForSuspend().
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
		virtual bool testPredicate() = 0;
		virtual void atomicAction(WaitState state) = 0;
	};

	/**
	 * @brief
	 *  Encapsulates the operations of the SyncInterface that will typically be defined by task management
	 *  code.
	 *
	 * @details
	 *  Utility classes and functions will combine %SyncActions implementations with SyncTasking implementations 
	 *  to create full SyncInterface types.  This allows easier code reuse without tightly coupling the the
	 *  program logic to a specific tasking system.
	 */
	class SyncTasking
	{
	public:
		virtual void markForSuspend() = 0;
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

		void markForSuspend() final
		{
			mSyncing.markForSuspend();
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

	/**
	 * @brief
	 */
	class STD_EXT_EXPORT AtomicTaskSync : public SyncTasking
	{
		std::atomic_flag *mFlag;

	public:
		AtomicTaskSync(std::atomic_flag* flag);
		virtual ~AtomicTaskSync();

		AtomicTaskSync(AtomicTaskSync&& other);
		AtomicTaskSync& operator=(AtomicTaskSync&& other);

		AtomicTaskSync(const AtomicTaskSync& other) = delete;
		AtomicTaskSync& operator=(const AtomicTaskSync& other) = delete;

		void markForSuspend() override;
		void wake() override;

		void clientWait() override;
	};

	template<HasSignature<bool> predicate_t, HasSignature<void, WaitState> handler_t>
	class CallableSyncActions : public SyncActions
	{
	private:
		predicate_t mPredicate;
		handler_t   mHandler;

	public:
		CallableSyncActions(predicate_t predicate, handler_t handler)
			: mPredicate( std::forward<predicate_t>(predicate) ),
			  mHandler( std::forward<handler_t>(handler) )
		{
		}

		bool testPredicate() final
		{
			return mPredicate();
		}

		void atomicAction(WaitState state)
		{
			mHandler(state);
		}
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