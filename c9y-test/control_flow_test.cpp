//
// c9y - concurrency
// Copyright(c) 2017-2019 Sean Farrell
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

#include <atomic>
#include <ctime>
#include <iostream>
#include <array>

#include <c9y/c9y.h>

#include <gtest/gtest.h>

TEST(control_flow, async)
{
    auto result = 0u;
    auto atid = std::thread::id{};
    auto stid = std::thread::id{};

    auto pool = c9y::task_pool{};

    c9y::async<unsigned int>(pool, [&]() -> unsigned int {
        atid = std::this_thread::get_id();
        return 5;
    }, [&](std::exception_ptr err, unsigned int r) {
        stid = std::this_thread::get_id();
        result = r;
    });

    pool.run();

    EXPECT_TRUE(std::this_thread::get_id() != atid);
    EXPECT_TRUE(std::this_thread::get_id() == stid);
    EXPECT_EQ(5, result);
}

TEST(control_flow, async_exception)
{
    auto err_msg = std::string{};
    auto pool = c9y::task_pool{};

    c9y::async<unsigned int>(pool, [&]() -> unsigned int {
        throw std::runtime_error("A Problem");
    }, [&](std::exception_ptr err, unsigned int r) {
        try
        {
            std::rethrow_exception(err);
        }
        catch (std::runtime_error& ex)
        {
            err_msg = ex.what();
        }
    });

    pool.run();

    EXPECT_EQ("A Problem", err_msg);
}

TEST(control_flow, async_void)
{
    auto atid = std::thread::id{};
    auto stid = std::thread::id{};
    auto pool = c9y::task_pool{};

    c9y::async(pool, [&]() {
        atid = std::this_thread::get_id();
    }, [&](std::exception_ptr err) {
        stid = std::this_thread::get_id();
    });

    pool.run();

    EXPECT_TRUE(std::this_thread::get_id() != atid);
    EXPECT_TRUE(std::this_thread::get_id() == stid);
}

TEST(control_flow, async_void_exception)
{
    auto err_msg = std::string{};
    auto pool = c9y::task_pool{};

    c9y::async(pool, [&]() {
        throw std::runtime_error("A Void Problem");
    }, [&](std::exception_ptr err) {
        try
        {
            std::rethrow_exception(err);
        }
        catch (std::runtime_error& ex)
        {
            err_msg = ex.what();
        }
    });

    pool.run();

    EXPECT_EQ("A Void Problem", err_msg);
}

TEST(control_flow, idle)
{
    unsigned int result = 0;
    auto stid = std::thread::id{};

    auto pool = c9y::task_pool{};

    auto handle = c9y::start_idle(pool, [&] () {
        stid = std::this_thread::get_id();
        result++;
        return (result < 5);
    });

    pool.run();
    EXPECT_TRUE(std::this_thread::get_id() == stid);
    EXPECT_EQ(5, result);
}

TEST(control_flow, timer)
{
    auto result = 0u;
    auto start = 0u;
    auto end = 0u;
    auto stid = std::thread::id{};

    auto pool = c9y::task_pool{};

    c9y::start_timer(pool, [&]() {
        stid = std::this_thread::get_id();
        result++;
        return (result < 5);
    }, 15);

    start = c9y::get_ms_time();
    pool.run();
    end = c9y::get_ms_time();

    unsigned int diff = end - start;

    EXPECT_EQ(5, result);
    EXPECT_TRUE(74 < diff);
    EXPECT_TRUE(std::this_thread::get_id() == stid);
}

TEST(control_flow, async_and_idle)
{
    auto done = false;
    auto pool = c9y::task_pool{};

    auto ih = c9y::start_idle(pool, [&] () {
        // something stupid
        return true;
    });

    c9y::async(pool, [&]() {
        // nothing
    }, [&](std::exception_ptr err) {
        c9y::stop_idle(ih);
        done = true;
    });

    pool.run();

    EXPECT_TRUE(done);
}

TEST(control_flow, async_and_timer)
{
    auto done = false;
    auto pool = c9y::task_pool{};

    auto th = c9y::start_timer(pool, [&] () {
        // something stupid
        return true;
    }, 5);

    c9y::async(pool, [&]() {
        // nothing
    }, [&](std::exception_ptr err) {
        c9y::stop_timer(th);
        done = true;
    });

    pool.run();

    EXPECT_TRUE(done);
}

TEST(control_flow, paralell)
{
    auto pool = c9y::task_pool{};

    auto count = std::atomic<unsigned int>{0};

    // limit scope to ensure container is destroyed before the task pool runs
    {
        auto coutner = std::array<std::function<void()>, 4>{
            [&]() {
                count += 1;
            },
                [&]() {
                count += 3;
            },
                [&]() {
                count += 3;
            },
                [&]() {
                count += 7;
            },
        };

        c9y::paralell(pool, coutner.begin(), coutner.end(), [&](std::exception_ptr err) {
            c9y::throw_on_error(err);
        });
    }

    pool.run();

    EXPECT_EQ(14, static_cast<unsigned int>(count));
}

TEST(control_flow, paralell_full_collection)
{
    auto pool = c9y::task_pool{};

    auto count = std::atomic<unsigned int>{0};

    c9y::paralell(pool, std::array<std::function<void()>, 4>{
        [&]() {
            count += 1;
        },
            [&]() {
            count += 3;
        },
            [&]() {
            count += 3;
        },
            [&]() {
            count += 7;
        },
    }, [&](std::exception_ptr err) {
            c9y::throw_on_error(err);
        });

    pool.run();

    EXPECT_EQ(14, static_cast<unsigned int>(count));
}

TEST(control_flow, paralell_throw_one)
{
    auto pool = c9y::task_pool{};

    auto count = std::atomic<unsigned int>{0};
    std::exception_ptr ex;

    c9y::paralell(pool, std::array<std::function<void()>, 4>{
        [&]() {
            count += 1;
        },
            [&]() {
            count += 3;
        },
            [&]() {
            throw std::runtime_error("No");
        },
            [&]() {
            count += 7;
        },
    }, [&](std::exception_ptr err) {
            ex = err;
        });

    pool.run();

    EXPECT_THROW(c9y::throw_on_error(ex), std::runtime_error);
}

TEST(control_flow, paralell_throw_all)
{
    auto pool = c9y::task_pool{};

    std::exception_ptr ex;

    // limit scope to ensure container is destroyed before the task pool runs
    {
        auto coutner = std::array<std::function<void()>, 4>{
            [&]() {
                throw std::out_of_range("No");
            },
                [&]() {
                throw std::invalid_argument("No");
            },
                [&]() {
                throw std::runtime_error("No");
            },
                [&]() {
                throw std::logic_error("No");
            },
        };

        c9y::paralell(pool, coutner.begin(), coutner.end(), [&](std::exception_ptr err) {
            ex = err;
        });
    }

    pool.run();

    EXPECT_THROW(c9y::throw_on_error(ex), std::exception);
}

TEST(control_flow, sequence)
{
    auto pool = c9y::task_pool{};

    auto count = std::atomic<unsigned int>{0};

    // limit scope to ensure container is destroyed before the task pool runs
    {
        auto coutner = std::array<std::function<void()>, 4>{
            [&]() {
                if (count == 0)
                {
                    count += 1;
                }
            },
                [&]() {
                if (count == 1)
                {
                    count += 3;
                }
            },
                [&]() {
                if (count == 4)
                {
                    count += 3;
                }
            },
                [&]() {
                if (count == 7)
                {
                    count += 7;
                }
            },
        };

        c9y::sequence(pool, coutner.begin(), coutner.end(), [&](std::exception_ptr err) {
            c9y::throw_on_error(err);
        });
    }

    pool.run();

    EXPECT_EQ(14, static_cast<unsigned int>(count));
}

TEST(control_flow, sequence_full_collection)
{
    auto pool = c9y::task_pool{};

    auto count = std::atomic<unsigned int>{0};

    c9y::sequence(pool, std::array<std::function<void()>, 4>{
        [&]() {
            if (count == 0)
            {
                count += 1;
            }
        },
            [&]() {
            if (count == 1)
            {
                count += 3;
            }
        },
            [&]() {
            if (count == 4)
            {
                count += 3;
            }
        },
            [&]() {
            if (count == 7)
            {
                count += 7;
            }
        },
    }, [&](std::exception_ptr err) {
            c9y::throw_on_error(err);
        });

    pool.run();

    EXPECT_EQ(14, static_cast<unsigned int>(count));
}

TEST(control_flow, sequence_throw_one)
{
    auto pool = c9y::task_pool{};

    auto count = std::atomic<unsigned int>{0};
    std::exception_ptr ex;

    c9y::sequence(pool, std::array<std::function<void()>, 4>{
        [&]() {
            if (count == 0)
            {
                count += 1;
            }
        },
            [&]() {
            if (count == 1)
            {
                count += 3;
            }
        },
            [&]() {
            throw std::runtime_error("NO");
        },
            [&]() {
            if (count == 7)
            {
                count += 7;
            }
        },
    }, [&](std::exception_ptr err) {
            ex = err;
        });

    pool.run();

    EXPECT_EQ(4, static_cast<unsigned int>(count));
    EXPECT_THROW(c9y::throw_on_error(ex), std::runtime_error);
}
