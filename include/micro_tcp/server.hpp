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

#ifndef MICRO_TCP_SERVER_HPP
#define MICRO_TCP_SERVER_HPP

#include <micro_tcp/request_handler.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>

/**
 * todo: CRITICAL | when stopping the server with active sessions, the sessions will continue until the io_service
 * is stopped. Add a session_manager to the party.
 */


namespace micro_tcp
{
    class server
    {
    public:
        /**
         * @brief AES-256-GCM (Galois/Counter operation mode).
         */
        static constexpr auto default_cipher_suite_ = "EECDH+AESGCM:EDH+AESGCM:AES256+EECDH:AES256+EDH";

        /**
         * @brief Non-copyable - delete copy constructor.
         */
        server(const server&) = delete;

        /**
         * @brief Non-copyable - delete assignment operator.
         */
        server& operator=(const server&) = delete;

        /**
         * @brief
         *
         * @param io_service
         * @param address
         * @param port
         * @param request_handler
         * @param context
         * @param cipher_suite
         */
        server(boost::asio::io_service& io_service, const std::string& address, unsigned short port,
               micro_tcp::request_handler& request_handler, boost::asio::ssl::context& context,
               const std::string& cipher_suite = default_cipher_suite_);

        /**
         * @brief
         *
         * @param io_service
         * @param endpoint
         * @param request_handler
         * @param context
         * @param cipher_suite
         */
        server(boost::asio::io_service& io_service, const boost::asio::ip::tcp::endpoint& endpoint,
               micro_tcp::request_handler& request_handler, boost::asio::ssl::context& context,
               const std::string& cipher_suite = default_cipher_suite_);

        /**
         * @brief
         */
        ~server();

        /**
         * @brief
         */
        void start();

        /**
         * @brief
         */
        void stop();

        /**
         * @brief
         *
         * @return
         */
        bool is_listening();

        /**
         * @brief Creates an address from an IPv4 address string in a dotted decimal notation or from an IPv6 address
         * in hexadecimal notation and calls server::set_address(const boost::asio::ip::address& address).
         *
         * @param address An IPv4 address string in a dotted decimal notation or an IPv6 address in hexadecimal notation.
         * @return True if the set was successful and false if an error occurred
         *
         * @pre The server should be inactive as in "!acceptor_.is_open()".
         * @post Class member endpoint_ has been initialised with the value of parameter "address".
         */
        bool set_address(const std::string& address);

        /**
         * @brief
         *
         * @param address
         * @return
         */
        bool set_address(const boost::asio::ip::address& address);

        /**
         * @brief
         *
         * @return
         */
        std::string get_address() const;

        /**
         * @brief
         *
         * @param port
         * @return
         */
        bool set_port(unsigned short port);

        /**
         * @brief
         *
         * @return
         */
        unsigned short get_port() const;

        /**
         * @brief
         *
         * @return
         */
        std::string get_address_port() const;

        /**
         * @brief
         *
         * @param endpoint
         * @return
         */
        bool set_endpoint(const boost::asio::ip::tcp::endpoint& endpoint);

        /**
         * @brief
         *
         * @param request_handler
         * @return
         */
        bool set_request_handler(micro_tcp::request_handler& request_handler);

        /**
         * @brief
         *
         * @param port
         * @return
         */
        bool port_in_use(unsigned short port);

        /**
         * @brief
         *
         * @param only_ipv4
         * @return
         */
        std::vector<std::string> get_local_addresses(bool only_ipv4 = false);

    private:
        /**
         * Just used for debugging.
         * Todo: delete this function.
         */
        void debug(const std::string& msg, const std::string& ec = "");

        /**
         * @brief
         */
        void do_accept();

        /**
         * @brief
         */
        void start_listening();

        boost::asio::ssl::context& context_;
        boost::asio::io_service::strand io_strand_;
        boost::asio::ip::tcp::acceptor acceptor_;
        boost::asio::ip::tcp::socket socket_;
        boost::asio::ip::tcp::endpoint endpoint_;
        micro_tcp::request_handler& request_handler_;
    };
}

#endif //BROEKMAN_TCP_IP_SERVER_HPP
