"""
/*******************************************************************************
* Copyright (C) Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
*******************************************************************************
*/
"""

import argparse
import colorama
import serial
from colorama import *
import time
from msbl import MsblFile
from api import bootloader_api

if __name__ == "__main__":
	# Parse command line arguments
	parser = argparse.ArgumentParser()
	parser.add_argument("-f", "--msblfile", required=True, type=str, help="msbl file as input")
	parser.add_argument("-p", "--port", required=True, type=str, help=("Serial port name in Windows and device file path in Linux."))
	args = parser.parse_args()

	colorama.init(convert=True)

	# Load data from msbl file
	print("Reading msbl file " + Fore.YELLOW + args.msblfile + Fore.WHITE + "...", end="")
	msbl = MsblFile(args.msblfile)
	print(Fore.GREEN + "Success!" + Fore.WHITE)

	msbl.print_info()

	# Connect to 32630FTHR
	print(f"Connecting to MAX32630FTHR on {Fore.YELLOW}{args.port}{Fore.WHITE}...", end="")
	bl = bootloader_api(args.port)
	print(f"{Fore.GREEN}Success!{Fore.WHITE}")

	# Enter application mode on start-up
	print(f"Resetting to application mode...")
	bl.send_cmd("exit")

	# Let Sensor Hub initialize
	print("Sleeping for 3s to let Sensor Hub initialize", end="")
	for i in range(6):
		time.sleep(0.5)
		print(".", end="")
	print("Done!")

	# Check for any existing firmware
	print("\tAttempting to retrieve version number...", end="")
	version = bl.send_cmd("sh_version", suppress=True) # Suppress exceptions for this command, since there may not be any firmware currently flashed

	if version is not False:
		print(f"{Fore.GREEN}Success!{Fore.WHITE}")
		print(f"\t{Fore.YELLOW}Current firmware is v{version}{Fore.WHITE}")
	else:
		print(f"\t{Fore.RED}Failed.{Fore.WHITE}")
		print(f"{Fore.CYAN}Failed to retrieve current version #, but there may not be anything flashed.  Attempting to proceed anyways...{Fore.WHITE}")

	# Enter bootloader mode
	print(f"Entering bootloader mode...")
	bl.send_cmd("bootldr")

	# Verify operating mode
	op_mode = bl.send_cmd("op_mode")
	if (op_mode != "Bootloader"): raise ( Exception( f"Failed to verify bootloader mode...  Device is in {op_mode} mode." ) )

	# Get bootloader version
	print(f"Getting bootloader version...", end="")
	bl_version = bl.send_cmd("bootloader_version")
	print(f"{Fore.GREEN}Success!{Fore.WHITE}")
	print(f"\t{Fore.YELLOW}Bootloader version: {bl_version}{Fore.WHITE}")

	# Get page size
	print(f"Getting page size...", end="")
	page_size = bl.send_cmd("page_size")
	print(f"{Fore.GREEN}Success!{Fore.WHITE}")

	# Verify page size
	if page_size != msbl.header.pageSize:
		print(f"{Fore.RED}Page size in bootloader does not match page size of msbl file.  Msbl file specifies a page size of {msbl.header.pageSize} but bootloader has a page size of {page_size}")
	else:
		print(f"{Fore.GREEN}Page size in bootloader matches page size of msbl file.{Fore.WHITE}")

	# Set number of pages to flash
	print(f"Setting number of pages to flash from msbl file ({msbl.header.numPages})...", end="")
	bl.send_cmd(f"num_pages {msbl.header.numPages}")
	print(f"{Fore.GREEN}Success!{Fore.WHITE}")

	# Set IV bytes ('nonce' from the header)
	print(f"Setting Initialization Vector (IV) bytes...", end="")
	nonce_hex = "".join("{:02X}".format(c) for c in msbl.header.nonce) # Firmware expects an ASCII representation of the hex values for the IV bytes concatenated into one long string
	bl.send_cmd(f"set_iv {nonce_hex}")
	print(f"{Fore.GREEN}Success!{Fore.WHITE}")

	# Set authentication bytes
	print(f"Setting Authentication bytes...", end="")
	auth_hex = "".join("{:02X}".format(c) for c in msbl.header.auth)
	bl.send_cmd(f"set_auth {auth_hex}")
	print(f"{Fore.GREEN}Success!{Fore.WHITE}")

	# Erase existing application flash memory
	print(f"Erasing existing msbl file...", end="")
	bl.send_cmd("erase")
	print(f"{Fore.GREEN}Success!{Fore.WHITE}")

	# Flash the msbl file page by page
	print("Flashing msbl file...")
	i = 1
	for page in msbl.pages:
		print(f"\tFlashing page {i}/{int(msbl.header.numPages)}...", end="")
		bl.flash_page(page)
		print(f"{Fore.GREEN}Success!{Fore.WHITE}")

		i += 1

	# Exit bootloader mode, enter application mode
	print("Exiting bootloader mode...")
	bl.send_cmd("exit")

	# Let Sensor Hub initialize
	print("Sleeping for 3s to let Sensor Hub initialize", end="")
	for i in range(6):
		time.sleep(0.5)
		print(".", end="")
	print("Done!")

	# Verify application mode
	print(f"Retrieving operating mode...", end="")
	op_mode = bl.send_cmd("op_mode")
	print(f"{Fore.GREEN}Success!{Fore.WHITE}")

	# Verify operating mode
	if (op_mode != "Application"): raise ( Exception( f"Failed to verify bootloader mode...  Device is in {op_mode} mode." ) )

	# Verify new firmware version
	print("Retrieving new firmware version...", end="")
	version = bl.send_cmd("sh_version")
	print(f"{Fore.GREEN}Success!{Fore.WHITE}")

	print(f"New firmware version is {Fore.YELLOW}v{version}{Fore.WHITE}")
