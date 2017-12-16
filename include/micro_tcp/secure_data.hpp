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

#ifndef MICRO_TCP_SECURE_DATA_HPP
#define MICRO_TCP_SECURE_DATA_HPP

#include <string>
#include <functional>

namespace micro_tcp
{
    /**
     * @brief Struct which contains all certificate/key file paths and the private key password. Leave the password
     * empty to be prompted on first use.
     */
    struct secure_data
    {
        /**
         * @brief
         *
         * @param certificate_file
         * @param certificate_chain_file
         * @param temp_diffie_hellman_parameters_file
         * @param rsa_private_key_file
         * @param rsa_private_key_passphrase
         */
        secure_data(const std::string& certificate_file, const std::string& certificate_chain_file,
                    const std::string& temp_diffie_hellman_parameters_file, const std::string& rsa_private_key_file,
                    const std::string& rsa_private_key_passphrase = "") :
                certificate_file_(certificate_file),
                certificate_chain_file_(certificate_chain_file),
                temp_diffie_hellman_parameters_file_(temp_diffie_hellman_parameters_file),
                rsa_private_key_file_(rsa_private_key_file),
                rsa_private_key_passphrase_(rsa_private_key_passphrase),
                rsa_private_key_passphrase_callback_([this]()
                                                     { return rsa_private_key_passphrase_; })
        {
            /*...*/
        }

        /**
         * @brief
         */
        secure_data() = default;

        /**
         * @brief
         */
        ~secure_data() = default;

        std::string certificate_file_;
        std::string certificate_chain_file_;
        std::string temp_diffie_hellman_parameters_file_;
        std::string rsa_private_key_file_;
        std::string rsa_private_key_passphrase_;
        std::function<std::string()> rsa_private_key_passphrase_callback_;
    };
}

#endif