/*
 * unicone_device.h - Abstractions for the unicone device,
 *                    configuring the firmware and FPGA, and
 *                    communicating with the device's I2C bus.
 *
 * Universal Controller Emulator project
 * Copyright (C) 2004 Micah Dowty
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#ifndef _H_UNICONE_DEVICE
#define _H_UNICONE_DEVICE

#include <usb.h>
#include "progress.h"

struct unicone_device {
  usb_dev_handle *usbdev;

  int firmware_installed;
  int fpga_configured;
};


/******************************************************************************/
/************************************************************ Initialization **/
/******************************************************************************/

/* Convenience function to initialize libusb */
void                      unicone_usb_init                ();

/* Look for the first unicone device (or bootloader device) found, and open it.
 * This requires that libusb be initialized beforehand. Note that if you're trying
 * to reopen a device after firmware has been installed, you'll need to call
 * usb_find_devices() a second time.
 */
struct unicone_device*    unicone_device_new              ();
void                      unicone_device_delete           (struct unicone_device *self);

/* Disconnect from a device, then reconnect after a delay */
int                       unicone_device_reconnect        (struct unicone_device *self,
							   struct progress_reporter* progress);


/******************************************************************************/
/************************************************ Firmware / FPGA Management **/
/******************************************************************************/

/* Returns 0 if the given firmware file has the same contents as the device, 1
 * if the contents are different, or -1 on error.
 */
int                       unicone_device_compare_firmware (struct unicone_device*    self,
							   const char*               filename,
							   struct progress_reporter* progress);

/* Returns 0 if the given bitstream file has the same contents as the device, 1
 * if the contents are different, or -1 on error.
 */
int                       unicone_device_compare_bitstream(struct unicone_device*    self,
							   const char*               filename,
							   struct progress_reporter* progress);

/* Ensure that the device contains the given firmware and FPGA configuration
 * bitstream, sending them only if they differ from what (if anything) is in
 * the device already.
 */
int                       unicone_device_configure        (struct unicone_device*    self,
							   const char*               firmware_file,
							   const char*               bitstream_file,
							   struct progress_reporter* progress);


/******************************************************************************/
/**************************************************** Firmware / FPGA Upload **/
/******************************************************************************/

/* Install firmware from the given raw binary file. Optionally
 * report progress using the given interface. Returns the number of
 * bytes transferred on success, or -1 on error. If this is successfull,
 * the existing unicone_device will need to be closed, then reopened
 * following a usb_find_devices().
 */
int                       unicone_device_upload_firmware  (struct unicone_device*    self,
							   const char*               filename,
							   struct progress_reporter* progress);

/* Remove firmware from an existing device by rebooting it into the bootloader.
 * Returns 0 on success, -1 on error. If successful, the existing unicone_device
 * will need to be reopened as described above.
 */
int                       unicone_device_remove_firmware  (struct unicone_device *self);

/* Install an FPGA configuration from the given bitstream file. Optionally
 * report progress using the given interface. Returns the number of
 * bytes transferred on success, or -1 on error.
 */
int                       unicone_device_upload_bitstream (struct unicone_device*    self,
							   const char*               filename,
							   struct progress_reporter* progress);


/******************************************************************************/
/************************************************** Low-level Communications **/
/******************************************************************************/

/* Update the device's status LED. This requires FPGA support for the LED
 * brightness control I2C core, but we get the firmware's help in performing fades.
 */
int                       unicone_device_set_led          (struct unicone_device*    self,
							   float                     brightness,
							   float                     decay_rate);

/* Make a generic I2C write to the FPGA. Returns 0 on success, -1 on USB error. I2C errors
 * are not yet reported via this function, they will appear on the serial port.
 */
int                       unicone_device_i2c_write        (struct unicone_device*    self,
							   int                       i2c_addr,
							   const unsigned char*      data,
							   int                       length);

#endif /* _H_UNICONE_DEVICE */

/* The End */
