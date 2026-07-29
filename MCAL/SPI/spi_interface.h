#ifndef SPI_INTERFACE_H
#define SPI_INTERFACE_H

#include "../../Service/STD_Types.h"
#include "spi_registers.h"

/* ================================================================================
 *  SPI DRIVER - PUBLIC INTERFACE (ATmega32)
 *  ------------------------------------------------------------------------------
 *  Full-duplex synchronous serial driver. The caller selects master or slave
 *  role, the clock polarity/phase (SPI mode 0..3), the bit order, and (for a
 *  master) the SCK rate. Data is exchanged one byte at a time: every transfer
 *  simultaneously shifts a byte out and a byte in.
 * ============================================================================== */

/* ---------------- Role ---------------- */
/**
 * @brief Selects whether the device drives the clock (master) or is clocked
 *        by an external master (slave).
 */
typedef enum
{
    SPI_SLAVE  = 0,
    SPI_MASTER = 1
} SPI_RoleType;

/* ---------------- Clock Polarity (CPOL) ---------------- */
/**
 * @brief Idle level of the SCK line.
 *  - SPI_CPOL_IDLE_LOW  : SCK is low  when idle (leading edge = rising).
 *  - SPI_CPOL_IDLE_HIGH : SCK is high when idle (leading edge = falling).
 */
typedef enum
{
    SPI_CPOL_IDLE_LOW  = 0,
    SPI_CPOL_IDLE_HIGH = 1
} SPI_ClockPolarityType;

/* ---------------- Clock Phase (CPHA) ---------------- */
/**
 * @brief Edge on which data is sampled.
 *  - SPI_CPHA_SAMPLE_LEADING  : Sample on the leading  clock edge.
 *  - SPI_CPHA_SAMPLE_TRAILING : Sample on the trailing clock edge.
 * @note (CPOL, CPHA) together define the four standard SPI modes 0..3.
 */
typedef enum
{
    SPI_CPHA_SAMPLE_LEADING  = 0,
    SPI_CPHA_SAMPLE_TRAILING = 1
} SPI_ClockPhaseType;

/* ---------------- Data Order ---------------- */
/**
 * @brief Bit order on the wire.
 */
typedef enum
{
    SPI_MSB_FIRST = 0,
    SPI_LSB_FIRST = 1
} SPI_DataOrderType;

/* ---------------- Clock Rate (master only) ---------------- */
/**
 * @brief SCK frequency as a division of F_CPU. The enum value encodes SPI2X:SPR1:SPR0.
 */
typedef enum
{
    SPI_CLOCK_DIV_4   = 0,   /* SPI2X=0 SPR1=0 SPR0=0 */
    SPI_CLOCK_DIV_16  = 1,   /* SPI2X=0 SPR1=0 SPR0=1 */
    SPI_CLOCK_DIV_64  = 2,   /* SPI2X=0 SPR1=1 SPR0=0 */
    SPI_CLOCK_DIV_128 = 3,   /* SPI2X=0 SPR1=1 SPR0=1 */
    SPI_CLOCK_DIV_2   = 4,   /* SPI2X=1 SPR1=0 SPR0=0 */
    SPI_CLOCK_DIV_8   = 5,   /* SPI2X=1 SPR1=0 SPR0=1 */
    SPI_CLOCK_DIV_32  = 6    /* SPI2X=1 SPR1=1 SPR0=0 */
} SPI_ClockRateType;

/* ---------------- Configuration Structure ---------------- */
/**
 * @brief Parameters consumed by SPI_Init().
 * @var SPI_ConfigType::role       Master or slave (SPI_RoleType).
 * @var SPI_ConfigType::polarity   Clock idle level / CPOL (SPI_ClockPolarityType).
 * @var SPI_ConfigType::phase      Sampling edge / CPHA (SPI_ClockPhaseType).
 * @var SPI_ConfigType::dataOrder  MSB- or LSB-first (SPI_DataOrderType).
 * @var SPI_ConfigType::clockRate  SCK divider - used only when role == SPI_MASTER.
 */
typedef struct
{
    SPI_RoleType          role;
    SPI_ClockPolarityType polarity;
    SPI_ClockPhaseType    phase;
    SPI_DataOrderType     dataOrder;
    SPI_ClockRateType     clockRate;
} SPI_ConfigType;

/* ---------------- Callback Pointer Type ---------------- */
/**
 * @brief Type of the transfer-complete callback. Receives the byte shifted in.
 *        Runs in interrupt context - keep it short.
 */
typedef void (*SPI_CallBackType)(uint8_h receivedByte);

/* ================================================================================
 *  FUNCTION PROTOTYPES
 * ============================================================================== */

/**
 * @brief  Initializes the SPI peripheral: sets the SPI I/O pin directions for the
 *         chosen role, applies polarity/phase/data-order, sets the clock rate
 *         (master only), and enables the module.
 * @param  addConfig  Pointer to a populated configuration structure.
 * @return STD_ReturnType  E_OK on success, E_NOK on NULL pointer / invalid field.
 */
STD_ReturnType SPI_Init(const SPI_ConfigType *addConfig);

/**
 * @brief  Disables the SPI peripheral (clears SPE).
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType SPI_DeInit(void);

/**
 * @brief  Full-duplex byte exchange (blocking): shifts 'txByte' out while shifting
 *         a byte in, then returns the received byte through the pointer.
 * @param  txByte      Byte to transmit.
 * @param  puint8Rx    Pointer that receives the byte shifted in; may be NULL if
 *                     the caller does not care about the received byte.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType SPI_Transceive(uint8_h txByte, uint8_h *puint8Rx);

/**
 * @brief  Convenience wrapper that transmits one byte and discards what is read.
 * @param  txByte  Byte to transmit.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType SPI_SendByte(uint8_h txByte);

/**
 * @brief  Sends a NUL-terminated string byte-by-byte (blocking).
 * @param  pString  Pointer to a '\0'-terminated string; must not be NULL.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType SPI_SendString(const uint8_h *pString);

/**
 * @brief  Registers the transfer-complete callback and enables the SPI interrupt.
 *         The caller must also enable global interrupts.
 * @param  callBack  Pointer to a void(uint8_h) function; must not be NULL.
 * @return STD_ReturnType  E_OK/E_NOK.
 */
STD_ReturnType SPI_SetCallBack(SPI_CallBackType callBack);

#endif /* SPI_INTERFACE_H */
