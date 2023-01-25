//
// c9y - concurrency
// Copyright 2017-2022 Sean Farrell
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <vector>
#include <thread>
#include <random>
#include <chrono>

#include <gtest/gtest.h>
#include <c9y/c9y.h>

enum class State
{
    INITIAL,
    THINKING,
    EATING
};

namespace atomic
{
    struct Fork
    {
        std::atomic<bool> available = true;
    };

    struct Philosopher
    {
        size_t              id        = 0;
        std::atomic<State>  state     = State::INITIAL;
        std::atomic<int>    satiation = 0;
        std::atomic<int>    health    = 100;

        Philosopher(size_t _id)
        : id(_id) {}

        Philosopher(const Philosopher& rhs)
        : id(rhs.id), state(rhs.state.load()), satiation(rhs.satiation.load()), health(rhs.health.load()) {}

        void think(unsigned int duration)
        {
            state = State::THINKING;
            health -= duration / 3;
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
        }

        void eat(std::vector<Fork>& forks, unsigned int duration)
        {
            const auto left_fork = id;
            const auto right_fork = (left_fork + 1) % forks.size();
            ASSERT_LT(left_fork, forks.size());
            ASSERT_LT(right_fork, forks.size());

            auto left_available = forks[left_fork].available.load();
            auto right_available = forks[right_fork].available.load();

            if (!left_available || !right_available)
            {
                return;
            }

            if (forks[left_fork].available.compare_exchange_weak(left_available, false) == false)
            {
                // in the mean time someone else took it
                return;
            }

            if (forks[right_fork].available.compare_exchange_weak(right_available, false) == false)
            {
                // give the left one back
                forks[left_fork].available = true;
                return;
            }

            state = State::EATING;
            std::this_thread::sleep_for(std::chrono::milliseconds(duration));
            satiation += duration;

            forks[left_fork].available = true;
            forks[right_fork].available = true;
        }

    };
}

auto get_random_seed()
{
    static std::mutex m;
    static std::random_device rd;

    std::scoped_lock<std::mutex> ls(m);
    return rd();
}

TEST(philosophers, atomics)
{
    using namespace atomic;

    std::vector<Philosopher> philosophers;
    for (size_t i = 0u; i < std::thread::hardware_concurrency(); i++)
    {
        philosophers.emplace_back(i);
    }

    std::vector<Fork> forks(philosophers.size());

    std::atomic<size_t> last_id = 0;
    c9y::thread_pool pool([&] () {
        try
        {
            std::mt19937 gen(get_random_seed());
            std::uniform_int_distribution<> thinking_dist(1, 25);
            std::uniform_int_distribution<> eating_dist(25, 50);

            auto id = last_id++;
            ASSERT_LT(id, philosophers.size());

            while (philosophers[id].satiation < 100 && philosophers[id].health > 0 )
            {
                philosophers[id].think(thinking_dist(gen));
                philosophers[id].eat(forks, eating_dist(gen));
            }
        }
        catch (const std::exception& ex)
        {
            // No we dont actually expect it, but we want to see it.
            EXPECT_EQ("Exception", ex.what());
            ASSERT_TRUE(false);
        }
        catch (...)
        {
            ASSERT_TRUE(false);
        }
    }, philosophers.size());

    pool.join();

    for (const auto& philosopher : philosophers)
    {
        EXPECT_GT(philosopher.health, 0);
        EXPECT_GE(philosopher.satiation, 100);
    }
}

TEST(philosophers, async_and_sync)
{
    c9y::set_main_thread_id(std::this_thread::get_id());

    struct Philosopher
    {
        size_t           id;
        State            state     = State::INITIAL;
        std::atomic<int> satiation = 0;
        std::atomic<int> health    = 100;

        Philosopher(size_t _id)
        : id(_id) {}

        Philosopher(const Philosopher& rhs)
        : id(rhs.id), state(rhs.state), satiation(rhs.satiation.load()), health(rhs.health.load()) {}
    };
    std::vector<Philosopher> philosophers;
    for (size_t i = 0u; i < std::thread::hardware_concurrency(); i++)
    {
        philosophers.emplace_back(i);
    }

    struct Fork
    {
        bool available = true;
    };
    std::vector<Fork> forks(philosophers.size());

    std::mt19937 gen(get_random_seed());
    std::uniform_int_distribution<> thinking_dist(1, 25);
    std::uniform_int_distribution<> eating_dist(25, 50);

    auto think = [&] (Philosopher& phil) mutable
    {
        auto f = c9y::sync<std::chrono::milliseconds>([&] () mutable
        {
            auto ammount = thinking_dist(gen);
            phil.state = State::THINKING;
            phil.health -= ammount / 3;
            return std::chrono::milliseconds(ammount);
        });
        std::this_thread::sleep_for(f.get());
    };

    auto eat = [&] (Philosopher& phil) mutable
    {
        auto f = c9y::sync<std::chrono::milliseconds>([&] () mutable
        {
            auto& left_fork = forks[phil.id];
            auto& right_fork = forks[(phil.id + 1) % forks.size()];

            if (left_fork.available && right_fork.available)
            {
                auto ammount    = eating_dist(gen);
                phil.state      = State::EATING;
                phil.satiation += ammount;
                left_fork.available  = false;
                right_fork.available = false;
                return std::chrono::milliseconds(eating_dist(gen));
            }
            return std::chrono::milliseconds(0);
        });

        auto duration = f.get();
        std::this_thread::sleep_for(duration);

        if (duration != std::chrono::milliseconds(0))
        {
            c9y::sync([id = phil.id, &forks] () mutable
            {
                auto& left_fork  = forks[id];
                auto& right_fork = forks[(id + 1) % forks.size()];
                left_fork.available  = true;
                right_fork.available = true;
            });
        }
    };

    std::vector<std::future<bool>> results(philosophers.size());
    for (auto i = 0u; i < philosophers.size(); i++)
    {
        results[i] = c9y::async<bool>([=] () mutable
        {
            while (philosophers[i].satiation < 100)
            {
                if (philosophers[i].health < 0)
                {
                    return false;
                }

                think(philosophers[i]);
                eat(philosophers[i]);
            }
            return true;
        });
    }

    while (!c9y::all_ready(results))
    {
        c9y::sync_point();
    }

    c9y::wait_all(results);
}
