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

#include <armarow/common/spi.h>
#include <avr-halib/share/singleton.h>
#include <avr-halib/regmaps/base/remoteRegMap.h>
#include <avr-halib/regmaps/regmaps.h>
#include <boost/mpl/list.hpp>

namespace armarow {
    namespace phy {
        namespace specification {
            namespace at86rf230 {
                using common::helpers::SpiImpl;
                using avr_halib::object::Singleton;
                using avr_halib::regmaps::base::RemoteRegMap;
                using namespace avr_halib::regmaps;
                using boost::mpl::list;

                struct TRX_StatusRegister
                {
                    enum RegisterParameters
                    {
                        address=0x01,
                        mode=base::read,
                        defaultValue=0x0
                    };

                    struct
                    {
                        uint8_t trx_status  : 5;
                        bool    reserved    : 1;
                        bool    cca_status  : 1;
                        bool    cca_done    : 1;
                    };
                };
                struct TRX_StateRegister
                {
                    enum RegisterParameters
                    {
                        address=0x02,
                        mode=base::both,
                        defaultValue=0x0
                    };

                    struct
                    {
                        uint8_t trx_cmd     : 5;
                        uint8_t trac_status : 3;
                    };
                };
                struct MAN_IDRegister
                {
                    enum RegisterParameters
                    {
                        address=0x01E,
                        mode=base::read,
                        defaultValue=0x0
                    };

                    uint8_t man_id;
                };

                typedef typename list<
                                        TRX_StatusRegister,
                                        TRX_StateRegister,
                                        MAN_IDRegister
                                    >::type RRMTestRegisterList;

                template <typename SpiImpl>
                struct InternalSpiImpl : public SpiImpl::noSingleton
                {

                    static const uint8_t readOpMask  = 0x80;
                    static const uint8_t writeOpMask = 0xC0;

                    typedef typename SpiImpl::noSingleton Base;

                    bool read(uint8_t address, uint8_t& data)
                    {
                        this->enable();
                        this->Base::write( address | readOpMask );
                        this->Base::read( data );
                        this->disable();
                        return true;
                    }
                    bool write(uint8_t address, const uint8_t data)
                    {
                        this->enable();
                        this->Base::write( address | writeOpMask );
                        this->Base::write( data );
                        this->disable();
                        return true;
                    }
                };

                template<typename SpiImpl>
                struct Spi : public Singleton<InternalSpiImpl<SpiImpl> >{};

                template<typename SpiImpl>
                struct RRMTestRegMap :
                    public base::RemoteRegMap<
                                            Spi<SpiImpl>,
                                            RRMTestRegisterList
                                            >{};
            }
        }
    }
}

#endif  //__ARMAROW_EXP_AT86RF230_SPEC_REGISTER_h__
