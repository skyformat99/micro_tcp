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

#ifndef MICRO_TCP_SESSION_HPP
#define MICRO_TCP_SESSION_HPP

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/asio/strand.hpp>
#include <micro_tcp/request_handler.hpp>
#include <micro_tcp/message.hpp>

namespace micro_tcp
{
    /**
     * @brief A session is an encapsulation of a request/response transaction sequence performed on a secure
     * stream. This class acts as a base class for client and server sessions.
     *
     * @li Server sequence:
     *      (1) secure handshake (as server) >
     *      (2) keep session alive (wait for incoming message) >
     *      (3) read message header >
     *      (4) read message contents >
     *      (5) handle request and define response >
     *      (6) write message header (response) >
     *      (7) write message contents (response) >
     *      (8) go back to (2)
     * @li Client sequence:
     *      (1) secure handshake (as client) >
     *      (2) keep session alive (wait for send) >
     *      (3) write message header (request) >
     *      (4) write message contents (request) >
     *      (5) read message header (response) >
     *      (6) read message contents (response) >
     *      (7) handle response >
     *      (8) go back to (2)
     *
     * @see server_session
     * @see client_session
     */
    class session :
            public std::enable_shared_from_this<session>
    {
    public:
        /**
         * @brief Non-copyable - delete copy constructor.
         */
        session(const session&) = delete;

        /**
         * @brief Non-copyable - delete assignment operator.
         */
        session& operator=(const session&) = delete;

        /**
         * @brief Default constructor. Wraps the raw socket in a secure (SSL/TLS) stream.
         *
         * @param socket A reference to a socket. Passed on using move semantics with class member session::socket_
         * as the final destination.
         * @param context A reference to a context object containing the secure (SSL/TLS) options, certificates,
         * verification mode and so on.
         */
        explicit session(boost::asio::ip::tcp::socket socket, boost::asio::ssl::context& context);

        /**
         * @brief Default destructor. Virtual since it is a base class for server_session and client_session.
         */
        virtual ~session();

        /**
         * @brief Start the session by attempting a secure handshake on the stream as either a server or client.
         *
         * @see server_session::start()
         * @see client_session::start()
         */
        virtual void start() = 0;

        /**
         * @brief Initiate a stop sequence. Cancel all outstanding asynchronous connect, send and receive operations
         * and attempt to securely shut down the secure stream. In the end the underlying transport is closed and the
         * most derived (server_session or client_session) session::on_stop() is called.
         *
         * @see session::shutdown_secure_stream()
         * @see session::close_socket()
         */
        void stop();

        /**
         * @brief Convenience function to check if the underlying transport/socket is open.
         *
         * @return True if the socket is open.
         */
        bool is_alive();

    protected:
        /**
         * @brief Attempt to asynchronously perform a secure (SSL/TLS) handshake as either a client or
         * server on the stream.
         *
         * @post If successful, the most derived (server_session or client_session) session::on_secure_handshake()
         * is called.
         * @post If failed, the session will be closed.
         *
         * @param type The type of handshaking to be performed.
         * Either as a server (boost::asio::ssl::stream_base::server) or client (boost::asio::ssl::stream_base::client).
         */
        void do_secure_handshake(boost::asio::ssl::stream_base::handshake_type type);

        /**
         * @brief Called on a successful secure handshake after session::do_secure_handshake(boost::asio::ssl::stream_base::handshake_type).
         *
         * @see server_session::on_secure_handshake()
         * @see client_session::on_secure_handshake()
         */
        virtual void on_secure_handshake() = 0;

        /**
         * @brief Start an asynchronous operation on the stream to read an X amount of bytes where X equals
         * read_buffer_.header_buffer_.size(). By default: message::default_header_length(). The buffer size MUST
         * be set in message::prepare_header_buffer_read().
         *
         * @post If successful, the header has been written from the stream into the read_buffer_.header_buffer_
         * and the most derived (server_session or client_session) session::on_read_header() is called.
         * @post If failed, the session will be closed.
         */
        void do_read_header();

        /**
         * @brief Called on a successful (header) read after session::do_read_header().
         *
         * @see server_session::on_read_header()
         * @see client_session::on_read_header()
         */
        virtual void on_read_header() = 0;

        /**
         * @brief Start an asynchronous operation on the stream to read an X amount of bytes where X equals
         * read_buffer_.content_buffer_.size(). The buffer size MUST be set in message::prepare_content_buffer_read().
         *
         * @post If successful, the content has been written from the stream into the read_buffer_.content_buffer_ and
         * the most derived (server_session or client_session) session::on_read_content() is called.
         * @post If failed, the session will be closed.
         */
        void do_read_content();

        /**
         * @brief Called on a successful (content) read after session::do_read_content().
         *
         * @see server_session::on_read_content()
         * @see client_session::on_read_content()
         */
        virtual void on_read_content() = 0;

        /**
         * @brief Start an asynchronous operation on the stream to write an X amount of bytes where X equals
         * write_buffer_.header_buffer_.size(). The buffer contents (and size) MUST be set in message::prepare_header_buffer_write().
         *
         * @post If successful, the buffer contents (header) have been written to the stream and the most derived
         * (server_session or client_session) session::on_write_header() is called.
         * @post If failed, the session will be closed.
         */
        void do_write_header();

        /**
         * @brief Called on a successful (header) write after session::do_write_header().
         *
         * @see server_session::on_write_content()
         * @see client_session::on_write_content()
         */
        virtual void on_write_header() = 0;

        /**
         * @brief Start an asynchronous operation on the stream to write an X amount of bytes where X equals
         * write_buffer_.content_buffer_.size(). The buffer contents (and size) MUST be set in message::prepare_content_buffer_write().
         *
         * @post If successful, the buffer contents (content) have been written to the stream and the most derived
         * (server_session or client_session) session::on_write_content() is called.
         * @post If failed, the session will be closed.
         */
        void do_write_content();

        /**
         * @brief Called on a successful (content) write after session::do_write_content().
         *
         * @see server_session::on_write_content()
         * @see client_session::on_write_content()
         */
        virtual void on_write_content() = 0;

        /**
         * @brief Asynchronously shut down the (SSL/TLS) protocol on the stream. Close the socket afterwards
         * by calling session::close_socket(). Quotes (1) and (2) in the remarks section describe the rationale
         * for this implementation.
         *
         * @remark (1):
         * Quote from Sansbury, T. (2017). Retrieved from: https://stackoverflow.com/a/32054476.
         * "Hence, if resources are bound to the lifetime of the socket or connection, then these resources will
         * remain alive waiting for the remote peer to take action or until the operation is cancelled locally.
         * However, waiting for a close_notify response is not required for a secure shutdown. If resources are
         * bound to the connection, and locally the connection is considered dead upon sending a shutdown, then
         * it may be worthwhile to not wait for the remote peer to take action."
         *
         * @remark (2):
         * Quote from Sansbury, T. (2017). Retrieved from: https://stackoverflow.com/a/32054476.
         * "When a client sends a close_notify message, the client guarantees that the client will not send
         * additional data across the secure connection. In essence, the async_write() is being used to detect
         * when the client has sent a close_notify, and within the completion handler, will close the underlying
         * transport, causing the async_shutdown() to complete with boost::asio::error::operation_aborted. As
         * noted in the quote (3), the async_write() operation is expected to fail.
         *
         * @remark (3):
         * Quote from Sansbury, T. (2015). Retrieved from: https://stackoverflow.com/a/25703699.
         * "In summary, Boost.Asio's SSL shutdown operations are a bit tricky. The inconsistencies between
         * the initiator and remote peer's error codes during proper shutdowns can make handling a bit awkward. As
         * a general rule, as long as the error code's category is not an SSL category, then the protocol was
         * securely shutdown."
         *
         * @remark (Behavior 1):
         * If the client_session (or remote host) calls boost:asio::async_shutdown(), the boost::asio::async_read() in
         * session::do_read_header() or session::do_read_content() is expected to complete with a boost::asio::error::eof
         * and calls session::stop()/boost::asio::async_shutdown() resulting in (Behavior 2).
         *
         * @remark (Behavior 2):
         * If both the client_session (or remote host) and server_session (or local host) call boost::asio::async_shutdown(),
         * the shutdown is expected to complete with a boost::asio::error::eof.
         *
         * @post todo: this
         */
        void do_shutdown_secure_stream();

        /**
         * @brief The server_session must implement a work-around for shutting down the secure protocol on the
         * stream without any errors in the secure SSL category. Please refer to session::do_shutdown_secure_stream()
         * for a more detailed explanation regarding the shutting down of the secure protocol on the stream.
         *
         * @see session::do_shutdown_secure_stream()
         * @see server_session::on_shutdown_secure_stream()
         * @see client_session::on_shutdown_secure_stream()
         */
        virtual void on_shutdown_secure_stream() = 0;

        /**
         * @brief Closes the underlying transport. The most derived (server_session or client_session)
         * session::on_close() is called.
         *
         * @post The socket is closed and the session goes out of scope destroying the last remaining shared_ptr
         * owning the session.
         */
        void do_close_socket();

        /**
         * @brief Called after the close sequence ending in session::close_socket().
         *
         * @see server_session::on_close()
         * @see client_session::on_close()
         */
        virtual void on_close_socket() = 0;

        /**
         * @brief Convenience function to get a reference to the underlying transport.
         *
         * @return A reference to the next layer in the stack of stream layers. In this case, the underlying
         * transport (boost::asio::ip::tcp::socket).
         */
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::next_layer_type& socket();

        /**
         * Just used for debugging.
         * Todo: delete this function.
         */
        void debug(const std::string& msg, const std::string& ec = "");

        micro_tcp::message read_buffer_; /*!< Buffer used for incoming messages. */
        micro_tcp::message write_buffer_; /*!< Buffer used for outgoing messages. */
        boost::asio::ip::tcp::socket socket_;
        boost::asio::ssl::stream<boost::asio::ip::tcp::socket&> secure_stream_;
        boost::asio::io_service::strand io_strand_;
    };

    typedef std::shared_ptr<session> session_ptr;
}

#endif
