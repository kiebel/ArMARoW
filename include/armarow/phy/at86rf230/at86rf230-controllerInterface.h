#pragma once

namespace armarow {
	namespace phy {
		/*! \brief  Interface between the microcontroller and
		 *          the radio controller.
		 *  \tparam ComInterface  class the interface is based on (e.g. SPI)
		 */
		template< class ComInterface, class Portmap, class spec_t>
		class ControllerInterface : public ComInterface {
			public:
				/*! \brief  Data Type for transceiver SRAM address*/
				typedef uint8_t ramaddr_t;
				/*! \brief  Data Type for transceiver register address*/
				typedef typename spec_t::registerDefault::address regaddr_t;
				/*! \brief  Data Type for transceiver register value*/
				typedef typename spec_t::registerMap regval_t;
				/*! \brief  data type for interface status*/
				typedef typename spec_t::SPI::status_t status_t;
			private:
				/*! \brief  internal status of the interface*/
				status_t status;

				void put(const uint8_t p) {ComInterface::put((const char)(p));}
				bool get(uint8_t &p) {return ComInterface::get((char &)(p));}
			public:
				ControllerInterface() {
					UseRegmap(rm, Portmap);
					rm.cs.ddr  = true;  // cs pin is an output pin
					rm.cs.port = true;
					SyncRegmap(rm);
				}
				/*! \brief  (Re)Initializes the interface.*/
				void init() {
					status.value = 0;   // resets internal status
				}

				/*! \brief  Invokes a command of the radio controller.
				 *  \param[in] pCommand command to be executed
				 *  \return Returns the interface state.
				 */
				status_t invoke(regaddr_t pCommand = spec_t::registerDefault::SNOP) {
					return status;
				}

				/*! \brief  Read from a radio controller register.
				 *  \param[in]  pAddress address of the source register
				 *  \param[out] pValue variable to write result to
				 */
				void readRegister(regaddr_t pAddress, regval_t &pValue) {
					UseRegmap(rm, Portmap);
					rm.cs.port = false;
					SyncRegmap(rm);
					this->put( spec_t::SPI::REGISTER_READ | pAddress );
					this->put( spec_t::SPI::REGISTER_READ | pAddress );
					this->get( pValue.value );
					rm.cs.port = true;
					SyncRegmap(rm);
				}
				/*! \brief  Writes to a radio controller register.
				 *  \param[in] pAddress address of the destination register
				 *  \param[in] pValue new value of the register
				 */
				void writeRegister(regaddr_t pAddress, regval_t pValue) {
					UseRegmap(rm, Portmap);
					rm.cs.port = false;
					SyncRegmap(rm);
					this->put( spec_t::SPI::REGISTER_WRITE | pAddress );
					this->put( pValue.value );
					rm.cs.port = true;
					SyncRegmap(rm);
				}

				/*! \brief  Read data from the RXFIFO.
				 *  \param[in]  pSize available size of the buffer
				 *  \param[out] pData pointer to the data buffer
				 *  \param[out] pLqi  pointer to the lqi value
				 *  \return Returns the number of bytes read from
				 *          the RXFIFO (including the LQI byte)
				 */
				uint8_t readRxFifo(uint8_t pSize, uint8_t *pData, uint8_t *pLqi) {
					uint8_t count = 0;

					UseRegmap(rm, Portmap);
					rm.cs.port = false;
					SyncRegmap(rm);
					this->put( spec_t::SPI::FRAMEBUFFER_READ );
					this->put( 0 );
					this->get( count );
					if ( count > pSize ) {
						rm.cs.port = true;
						SyncRegmap(rm);
						return 0;
					}
					pSize = count;
					while(count--) {
						this->put( 0 );
						this->get( *pData++ );
					}
					if ( pLqi ) {
						this->put( 0 );
						this->get( *pLqi );
					}
					rm.cs.port = true;
					SyncRegmap(rm);
					return pSize;
				}
				/*! \brief  Read data from RXFIFO and do a CRC16 check
				 *          (CRC value is suppressed).
				 *
				 *  While transfering the frame into the provided buffer
				 *  the CRC value is calculated. This value is then
				 *  compared against the last two byte which are not
				 *  copied into the buffer.
				 *
				 *  \param[in]  pSize available size of the buffer
				 *  \param[out] pData pointer to the data buffer
				 *  \param[out] pLqi  pointer to the variable storing the LQI
				 *  \param[out] pCrc  result of the CRC check
				 *  \return Returns the number of bytes read from the
				 *          RXFIFO (including the LQI byte)
				 */
				uint8_t readRxFifoCrc(uint8_t pSize, uint8_t *pData, uint8_t *pLqi, bool &pCrc) {
					uint8_t  count = 0;
					uint16_t crc16 = 0;

					UseRegmap(rm, Portmap);
					rm.cs.port = false;
					SyncRegmap(rm);
					this->put( spec_t::SPI::FRAMEBUFFER_READ );
					this->put( 0 );
					this->get( count );
					if ( count > pSize ) {
						rm.cs.port = true;
						SyncRegmap(rm);
						return 0;
					}
					pSize = count;
					while ( count-- ) {
						this->put( 0 );
						this->get( *pData );
						//TODO crc16 = _crc_ccitt_update(crc16, *pData++);
					}
					if ( pLqi ) {
						this->put( 0 );
						this->get( *pLqi );
					}
					pCrc = (crc16 == 0); //TODO check clausel
					rm.cs.port = true;
					SyncRegmap(rm);
					return pSize;
				}
				/*! \brief  Writes data into the TXFIFO.
				 *  \param[in] pSize size of the data buffer
				 *  \param[in] pData pointer to the buffer
				 */
				void writeTxFifo(uint8_t pSize, const uint8_t *pData) {
					if (pSize == 0) return;
					UseRegmap(rm, Portmap);
					rm.cs.port = false;
					SyncRegmap(rm);
					this->put( spec_t::SPI::FRAMEBUFFER_WRITE );
					this->put( pSize );
					while(pSize--) { this->put( *pData++ ); }
					rm.cs.port = true;
					SyncRegmap(rm);
				}
				/*! \brief  Writes data into the TXFIFO and calculate
				 *          CRC (if not supported by the radio
				 *          controller).
				 *  \param[in] pSize size of the data buffer
				 *  \param[in] pData pointer to the buffer
				 */
				void writeTxFifoCRC(uint8_t pSize, const uint8_t *pData) {
					//TODO write to TXFIFO with CRC
				}

				/*! \brief  Read from the SRAM.
				 *  \param[in]  pAddress address in the SRAM
				 *  \param[in]  pSize number of bytes to be read
				 *  \param[out] pData pointer to the buffer
				 */
				void readSRAM(ramaddr_t pAddress, uint8_t pSize, uint8_t *pData) {
					UseRegmap(rm, Portmap);
					rm.cs.port = false;
					SyncRegmap(rm);
					this->put( spec_t::SPI::SRAM_READ );
					this->put( pAddress );
					while( pSize-- ) {
						this->put( 0 );
						this->get( *pData++ );
					}
					rm.cs.port = true;
					SyncRegmap(rm);
				}
				/*! \brief  Write to the SRAM.
				 *  \param[in] pAddress address in the SRAM
				 *  \param[in] pSize number of bytes to write
				 *  \param[in] pData pointer to the buffer
				 */
				void writeSRAM(ramaddr_t pAddress, uint8_t pSize, uint8_t *pData) {
					if ( pSize == 0 ) return;
					UseRegmap(rm, Portmap);
					rm.cs.port = false;
					SyncRegmap(rm);
					this->put( spec_t::SPI::SRAM_WRITE );
					this->put( pAddress );
					while( pSize-- ) {
						this->put( *pData++ );
					}
					rm.cs.port = true;
					SyncRegmap(rm);
				}
		};
	}
}
