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

#include <micro_tcp/server_session.hpp>

namespace micro_tcp
{
    server_session::server_session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context& context,
                                   request_handler& request_handler) :
            session(std::move(socket), context),
            request_handler_(request_handler)
    {
        /*...*/
    }

    void server_session::start()
    {
        do_secure_handshake(boost::asio::ssl::stream_base::server);
    }

    void server_session::on_secure_handshake()
    {
        debug("SERVER | secure handshake OK");
        read_buffer_.prepare_header_buffer_read();
        do_read_header();
    }

    void server_session::on_read_header()
    {
        debug("SERVER | read request header OK");
        read_buffer_.prepare_content_buffer_read();
        do_read_content();
        /* Wait for any incoming message of size read_buffer_.header_buffer_.size() */
    }

    void server_session::on_read_content()
    {
        debug("SERVER | read request content OK");
        request_handler_.handle_request(read_buffer_, write_buffer_);
        write_buffer_.prepare_header_buffer_write();
        do_write_header();
    }

    void server_session::on_write_header()
    {
        debug("SERVER | write response header OK");
        write_buffer_.prepare_content_buffer_write();
        do_write_content();
    }

    void server_session::on_write_content()
    {
        debug("SERVER | write response content OK");
        read_buffer_.clear();
        write_buffer_.clear();
        read_buffer_.prepare_header_buffer_read();
        do_read_header();
    }

    void server_session::on_shutdown_secure_stream()
    {
        auto self(shared_from_this());
        boost::asio::async_write(secure_stream_, boost::asio::null_buffers(), io_strand_.wrap([this, self](
                boost::system::error_code /*ec*/, std::size_t /*bytes_transferred*/)
        {
            debug("SERVER | shutting down secure (SSL/TLS) protocol on stream OK");
            do_close_socket();
        }));
    }

    void server_session::on_close_socket()
    {
        debug("SERVER | socket close OK");
    }
}