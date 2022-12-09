
# Concurency

The c9y library implements algorithms and strategies how to write concurrent
code. It builds on top of the C++ threading primitives and enhances them.

## Basic Classes

The thread_pool implements a thread pool with a similar API as std::thread, but
starts the given amount of threads.

The task_pool implements a task oriented thread pool. That is it provides the
means to schedule work at any given time after the creation of the task_pool.

The queue class implements a thread safe queue with the ability to wait for
elements to be put into the queue.

The latch class implements a one time multi event synchronisation mechanic.
It is basically a count down condition and only when all events happend the
waiting threads will be release.

## Parallel Algorithms

c9y implements a set of standard algorithms but as parallel versions. That is
the calling function will wait until the algorithm finishes to execute on
a thread pool.

The parallel function allows you to execute any number of taks in paralell,
waiting for all of them to compleate.

The following standard algorithms are implemented:

- parallel_all_of
- parallel_any_of
- parallel_copy
- parallel_count
- parallel_count_if
- parallel_for_each
- parallel_generate
- parallel_none_of
- parallel_reduce
- parallel_transform

Also a parallel parallel_map_reduce is provided to implement the map/reduce
algorithm.

## Async Functions

c9y provides the ability to triggert "fire and forget" functions. In contrast
to std::async, c9y::async has a version that does not return a future, thus
allowing the calling code to return before the async function finishes.

A version with a future is also provided for completeness.

## Sync Functions

c9y provides a set of functions to "synchronies" into other threads. If a thread
calls sync_point regularly, it is possible to schedule a task in this thread via
the sync function and will be executed in the sync_point.

Both a version with an explicit thread to be used and one using the "main thread".
The main thread can be registered with set_main_thread.

A special case is implemented with the delay function. This will schedule a task
on the same thread as the calling thread. This may be usefully to delay an action
to be preformed outside of a given callback. This allows for such things as
unregistration of event handlers from the handler or self delete in a way that
does not corrupt the call stack.

All sync function take a once_tag. This allows for multiple sync calls to to be
accumulated, preventing work to be redone multiple times.

## License

The c9y library is distributed under the MIT license. See [License.txt](License.txt)
for details.

## Thanks

I would like to thank [G. Sliepen](https://codereview.stackexchange.com/users/129343/g-sliepen) for reviewing c9y's code.
