/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2019 Semtech

Description:
    SX1302 timestamp counter Hardware Abstraction Layer
    Handles the conversion of a 32-bits 32MHz counter into a 32-bits 1 MHz counter.
    This modules MUST be called regularly by the application to maintain counter
    wrapping handling for conversion in 1MHz counter.
    Provides function to compute the correction to be applied to the received
    timestamp for demodulation processing time.

License: Revised BSD License, see LICENSE.TXT file include in the project
*/


#ifndef _LORAGW_SX1302_TIMESTAMP_H
#define _LORAGW_SX1302_TIMESTAMP_H

/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include <stdint.h>     /* C99 types*/
#include <stdbool.h>    /* boolean type */

#include "loragw_hal.h"

#include "config.h"     /* library configuration options (dynamically generated) */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC CONSTANTS ----------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC MACROS -------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* --- PUBLIC TYPES --------------------------------------------------------- */

/**
@struct timestamp_counter_s
@brief context to maintain the internal counters (inst and pps trig) rollover status
*/
struct timestamp_info_s {
    uint32_t counter_us_27bits_ref;     /* reference value (last read) */
    uint8_t  counter_us_27bits_wrap;    /* rollover/wrap status */
};
typedef struct timestamp_counter_s {
    struct timestamp_info_s inst; /* holds current reference of the instantaneous counter */
    struct timestamp_info_s pps;  /* holds current reference of the pps-trigged counter */
} timestamp_counter_t;

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS ----------------------------------------------------- */

/**
@brief Initialize the timestamp_counter instance
@param self     Pointer to the counter handler
@return N/A
*/
void timestamp_counter_new(timestamp_counter_t * self);

/**
@brief Reset the timestamp_counter instance
@param self     Pointer to the counter handler
@return N/A
*/
void timestamp_counter_delete(timestamp_counter_t * self);

/**
@brief Update the counter wrapping status based on given current counter
@param self     Pointer to the counter handler
@param pps      Set to true to update the PPS trig counter status
@param cnt      Current value of the counter to be used for the update
@return N/A
*/
void timestamp_counter_update(timestamp_counter_t * self, bool pps, uint32_t cnt);

/**
@brief Convert the 27-bits counter given by the SX1302 to a 32-bits counter which wraps on a uint32_t.
@param self     Pointer to the counter handler
@param pps      Set to true to expand the counter based on the PPS trig wrapping status
@param cnt_us   The 27-bits counter to be expanded
@return the 32-bits counter
*/
uint32_t timestamp_counter_expand(timestamp_counter_t * self, bool pps, uint32_t cnt_us);

/**
@brief Convert the 27-bits packet timestamp to a 32-bits counter which wraps on a uint32_t.
@param self     Pointer to the counter handler
@param cnt_us   The packet 27-bits counter to be expanded
@return the 32-bits counter
*/
uint32_t timestamp_pkt_expand(timestamp_counter_t * self, uint32_t cnt_us);

/**
@brief Reads the SX1302 internal counter register, and return the 32-bits 1 MHz counter
@param self     Pointer to the counter handler
@param pps      Set to true to expand the counter based on the PPS trig wrapping status
@return the current 32-bits counter
*/
uint32_t timestamp_counter_get(timestamp_counter_t * self, bool pps);

/**
@brief Get the correction to applied to the LoRa packet timestamp (count_us)
@param ifmod            modem type
@param bandwidth        modulation bandwidth
@param datarate         modulation datarate
@param coderate         modulation coding rate
@param crc_en           indicates if CRC is enabled or disabled
@param payload_length   payload length
@return The correction to be applied to the packet timestamp, in microseconds
*/
int32_t timestamp_counter_correction(lgw_context_t * context, int ifmod, uint8_t bandwidth, uint8_t datarate, uint8_t coderate, bool crc_en, uint8_t payload_length);

/**
@brief Configure the SX1302 to output legacy timestamp or precision timestamp
@note  Legacy timestamp gives a timestamp latched at the end of the packet
@note  Precision timestamp gives a timestamp latched at the end of the header
@note  and additionally supplies metrics every N symbols troughout the payload.
@param enable_precision_ts  A boolean to enable precision timestamp output.
@param max_ts_metrics       The number of timestamp metrics to be returned when precision timestamp is enabled
@param nb_symbols           The sampling rate of timestamp metrics
@return LGW_REG_SUCCESS if success, LGW_REG_ERROR otherwise
*/
int timestamp_counter_mode(bool enable_precision_ts, uint8_t max_ts_metrics, uint8_t nb_symbols);

/**
@brief TODO
*/
int precise_timestamp_calculate(uint8_t ts_metrics_nb, const int8_t * ts_metrics, uint32_t pkt_coarse_tmst, double * pkt_ftime);

/**
@brief TODO
*/
int set_xtal_correct(bool is_valid, double xtal_correct);

#endif

/* --- EOF ------------------------------------------------------------------ */
