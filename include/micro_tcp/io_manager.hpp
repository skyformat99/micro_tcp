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

#ifndef MICRO_TCP_IO_MANAGER_HPP
#define MICRO_TCP_IO_MANAGER_HPP

#include <boost/asio/io_service.hpp>
#include <thread>
#include <vector>

namespace micro_tcp
{
    /**
     * @brief The io_manager holds a io_service instance and the manages it. The io_service provides the core
     * I/O functionality for asynchronous I/O objects.
     */
    class io_manager
    {
    public:
        /**
         * @brief Non-copyable - delete copy constructor.
         */
        io_manager(const io_manager&) = delete;

        /**
         * @brief Non-copyable - delete assignment operator.
         */
        io_manager& operator=(const io_manager&) = delete;

        /**
         * @brief Default constructor.
         */
        io_manager();

        /**
         * @brief Default destructor. Stop the io_service/manager (if still active).
         */
        ~io_manager();

        /**
         * @brief Get a reference to the io_manager::io_service_ class member managed by this instance.
         *
         * @return A reference to the io_service_ class instance.
         */
        boost::asio::io_service& get_io_service();

        /**
         * @brief
         * @return
         */
        bool is_active() const;

        /**
         * @brief Start running the io_service on [num_threads] threads. By default the amount of threads
         * in your system - 1. If std::thread::hardware_concurrency() in the default value fails to retrieve
         * the amount of threads in the system, a default of 0 will be returned. By using the std::max construction,
         * the amount of threads will always be at least 1 if 0 was returned by std::thread::hardware_concurrency().
         *
         * The io_service is encapsulated within a work informer to ensure it always has at least one job and
         * won't stop/return until io_manager::stop() is manually called.
         *
         * @param num_threads The number of threads the io_service will do work on.
         */
        void start(unsigned int num_threads = std::max(std::thread::hardware_concurrency(), 2u) - 1u);

        /**
         * @brief Reset the boost::asio::io_service::work informer to allow for handlers to finish normally. Once
         * stopped, all threads are joined and io_service::reset() is called in preparation for a subsequent
         * io_service::run() invocation.
         *
         * Calling boost::asio::io_service::stop() will force any invocations of io_service::run() to return
         * "as soon as possible", abandoning all unfinished operations. Joining the threads before calling
         * io_service::reset() makes sure that all invocations have returned and that the io_service has been
         * properly stopped.
         */
        void stop();

    private:
        bool active_;
        boost::asio::io_service io_service_;
        std::unique_ptr<boost::asio::io_service::work> io_work_informer_;
        std::vector<std::thread> io_thread_pool_;
    };
}

#endif
