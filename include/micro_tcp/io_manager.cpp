///
//! @copyright Copyright (c) 2017 Stefan Broekman.
//! @license This file is released under the MIT license.
//! @see https://stefanbroekman.nl
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
///

#include <micro_tcp/io_manager.hpp>

namespace micro_tcp
{
    io_manager::io_manager() :
            active_(false),
            io_service_(),
            io_work_informer_(nullptr)
    {
        io_service_.stop();
        io_service_.reset();
    }

    io_manager::~io_manager()
    {
        stop();
    }

    boost::asio::io_service& io_manager::get_io_service()
    {
        return io_service_;
    }

    bool io_manager::is_active() const
    {
        return active_;
    }

    void io_manager::start(unsigned int num_threads)
    {
        if (!is_active())
        {
            io_thread_pool_.clear();
            io_work_informer_ = std::make_unique<boost::asio::io_service::work>(io_service_);
            io_thread_pool_.reserve(num_threads);
            for (unsigned int worker = 0; worker < num_threads; ++worker)
            {
                io_thread_pool_.emplace_back([this]()
                                             { io_service_.run(); });
            }
            active_ = true;
        }
    }

    void io_manager::stop()
    {
        if (is_active())
        {
            io_work_informer_.reset();
            io_service_.stop();
            for (auto& worker_thread : io_thread_pool_)
            {
                worker_thread.join();
            }
            io_service_.reset();
            active_ = false;
        }
    }
}