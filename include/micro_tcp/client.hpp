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

#ifndef MICRO_TCP_CLIENT_HPP
#define MICRO_TCP_CLIENT_HPP

#include <micro_tcp/client_session.hpp>
#include <micro_tcp/response_handler.hpp>

namespace micro_tcp
{
    class client
    {
    public:

        /**
         * @brief Non-copyable - delete copy constructor.
         */
        client(const client&) = delete;

        /**
         * @brief Non-copyable - delete assignment operator.
         */
        client& operator=(const client&) = delete;

        /**
         * @brief
         *
         * @param io_service
         * @param response_handler
         * @param context
         */
        explicit client(boost::asio::io_service& io_service, micro_tcp::response_handler& response_handler,
                        boost::asio::ssl::context& context);

        /**
         * @brief
         */
        ~client();

        /**
         * @brief
         *
         * @param remote_host
         * @param remote_port
         */
        void connect(const std::string& remote_host, unsigned short remote_port);

        /**
         * @brief
         */
        void disconnect();

        /**
         * @brief
         *
         * @param message
         * @return
         */
        bool send(const micro_tcp::message& message);

        /**
         * @brief
         *
         * @param file_path
         * @return
         */
        bool send_file(const std::string& file_path);

        /**
         * @brief
         * @return
         */
        bool is_connected() const;

    private:
        boost::asio::ssl::context& context_;
        boost::asio::io_service::strand io_strand_;
        boost::asio::ip::tcp::socket socket_;
        boost::asio::ip::tcp::resolver resolver_;
        micro_tcp::client_session_ptr active_session_;
        micro_tcp::response_handler& response_handler_;
    };
}

#endif
