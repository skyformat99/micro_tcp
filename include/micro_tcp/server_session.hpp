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
#ifndef MICRO_TCP_SERVER_SESSION_HPP
#define MICRO_TCP_SERVER_SESSION_HPP

#include <micro_tcp/session.hpp>
#include <micro_tcp/request_handler.hpp>

namespace micro_tcp
{
    class server_session :
            public session
    {
    public:
        /**
         * @brief Non-copyable - delete copy constructor.
         */
        server_session(const server_session&) = delete;

        /**
         * @brief Non-copyable - delete assignment operator.
         */
        server_session& operator=(const server_session&) = delete;

        /**
         * @brief
         * @param socket
         * @param context
         * @param request_handler
         */
        explicit server_session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context& context,
                                request_handler& request_handler);

        /**
         * @brief
         */
        void start() override;

    private:
        /**
         * @brief
         */
        void on_secure_handshake() override;

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
        void on_write_header() override;

        /**
         * @brief
         */
        void on_write_content() override;

        /**
         * @brief
         */
        void on_shutdown_secure_stream() override;

        /**
         * @brief
         */
        void on_close_socket() override;

    private:
        micro_tcp::request_handler& request_handler_;
    };
}

#endif
