#ifndef I2C_INTERFACE_H
#define I2C_INTERFACE_H

#include "../../Service/STD_Types.h"
#include "i2c_registers.h"

/* ================================================================================
 *  I2C (TWI) DRIVER - PUBLIC INTERFACE (ATmega32)
 *  ------------------------------------------------------------------------------
 *  Two-Wire Interface driver. Provides both low-level primitives (START, STOP,
 *  write byte, read byte with/without ACK, read status) and high-level helpers
 *  for a master to write/read a buffer to/from an addressed slave.
 *
 *  Slave addresses passed to this API are 7-bit; the driver appends the R/W bit
 *  internally when forming SLA+W / SLA+R.
 * ============================================================================== */

/* ---------------- Bus Speed (SCL frequency) ---------------- */
/**
 * @brief Common SCL clock frequencies. The driver converts the requested speed
 *        and F_CPU into a TWBR value at init time.
 */
#define I2C_SCL_100KHZ        100000UL   /* Standard mode */
#define I2C_SCL_400KHZ        400000UL   /* Fast mode     */

/**
 * @brief CPU clock in Hz used to compute the bit-rate register. Adjust to match
 *        the target board.
 */
#ifndef I2C_F_CPU
#define I2C_F_CPU             8000000UL
#endif

/* ---------------- Acknowledge Selection ---------------- */
/**
 * @brief Whether the master returns ACK or NACK after receiving a data byte.
 *  - I2C_ACK  : keep reading (more bytes to come).
 *  - I2C_NACK : signal the last byte (master will STOP next).
 */
typedef enum
{
    I2C_NACK = 0,
    I2C_ACK  = 1
} I2C_AckType;

/* ---------------- Master Configuration Structure ---------------- */
/**
 * @brief Parameters consumed by I2C_InitMaster().
 * @var I2C_MasterConfigType::sclFrequency  Target SCL clock in Hz (e.g. I2C_SCL_100KHZ).
 */
typedef struct
{
    uint32_h sclFrequency;
} I2C_MasterConfigType;

/* ---------------- Slave Configuration Structure ---------------- */
/**
 * @brief Parameters consumed by I2C_InitSlave().
 * @var I2C_SlaveConfigType::ownAddress        This device's own 7-bit slave address.
 * @var I2C_SlaveConfigType::enableGeneralCall Non-zero to also answer general-call (0x00).
 */
typedef struct
{
    uint8_h ownAddress;
    uint8_h enableGeneralCall;
} I2C_SlaveConfigType;

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Initializes the TWI as a bus master: computes TWBR from sclFrequency and
 *         F_CPU (prescaler = 1), then enables the peripheral.
 * @param  addConfig  Pointer to a populated master configuration structure.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK on NULL pointer).
 */
STD_ReturnType I2C_InitMaster(const I2C_MasterConfigType *addConfig);

/**
 * @brief  Initializes the TWI as an addressable slave: loads TWAR with the own
 *         address (and optional general-call bit) and enables the peripheral
 *         with acknowledge generation on.
 * @param  addConfig  Pointer to a populated slave configuration structure.
 * @return STD_ReturnType  E_OK/E_NOK (E_NOK on NULL pointer).
 */
STD_ReturnType I2C_InitSlave(const I2C_SlaveConfigType *addConfig);

/**
 * @brief  Disables the TWI peripheral (clears TWEN).
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType I2C_DeInit(void);

/**
 * @brief  Generates a START (or repeated START) condition on the bus and waits
 *         for it to complete.
 * @return STD_ReturnType  E_OK if the resulting status is START/REP_START, else E_NOK.
 */
STD_ReturnType I2C_Start(void);

/**
 * @brief  Generates a STOP condition, releasing the bus.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType I2C_Stop(void);

/**
 * @brief  Transmits one byte on the bus and waits for the transfer to finish.
 *         Used both for the address byte (SLA+W/R) and for data bytes.
 * @param  uint8Data  Byte to place on the bus.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType I2C_WriteByte(uint8_h uint8Data);

/**
 * @brief  Reads one byte from the bus and returns ACK to the slave (more to read).
 * @param  puint8Data  Pointer that receives the byte; must not be NULL.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType I2C_ReadByteWithAck(uint8_h *puint8Data);

/**
 * @brief  Reads one byte from the bus and returns NACK (this is the last byte).
 * @param  puint8Data  Pointer that receives the byte; must not be NULL.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType I2C_ReadByteWithNack(uint8_h *puint8Data);

/**
 * @brief  Returns the current 5-bit TWI status code (TWSR & 0xF8), e.g. one of
 *         the I2C_STATUS_* constants.
 * @return uint8_h  The masked status code.
 */
uint8_h I2C_GetStatus(void);

/**
 * @brief  High-level master transmit: START -> SLA+W -> all data bytes -> STOP.
 * @param  slaveAddress  7-bit slave address (R/W bit added internally).
 * @param  pData         Pointer to the bytes to send; must not be NULL.
 * @param  length        Number of bytes to send.
 * @return STD_ReturnType  E_OK if every step acknowledged, otherwise E_NOK.
 */
STD_ReturnType I2C_MasterWrite(uint8_h slaveAddress, const uint8_h *pData, uint16_h length);

/**
 * @brief  High-level master receive: START -> SLA+R -> read 'length' bytes
 *         (ACK on all but the last, NACK on the last) -> STOP.
 * @param  slaveAddress  7-bit slave address (R/W bit added internally).
 * @param  pBuffer       Destination buffer; must not be NULL.
 * @param  length        Number of bytes to read.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType I2C_MasterRead(uint8_h slaveAddress, uint8_h *pBuffer, uint16_h length);

#endif /* I2C_INTERFACE_H */
