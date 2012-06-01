/*
 * rtl-sdr, turns your Realtek RTL2832 based DVB dongle into a SDR receiver
 * Copyright (C) 2012 by Steve Markgraf <steve@steve-m.de>
 * Copyright (C) 2012 by Dimitri Stolnikov <horiz0n@gmx.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __RTL_SDR_H
#define __RTL_SDR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <rtl-sdr_export.h>

typedef struct rtlsdr_dev rtlsdr_dev_t;

RTLSDR_API uint32_t rtlsdr_get_device_count(void);

RTLSDR_API const char* rtlsdr_get_device_name(uint32_t index);

/*!
 * Get USB device strings.
 *
 * NOTE: The string arguments must provide space for up to 256 bytes.
 *
 * \param index the device index
 * \param manufact manufacturer name, may be NULL
 * \param product product name, may be NULL
 * \param serial serial number, may be NULL
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_get_device_usb_strings(uint32_t index,
					     char *manufact,
					     char *product,
					     char *serial);

RTLSDR_API int rtlsdr_open(rtlsdr_dev_t **dev, uint32_t index);

RTLSDR_API int rtlsdr_close(rtlsdr_dev_t *dev);

/* configuration functions */

/*!
 * Set crystal oscillator frequencies used for the RTL2832 and the tuner IC.
 *
 * Usually both ICs use the same clock. Changing the clock may make sense if
 * you are applying an external clock to the tuner or to compensate the
 * frequency (and samplerate) error caused by the original cheap crystal.
 *
 * NOTE: Call this function only if you know what you are doing.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param rtl_freq frequency value used to clock the RTL2832 in Hz
 * \param tuner_freq frequency value used to clock the tuner IC in Hz
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_set_xtal_freq(rtlsdr_dev_t *dev, uint32_t rtl_freq,
				    uint32_t tuner_freq);

/*!
 * Get crystal oscillator frequencies used for the RTL2832 and the tuner IC.
 *
 * Usually both ICs use the same clock.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param rtl_freq frequency value used to clock the RTL2832 in Hz
 * \param tuner_freq frequency value used to clock the tuner IC in Hz
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_get_xtal_freq(rtlsdr_dev_t *dev, uint32_t *rtl_freq,
				    uint32_t *tuner_freq);

/*!
 * Get USB device strings.
 *
 * NOTE: The string arguments must provide space for up to 256 bytes.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param manufact manufacturer name, may be NULL
 * \param product product name, may be NULL
 * \param serial serial number, may be NULL
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_get_usb_strings(rtlsdr_dev_t *dev, char *manufact,
				      char *product, char *serial);

RTLSDR_API int rtlsdr_set_center_freq(rtlsdr_dev_t *dev, uint32_t freq);

/*!
 * Get actual frequency the device is tuned to.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \return 0 on error, frequency in Hz otherwise
 */
RTLSDR_API uint32_t rtlsdr_get_center_freq(rtlsdr_dev_t *dev);

RTLSDR_API int rtlsdr_set_freq_correction(rtlsdr_dev_t *dev, int ppm);

RTLSDR_API int rtlsdr_get_freq_correction(rtlsdr_dev_t *dev);

enum rtlsdr_tuner {
	RTLSDR_TUNER_UNKNOWN = 0,
	RTLSDR_TUNER_E4000,
	RTLSDR_TUNER_FC0012,
	RTLSDR_TUNER_FC0013,
	RTLSDR_TUNER_FC2580
};

/*!
 * Get the tuner type.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \return <= 0 on error, tuner type otherwise
 */
RTLSDR_API enum rtlsdr_tuner rtlsdr_get_tuner_type(rtlsdr_dev_t *dev);

/*!
 * Get a list of gains supported by the tuner.
 *
 * NOTE: The gains argument must be preallocated by the caller. If NULL is
 * being given instead, the number of available gain values will be returned.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param gains array of gain values. In tenths of a dB, 115 means 11.5 dB.
 * \return <= 0 on error, number of available (returned) gain values otherwise
 */
RTLSDR_API int rtlsdr_get_tuner_gains(rtlsdr_dev_t *dev, int *gains);

/*!
 * Set the gain for the device.
 * Manual gain mode must be enabled for this to work.
 *
 * Valid gain values (in tenths of a dB) for the E4000 tuner:
 * -10, 15, 40, 65, 90, 115, 140, 165, 190,
 * 215, 240, 290, 340, 420, 430, 450, 470, 490
 *
 * Valid gain values may be queried with \ref rtlsdr_get_tuner_gains function.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param gain in tenths of a dB, 115 means 11.5 dB.
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_set_tuner_gain(rtlsdr_dev_t *dev, int gain);

/*!
 * Get actual gain the device is configured to.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \return 0 on error, gain in tenths of a dB, 115 means 11.5 dB.
 */
RTLSDR_API int rtlsdr_get_tuner_gain(rtlsdr_dev_t *dev);

/*!
 * Set the gain mode (automatic/manual) for the device.
 * Manual gain mode must be enabled for the gain setter function to work.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param manual gain mode, 1 means manual gain mode shall be enabled.
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_set_tuner_gain_mode(rtlsdr_dev_t *dev, int manual);

/* this will select the baseband filters according to the requested sample rate */
RTLSDR_API int rtlsdr_set_sample_rate(rtlsdr_dev_t *dev, uint32_t rate);

/*!
 * Get actual sample rate the device is configured to.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \return 0 on error, sample rate in Hz otherwise
 */
RTLSDR_API uint32_t rtlsdr_get_sample_rate(rtlsdr_dev_t *dev);

/*!
 * Enable test mode that returns an 8 bit counter instead of the samples.
 * The counter is generated inside the RTL2832.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param test mode, 1 means enabled, 0 disabled
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_set_testmode(rtlsdr_dev_t *dev, int on);

/* streaming functions */

RTLSDR_API int rtlsdr_reset_buffer(rtlsdr_dev_t *dev);

RTLSDR_API int rtlsdr_read_sync(rtlsdr_dev_t *dev, void *buf, int len, int *n_read);

typedef void(*rtlsdr_read_async_cb_t)(unsigned char *buf, uint32_t len, void *ctx);

/*!
 * Read samples from the device asynchronously. This function will block until
 * it is being canceled using rtlsdr_cancel_async()
 *
 * NOTE: This function is deprecated and is subject for removal.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param cb callback function to return received samples
 * \param ctx user specific context to pass via the callback function
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_wait_async(rtlsdr_dev_t *dev, rtlsdr_read_async_cb_t cb, void *ctx);

/*!
 * Read samples from the device asynchronously. This function will block until
 * it is being canceled using rtlsdr_cancel_async()
 *
 * \param dev the device handle given by rtlsdr_open()
 * \param cb callback function to return received samples
 * \param ctx user specific context to pass via the callback function
 * \param buf_num optional buffer count, buf_num * buf_len = overall buffer size
 *		  set to 0 for default buffer count (32)
 * \param buf_len optional buffer length, must be multiple of 512,
 *		  set to 0 for default buffer length (16 * 32 * 512)
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_read_async(rtlsdr_dev_t *dev,
				 rtlsdr_read_async_cb_t cb,
				 void *ctx,
				 uint32_t buf_num,
				 uint32_t buf_len);

/*!
 * Cancel all pending asynchronous operations on the device.
 *
 * \param dev the device handle given by rtlsdr_open()
 * \return 0 on success
 */
RTLSDR_API int rtlsdr_cancel_async(rtlsdr_dev_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* __RTL_SDR_H */
