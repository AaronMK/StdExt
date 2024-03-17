#include <StdExt/Tasking/Task.h>

#include <StdExt/Collections/Vector.h>
#include <StdExt/Signals/CallableHandler.h>
#include <StdExt/Tasking/SyncInterface.h>

#include "internal/ThreadPool.h"

namespace StdExt::Tasking
{
	static constexpr size_t NO_INDEX = std::numeric_limits<size_t>::max();
	
	Task::Task()
		: mState(TaskState::Dormant),
		  mFinished(false)
	{
	}

	Task::~Task()
	{
	}

	void Task::start()
	{
		assert( TaskState::Dormant == mState || TaskState::Finished == mState);

		mFinished.setValue(false);
		ThreadPool::addTask(this);
	}

	size_t Task::waitForAny(std::initializer_list<Task*> task_list)
	{
		class IndexedEndHandler : Signals::Subscription<bool>
		{
		private:
			size_t     mMyIndex;
			size_t*    mOutIndex;
			SyncPoint* mSyncPoint;

		public:
			IndexedEndHandler(SyncPoint* syn_point, size_t my_index, size_t* out_index)
				: mSyncPoint(syn_point), mMyIndex(my_index), mOutIndex(out_index)
			{
			}

		protected:
			void onUpdated(bool new_value) final
			{
				if ( new_value )
				{
					mSyncPoint->trigger(
						[&]()
						{
							if ( NO_INDEX != *mOutIndex )
								*mOutIndex = mMyIndex;
						}
					);
				}
			}
		};

		SyncPoint wait_sync;
		Collections::Vector<IndexedEndHandler, 8> end_handlers;
		size_t    result_index = NO_INDEX;

		CallableSyncActions sync_actions(
			[&]()
			{
				size_t current_index = 0;
				end_handlers.reserve(task_list.size());
				
				for ( Task* item : task_list )
				{
					if ( item->mFinished.value() )
					{
						result_index = current_index;
						end_handlers.clear();

						return;
					}

					end_handlers.emplace_back(&wait_sync, current_index, &result_index);
					++current_index;
				}
			},
			[&]()
			{
				return (NO_INDEX != result_index);
			},
			[&](WaitState state)
			{
				end_handlers.clear();
			}
		);

		return result_index;
	}

	void Task::waitForAll(std::initializer_list<Task*> task_list)
	{
		SyncPoint wait_sync;
		size_t finish_count = 0;

		class CounterEndHandler : Signals::Subscription<bool>
		{
		private:
			size_t*    mCount;
			size_t     mTotalCount;
			SyncPoint* mSyncPoint;

		public:
			CounterEndHandler(SyncPoint* syn_point, size_t total_count, size_t* out_count)
				: mSyncPoint(syn_point), mTotalCount(total_count), mCount(out_count)
			{
			}

		protected:
			void onUpdated(bool done) final
			{
				mSyncPoint->trigger(
					[&]()
					{
						return ( done && (++(*mCount) == mTotalCount) );
					}
				);
			}
		};

		auto update_count = [&](bool done)
			{
				wait_sync.trigger(
					[&]()
					{
						return ( done && (++finish_count == task_list.size()) );
					}
				);
			};

		Collections::Vector<CounterEndHandler, 8> end_handlers;

		ThreadPool::TaskSync.trigger(
			[&]()
			{
				for ( Task* item : task_list )
				{
					if ( item->mFinished.value() )
						++finish_count;
					else
						end_handlers.emplace_back(&wait_sync, task_list.size(), &finish_count);
				}

				return false;
			}
		);

		auto wait_interface = mixSyncActions<AtomicTaskSync>(
			CallableSyncActions(
			[&]()
				{
					return (finish_count >= task_list.size());
				},
				[&](WaitState state)
				{
					end_handlers.clear();
				}
			)
		);

		wait_sync.wait(&wait_interface);
		wait_interface.clientWait();
	}

	WaitState Task::sync(SyncPoint& sync_point, const CallableArg<bool>& predicate)
	{
		auto sync_interface = mixSyncActions<ThreadPoolSync>(
			CallableSyncActions(
				[&]()
				{
					return predicate();
				}
			)
		);

		sync_point.wait(&sync_interface);
		sync_interface.clientWait();

		return sync_interface.waitState();
	}

	WaitState Task::sync(
		SyncPoint& sync_point, const CallableArg<bool>& predicate,
		const CallableArg<void, WaitState>& handler
	)
	{
		auto sync_interface = mixSyncActions<ThreadPoolSync>(
			CallableSyncActions(
				[&]()
				{
					return predicate();
				},
				[&](WaitState state)
				{
					handler(state);
				}
			)
		);

		sync_point.wait(&sync_interface);
		sync_interface.clientWait();

		return sync_interface.waitState();
	}
}