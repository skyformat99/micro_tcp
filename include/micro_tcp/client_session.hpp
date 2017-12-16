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

#ifndef MICRO_TCP_CLIENT_SESSION_HPP
#define MICRO_TCP_CLIENT_SESSION_HPP

#include <micro_tcp/session.hpp>
#include <micro_tcp/response_handler.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace micro_tcp
{
    /**
     * @brief
     */
    class client_session :
            public session
    {
    public:
        static constexpr auto default_timeout_ms = 10000;

        /**
         * @brief Non-copyable - delete copy constructor.
         */
        client_session(const client_session&) = delete;

        /**
         * @brief Non-copyable - delete assignment operator.
         */
        client_session& operator=(const client_session&) = delete;

        /**
         * @brief
         *
         * @param socket
         * @param context
         * @param response_handler
         */
        explicit client_session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context& context,
                                micro_tcp::response_handler& response_handler);

        /**
         * @brief
         */
        void start() override;

        /**
         * @brief
         *
         * @param message
         */
        void send(const micro_tcp::message& message);

    private:
        /**
         * @brief
         */
        void on_secure_handshake() override;

        /**
         * @brief
         */
        void on_write_header() override;

        /**
         * @brief
         */
        void on_write_content() override;

        /**
         * @brief
         */
        void on_read_header() override;

        /**
         * @brief
         */
        void on_read_content() override;

        /**
         * @brief
         */
        void on_shutdown_secure_stream() override;

        /**
         * @brief
         */
        void on_close_socket() override;

        /**
         * @brief
         */
        void do_timeout();

        /**
         * @brief
         */
        void set_timeout_expiry_time(unsigned long timeout_ms = default_timeout_ms);

    private:
        micro_tcp::response_handler& response_handler_;
        boost::asio::deadline_timer timeout_;
    };

    typedef std::shared_ptr<client_session> client_session_ptr;


}

#endif
