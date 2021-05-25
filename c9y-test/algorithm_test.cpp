//
// c9y - concurrency
// Copyright(c) 2017-2021 Sean Farrell
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
#include <array>
#include <map>
#include <string>

#include <c9y/c9y.h>
#include <gtest/gtest.h>

TEST(algorithm, paralell)
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

TEST(algorithm, paralell_full_collection)
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

TEST(algorithm, paralell_throw_one)
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

TEST(algorithm, paralell_throw_all)
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

TEST(algorithm, sequence)
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

TEST(algorithm, sequence_full_collection)
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

TEST(algorithm, sequence_throw_one)
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

TEST(algorithm, generate)
{
    auto result = std::vector<unsigned int>(1000);

    auto pool = c9y::task_pool{};

    c9y::generate(pool, begin(result), end(result), [] () {
        return std::rand();
    }, [&](std::exception_ptr err) {
        c9y::throw_on_error(err);
    });

    pool.run();

    EXPECT_TRUE(result[0] != result[1]);
}

TEST(algorithm, transform)
{
    auto source = std::vector<unsigned int>(1000);
    auto result = std::vector<unsigned int>(1000);

    auto idx = 0u;
    std::generate(begin(source), end(source), [&] () {
        return ++idx;
    });

    auto pool = c9y::task_pool{};

    c9y::transform(pool, begin(source), end(source), begin(result), [] (auto value) {
        return value + 1;
    }, [&](std::exception_ptr err) {
        c9y::throw_on_error(err);
    });

    pool.run();

    EXPECT_EQ(source[42] + 1, result[42]);
}


std::vector<std::string> tokenize(const std::string_view str, const std::string_view delimiters)
{
    std::vector<std::string> gibs;
    size_t start = 0;
    size_t end = 0;

    while ((start != std::string::npos) && (start < str.size()))
    {
        end = str.find_first_of(delimiters, start);

        std::string gib;
        if (end == std::string::npos)
        {
            gib = str.substr(start);
            start = std::string::npos;
        }
        else
        {
            gib = str.substr(start, end - start);
            start = end + 1;
        }
        gibs.push_back(gib);
    }

    return gibs;
}

const char* text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Vestibulum sit amet magna ante. Sed dapibus maximus nibh non pretium. Sed ullamcorper, metus et rutrum scelerisque, libero eros tincidunt dolor, eget lobortis est lorem vitae felis. Nam dictum risus gravida odio finibus, vel facilisis lacus suscipit. Sed orci augue, commodo vitae odio vitae, vestibulum sollicitudin enim. Nulla sit amet placerat quam. Donec et est tempor, fermentum sapien id, posuere felis. Proin quis consectetur nibh. Fusce faucibus feugiat risus et porta. Proin nec ipsum venenatis, aliquam nisi eget, sagittis orci. Suspendisse ex velit, convallis posuere ligula nec, laoreet consequat lorem.\n"
                   "Sed tellus arcu, pellentesque ac rutrum quis, vehicula sed leo. Donec tempor ipsum nisl, sed porta leo aliquet at. Donec ornare enim erat, non scelerisque lorem commodo quis. Sed molestie lacinia orci, nec suscipit odio tincidunt in. Proin placerat mi et velit finibus sodales. Integer vitae finibus metus, a consectetur quam. Maecenas fringilla dignissim purus, a pulvinar elit lobortis venenatis. Ut tristique, neque in rutrum pellentesque, justo nisi pharetra ligula, at efficitur ex justo ac erat. Nulla placerat lobortis purus, vel suscipit nulla fringilla at. Maecenas et consequat arcu. Phasellus consequat urna luctus mauris accumsan, non varius mauris scelerisque.\n"
                   "Aliquam erat volutpat. Pellentesque arcu lacus, pellentesque at nulla ut, fermentum hendrerit dolor. Cras commodo turpis in libero congue aliquet. Proin non mi cursus, interdum quam vitae, gravida dolor. Pellentesque tincidunt imperdiet consectetur. Phasellus mollis, ipsum vel accumsan consequat, mauris arcu fringilla mi, sed gravida odio leo at erat. Nunc volutpat vel metus in volutpat. Donec efficitur purus vitae neque venenatis, eget eleifend dolor vehicula. Nulla pellentesque viverra sem. Suspendisse maximus, augue ac lacinia blandit, quam tellus molestie augue, aliquam fermentum ex arcu sed justo. Aenean mollis quam quis aliquam efficitur.\n"
                   "Vestibulum eget massa eros. Vestibulum in convallis quam, non imperdiet mi. Nam mi tortor, consequat eu nisi ut, varius molestie erat. Duis sit amet pretium lorem. In vehicula a est non viverra. Morbi sodales auctor arcu, dignissim laoreet magna. Suspendisse non semper mauris. Ut blandit eleifend mauris, in lacinia nunc. Sed tempor neque et molestie ultricies. Aliquam vel lobortis metus. Morbi tempus sapien ac tellus ullamcorper, congue elementum urna maximus. Praesent in ornare massa. In maximus mattis ultrices. Sed eget tempor quam, eget eleifend orci. Etiam mattis dui eget nunc congue suscipit.\n"
                   "Integer sed libero faucibus, gravida tortor ac, molestie nisl. Ut iaculis dui eget ligula tincidunt fringilla. Pellentesque eleifend libero consequat orci tempus tristique. Aenean egestas et metus sit amet dapibus. Donec id tortor eget leo vestibulum rutrum. Praesent purus leo, viverra eu purus sit amet, aliquet maximus massa. Orci varius natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Quisque bibendum efficitur orci sed pellentesque. Aenean velit enim, vehicula vitae ultrices varius, fringilla non quam. Integer iaculis ex sit amet libero laoreet laoreet. Vestibulum scelerisque ante vitae tristique pharetra. Etiam at vehicula diam. Sed sit amet lorem arcu.\n";

TEST(algorithm, map_reduce)
{
    auto words = tokenize(text, " ,.\n");

    auto pool = c9y::task_pool{};

    auto result = std::map<std::string, unsigned int>{};

    c9y::map_reduce(pool, words, result, [&] (auto word) {
        return std::make_pair(word, 1);
    }, [&] (auto pair) {
        return std::make_pair(pair.first, pair.second.size());
    }, [&] (auto err) {
        c9y::throw_on_error(err);
    });

    pool.run();

    EXPECT_EQ(8, result["non"]);
}
