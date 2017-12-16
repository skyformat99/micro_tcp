# Micro TCP - server and client implementation with secure SSL/TLS support
Micro TCP is a simple TCP server- and client implementation with SSL/TLS support, built fully on top of Boost.Asio.

## Features
* Server (session) implementation where a session is defined as:
    1. secure handshake
    2. wait for incoming message/request
    3. read message contents
    4. handle request / create response
    5. send response
    6. go back to ii., wait for a new message.
* Client (session) implementation where a session is defined as:
    1. secure handshake
    2. send request
    3. wait for incoming message/response
    4. handle response
    5. keep session alive until manually closed or timeout (timeout in development).
* Secure communication over SSL/TLS (enabled by default with a strong cipher suite)
* Multithread support (enabled by default)
* Asynchronous implementation
* Basic file transfer and/or receive support
* Implement your custom response and request handler, override the examples (see next heading)
* Built fully on top of Boost.Asio
* Custom messaging protocol
* MIT License
* **Note:** this project is still under development and is provided as is, without any warranty (see License).

## Develop yourself (important, read this)
Core functionality has been implemented, yet personal features such as request and response handling, 
should be implemented by the user! Below I will attempt to guide you through some of files that should/could
be developed.

**Override and implement** the following classes to suit your own needs:
* **request_handler:** inherit this class and override the handle_request functionality.
    * _**Example implementation:**_ echo's back the received message.
    * **_See:_** _include/micro_tcp/request_handler.hpp_
* **response_handler:** inherit this class and override the handle_response functionality.
    * _**Example implementation:**_ if the response is larger than 10000 bytes it assumes we received a file, which will be 
    written to _test.out_. Otherwise it will be written to stdout.
    * **_See:_** _include/micro_tcp/response_handler.hpp_
    
## Supported platforms
* Linux / OSX
* Windows _(only tested with MinGW-W64)_

## Dependencies
* **C++14:** a C++14 compatible compiler is required for e.g. lambda auto and smart_ptr functionality.
* **Boost:** in particular, Boost.Asio is required.
* **OpenSSL:** Boost.Asio uses OpenSSL for basic SSL/TLS support and of course secure sockets.
* **CMake:** version 3.5. or later is required to be able to use the provided CMakeLists.txt.

## Build steps
To build the example, simply run CMake and build.

````
$ mkdir build && cd build
$ cmake ..
$ make
````

You can find the binary in the build/bin directory. 
Don't forget to add the program option --config path_to_config.xml when running.

## Usage provided example
The provided example (_src/main.cpp_), provides a basic console application. Before you start, edit the provided 
config file accordingly ()_example/config.xml_ ). Also, the provided secure files 
(_example/secure/*_), e.g. the certificate chain and key, are just examples. Generate your own if
you decide to go live with this:

````
Example for a self-signed certificate:
$ openssl req -new -newkey rsa:4096 -days 3000 -nodes -x509 -keyout private.key.pem -out certificate.cert.pem
We just act as the self-signed root CA, so just copy the certificate to a new certificate-chain file:
$ cp certificate.cert.pem certificate-chain.cert.pem
Encrypt your private key with a password (optional):
$ ssh-keygen -p -f private.key.pem
Generate a Diffie-Hellman parameters file (at least 2048 bit):
$ openssl dhparam -out dhparam.pem 4096
````

**Run** the application with the program option --config path_to_config.xml (you can fully remove the config in the _src/main.cpp_ if you like).

Below is a list of supported commands (through stdin).
* server_start | starts the server on the listening address and port as defined in the _example/config.xml_ (override in _src/main.cpp_).
* client_connect | attempts to start a client session with the server as defined in the _example/config.xml_ (override in _src/main.cpp_).
* client_send | prompts for a message and sends it to the selected server (by default the local server).
* client_send_file | prompts for a file path and sends it to the selected server (by default the local server).
* client_disconnect | closes the session for this client.
* server_stop | stops the server.
* server_set_address | prompts for a new server listening address.
* server_set_port | prompts for a new server listening port.
* status | gives some basic client/server information.
* quit | stops the application (return 0).

## Contact
Found a bug or have feedback? Please create an issue on GitHub. All feedback is welcome! Do 
note that this project is still under development and bugs may occur. I will try to respond/solve
bugs as fast as possible but please do not expect a fully operational help desk.

## License
Copyright  © 2017 Stefan Broekman. This project is released under the MIT license.

See https://stefanbroekman.nl

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.