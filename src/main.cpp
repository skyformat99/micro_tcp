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
#include <micro_tcp/secure_data.hpp>
#include <micro_tcp/server.hpp>
#include <micro_tcp/client.hpp>
#include <boost/program_options.hpp>
#include <boost/property_tree/xml_parser.hpp>

int main(int argc, const char *argv[])
{
    /**
     * Add program options to the application. Start with ./app -c config.xml
     */
    std::string config_path;
    boost::program_options::options_description options("Options");
    options.add_options()
            ("help,h",
                 "Prints this help message")
            ("config,c", boost::program_options::value<std::string>(&config_path)->required(),
                 "Specifies a configuration file. See the provided example: \"config.xml\"");
    boost::program_options::variables_map vm;
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options), vm);
    if (vm.count("help"))
    {
        std::cout << "This prints a help message!" << std::endl;
    }
    boost::program_options::notify(vm);

    /**
     * Read configuration file.
     */
    boost::property_tree::ptree config;
    boost::property_tree::read_xml(config_path, config, boost::property_tree::xml_parser::no_comments);
    config = config.get_child("Config");

    /**
     * Initialise io service.
     */
    micro_tcp::io_manager io_manager;
    boost::asio::io_service& io_service = io_manager.get_io_service();

    /**
     * Initialise SSL/TLS context.
     */
    const auto address = config.get<std::string>("Server.listen_address");
    const auto port = config.get<unsigned short>("Server.listen_port");
    boost::asio::ssl::context server_context(io_service, boost::asio::ssl::context::tlsv12);
    micro_tcp::secure_data ssl_data(config.get<std::string>("Server.certificate_file"),              //Specifies certificate file
                                    config.get<std::string>("Server.certificate_chain_file"),        //Specifies certificate chain file
                                    config.get<std::string>("Server.diffie_hellman_parameter_file"), //Specifies Diffie-Hellman parameters file
                                    config.get<std::string>("Server.rsa_private_key_file"),          //Specifies RSA private key file
                                    config.get<std::string>("Server.rsa_private_key_password"));     //Specifies RSA private key password
    server_context.set_options(boost::asio::ssl::context::default_workarounds
                               | boost::asio::ssl::context::no_tlsv1
                               | boost::asio::ssl::context::no_tlsv1_1
                               | boost::asio::ssl::context::no_sslv2
                               | boost::asio::ssl::context::no_sslv3
                               | boost::asio::ssl::context::single_dh_use);
    if (!ssl_data.rsa_private_key_passphrase_.empty())
    {
        server_context.set_password_callback(boost::bind(ssl_data.rsa_private_key_passphrase_callback_));
    }
    server_context.use_certificate_chain_file(ssl_data.certificate_chain_file_);
    server_context.use_certificate_file(ssl_data.certificate_file_, boost::asio::ssl::context::pem);
    server_context.use_rsa_private_key_file(ssl_data.rsa_private_key_file_, boost::asio::ssl::context::pem);
    server_context.use_tmp_dh_file(ssl_data.temp_diffie_hellman_parameters_file_);

    /**
     * Init request handler and instantiate a server instance.
     */
    micro_tcp::request_handler request_handler;
    micro_tcp::server server(io_service, address, port, request_handler, server_context);

    /**
     * Initialise client SSL/TLS context.
     */
    boost::asio::ssl::context client_context(io_service, boost::asio::ssl::context::tlsv12);
    client_context.set_verify_mode(boost::asio::ssl::verify_none);

    /**
     * Init response handler and instantiate a client instance.
     */
    micro_tcp::response_handler response_handler;
    micro_tcp::client client(io_service, response_handler, client_context);

    /**
     * Start io_service work and start listening for incoming requests.
     */
    io_manager.start();

    /**
     * Simple "keep alive" console application.
     */
    while (true)
    {
        std::string input;
        std::getline(std::cin, input);
        if (input == "server_start")
        {
            server.start();
        }
        else if (input == "server_stop")
        {
            server.stop();
        }
        else if (input == "server_set_address")
        {
            std::cout << "Enter an address in dotted decimal (IPv4) or hexadecimal (IPv6):" << '\n';
            std::getline(std::cin, input);
            server.set_address(input);
        }
        else if (input == "server_set_port")
        {
            std::cout << "Enter a port (0-65535):" << '\n';
            std::getline(std::cin, input);
            server.set_port(boost::lexical_cast<unsigned short>(input));
        }
        else if (input == "client_connect")
        {
            client.connect(address, port);
        }
        else if (input == "client_disconnect")
        {
            client.disconnect();
        }
        else if (input == "client_send")
        {
            std::cout << "Enter a message:" << '\n';
            std::getline(std::cin, input);
            micro_tcp::message message(input);
            client.send(message);
        }
        else if (input == "client_send_file")
        {
            std::cout << "Enter a file path:" << '\n';
            std::getline(std::cin, input);
            client.send_file(input);
        }
        else if (input == "status")
        {
            std::cout << "##################################"
                      << "\n<|Server|>"
                      << "\n Address: " << server.get_address()
                      << "\n Port: " << server.get_port()
                      << "\n Listening: " << std::boolalpha << server.is_listening();
            std::cout << "\n<|Client|>"
                      << "\n Connected: " << std::boolalpha << client.is_connected();
            if(client.is_connected())
            {
                std::cout << "\n  *Host: " << "x.x.x.x"
                          << "\n  *Port: " << "00000";
//                          << "\n  *Cipher: " << client.get_cipher_active_session();
            }
            std::cout << "\n##################################"
                      << "\n";
        }
        else if (input == "quit")
        {
            break;
        }
        else
        {
            std::cout << "Invalid input! The following input is available:\n" << "- server_start\n"
                      << "- server_stop\n" << "- server_set_address\n" << "- server_set_port\n"
                      << "- client_connect\n" << "- client_disconnect\n" << "- client_send\n"
                      << "- client_send_file\n" << "- status\n" << "- quit" << std::endl;
        }
    }

    /**
     * Disconnect any active client sessions, stop listening and stop any io_service work.
     */
    client.disconnect();
    server.stop();
    io_manager.stop();

    return 0;
}