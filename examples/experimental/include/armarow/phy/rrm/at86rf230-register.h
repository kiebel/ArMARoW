/*******************************************************************************
 *
 * Copyright (c) 2010-2011 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 *               2010-2011 Christoph Steup <christoph.steup@st.ovgu.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/
#ifndef __ARMAROW_EXP_AT86RF230_SPEC_REGISTER_h__
#define __ARMAROW_EXP_AT86RF230_SPEC_REGISTER_h__

namespace armarow {
    namespace phy {
        namespace specification {
            namespace at86rf230 {
                struct Register {
                    enum  address { reg_trx_status = 0x01, reg_trx_state = 0x02, reg_man_id_0 = 0x1E };
                    union mapping {
                        struct {
                            uint8_t trx_status  : 5;
                            bool    reserved    : 1;
                            bool    cca_status  : 1;
                            bool    cca_done    : 1;
                        } reg_trx_status;
                        struct {
                            uint8_t trx_cmd     : 5;
                            uint8_t trac_status : 3;
                        } reg_trx_state;
                        uint8_t value;
                    };
                    enum  value   { reg_default_trx_status = 0x00, reg_default_trx_state = 0x00 };

                    template <typename Interface, address ADDR>
                    struct Access {
                        static void read(mapping& pValue) {
                            (Interface::getInstance()).enable();
                            (Interface::getInstance()).write( ADDR | 0x80 );
                            (Interface::getInstance()).read( pValue.value );
                            (Interface::getInstance()).disable();
                        }
                        static void write(const mapping pValue) {
                            (Interface::getInstance()).enable();
                            (Interface::getInstance()).write( ADDR | 0xC0 );
                            (Interface::getInstance()).write( pValue.value );
                            (Interface::getInstance()).disable();
                        }
                    };
                };
            }
        }
    }
}

#endif  //__ARMAROW_EXP_AT86RF230_SPEC_REGISTER_h__
