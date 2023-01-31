
# Concurency

The c9y library implements algorithms and strategies how to write concurrent
code. It builds on top of the C++ threading primitives and enhances them.

## Basic Classes

The  `thread_pool` implements a thread pool with a similar API as `jthread`, but
starts the given amount of threads.

The `task_pool` implements a task oriented thread pool. That is it provides the
means to schedule work at any given time after the creation of the task pool.

The `queue` class implements a thread safe queue with the ability to wait for
elements to be put into the queue.

### C++20 Forward Compatibility

Since not all compilers on all platforms have all the new threading primitives, 
c9y provides drop in replacements for `latch`, `jthread`, `stop_source`, 
`stop_token` and `stop_callback`. These classes live in the `c9y` namesapce and 
if available, the `std` counterpart is used.

## Parallel Algorithms

c9y implements a set of standard algorithms but as parallel versions. That is
the calling function will wait until the algorithm finishes to execute on
a thread pool.

The parallel function allows you to execute any number of taks in paralell,
waiting for all of them to compleate.

The following standard algorithms are implemented:

- `parallel_all_of`
- `parallel_any_of`
- `parallel_copy`
- `parallel_count`
- `parallel_count_if`
- `parallel_for_each`
- `parallel_generate`
- `parallel_none_of`
- `parallel_reduce`
- `parallel_transform`

The following is semantically the same, but `parallel_for_each` will run on a 
thread pool and thus complete quicker.

```cpp
auto values = std::vector<int>(10000);

std::for_each(begin(values), end(values), [] (auto& value) {
  value += 1;  
});

c9y::parallel_for_each(begin(values), end(values), [] (auto& value) {
  value += 1;  
});
```

Also a parallel `parallel_map_reduce` is provided to implement the map/reduce
algorithm.

## Async Functions

c9y provides the ability to triggert "fire and forget" functions. In contrast
to `std::async`, `c9y::async` has a version that does not return a future, thus
allowing the calling code to return before the async function finishes.

```cpp
int main() 
{
  auto app = MyApp{};

  c9y::async([&] () {
    auto latest_version = http::get("http://myservice.com/latest-version");
    app.set_latest_version(latest_version);
  });

  app.run();
}
```

A version with a future is also provided for completeness.

## Sync Functions

c9y provides a set of functions to "synchronize" into other threads. If a thread
calls `sync_point` regularly, it is possible to schedule a task in this thread 
via the `sync` function and will be executed in the `sync_point`.

```cpp
auto scene = std::make_shared<Scene>();

auto render_worker = std::jthread([] (std::stop_token st) {
  auto gc = create_graphics_constext();
  while (!st.stop_requested())
  {
    c9y::sync_point();
    scene.render(gc);
  }
});

// at some time later
auto mesh = load_mesh();
c9y::sync(render_worker.get_id(), [scene, mesh] () {
  scene->add_mesh(mesh);
});
```

Both a version with an explicit thread to be used and one using the "main thread".
The main thread can be registered with `set_main_thread`.

A special case is implemented with the `delay` function. This will schedule a task
on the same thread as the calling thread. This may be usefully to delay an action
to be preformed outside of a given callback. This allows for such things as
unregistration of event handlers from the handler or self delete in a way that
does not corrupt the call stack.

```cpp
auto key_down_signal = rsgi::singal<Key>{};

// some place in the code
auto esc_conn = rsig::connection{};
esc_conn = key_down_signal.connect([&esc_conn] (Key key) {
  if (key == Key::ESC) 
  {
    do_esc_things();
    c9y::delay([&esc_conn] () {
      // this would crash / deadlook otherwise
      key_down_signal.disconnect(esc_conn); 
    });
  }
});

// in the main function or such
while (running) 
{
  handle_input();
  c9y::sync_point();
}
```

All `sync` function take a `once_tag`. This allows for multiple `sync` calls 
to to be accumulated, preventing work to be redone multiple times.

```cpp
// use render worker from above

auto physics_sync_tag = c9y::once_tag{}; 

void update_pysics()
{
  auto new_state = update_dynamic_bodies();

  // this happens only once per frame
  c9y::sync(physics_sync_tag, render_worker.get_id(), [new_state] () {
    for (auto& body : new_state) 
    {
      auto& object = scene->get_object(body.id);
      object.set_transform(body.transform);
    }  
  });
}
```

A real world example of using `sync` can be seen in [spdr's Node class](https://github.com/rioki/spdr/blob/master/spdr/Node.h).

## Coroutines

c9y implements coroutines support. By using the namespace `co_async` or
`co_sync` you can subsequently use `co_await` and `co_return`.

```cpp
#include <c9y/coroutine.h>

std::future<int> compute_async() 
{
    using namespace c9y::co_async;
    int a = co_await c9y::async<int>([] { return 6; });
    int b = co_await c9y::async<int>([] { return 7; });
    co_return a * b;
}

TEST(coroutine, compute_async)
{
    EXPECT_EQ(6 * 7, compute_async().get());
}
```

If you are using `co_async` the coroutines will be executed with the help of
`async`. This means that different bits of the function will be executed on
different threads. On the upside this is that the code just works. The 
downside is, when interacting with other code, care needs to be taken to not
introduce a race condition.

```cpp
std::future<int> compute_sync() 
{
    using namespace c9y::co_sync;
    int a = co_await c9y::async<int>([] { return 6; });
    int b = co_await c9y::async<int>([] { return 7; });
    co_return a * b;
}

TEST(coroutine, compute_sync)
{
    using namespace std::chrono_literals;

    auto f = compute_sync();

    while (f.wait_for(0s) == std::future_status::timeout)
    {
        c9y::sync_point();
    }

    EXPECT_EQ(6 * 7, f.get());
}
```

If you are using `co_sync` the coroutines will be exectued with the help of 
`sync`. This means that the thread calling the coroutine needs to call 
`sync_point` at some point. If this is already the case, coroutines embed 
naturally into the code and behave in the expected way, that is they execute 
interleaved in the same thread. 

## License

The c9y library is distributed under the MIT license. See [License.txt](License.txt)
for details.

## Thanks

I would like to thank [G. Sliepen](https://codereview.stackexchange.com/users/129343/g-sliepen) for reviewing c9y's code.
