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

from ctypes import *
from colorama import *
from copy import deepcopy

class MsblHeader(Structure):
	_fields_ = [('magic', 4 * c_char),
				('formatVersion', c_uint),
				('target', 16 * c_char),
				('enc_type', 16 * c_char),
				('nonce', 11 * c_ubyte),
				('resv0', c_ubyte),
				('auth', 16 * c_ubyte),
				('numPages', c_ushort),
				('pageSize', c_ushort),
				('crcSize', c_ubyte),
				('resv1', 3 * c_ubyte)]

class AppHeader(Structure):
	_fields_ = [('crc32', c_uint),
				('length', c_uint),
				('validMark', c_uint),
				('boot_mode', c_uint)]

class Page(Structure):
	_fields_ = [('data', (8192 + 16) * c_ubyte)]

class CRC32(Structure):
	_fields_ = [('val', c_uint)]

class MsblFile:
    header = MsblHeader()
    pages = []
    crc32 = CRC32()

    def __init__( self, filename ):
        with open( filename, 'rb' ) as msbl:
            # Read out header	
            if msbl.readinto( self.header ) != sizeof( self.header ):
                raise( Exception( f"{Fore.RED}Failed to load msbl file {filename}.  Invalid header{Fore.WHITE}" ) )

            # Read out page data
            tmp_page = Page()
            while msbl.readinto( tmp_page ) == sizeof( tmp_page ):
                self.pages.append( deepcopy( tmp_page.data ) ) # Deep copy is used so that a new copy of the page data is actually appended to the list.  See https://docs.python.org/3/library/copy.html 

            # Verify the correct number of pages were read.
            if (len(self.pages) != self.header.numPages):
               raise( Exception (f"{Fore.RED}Failed to load msbl file.  Expected to read {self.header.numPages} pages but read {len(self.pages)} instead.{Fore.WHITE}") )


            # Read out CRC32		
            msbl.seek(-4, 2)
            msbl.readinto(self.crc32)

        # msbl file data is now loaded into class attributes (header, pages, crc32)

    def print_info(self):
        # Print MSBL file information
        print(f"\tMagic: {self.header.magic.decode('ASCII')}")
        print(f"\tFormat Version: {self.header.formatVersion}")
        print(f"\tTarget: {self.header.target.decode('ASCII')}")
        print(f"\tEncoding Type: {self.header.enc_type.decode('ASCII')}")
        print(f"\tNum Pages: {self.header.numPages}")
        print(f"\tPage Size: {self.header.pageSize}")
        print(f"\tCRC Size: {self.header.crcSize}")
        print(f"\tSize of header: {sizeof(self.header)}")
        print(f"\tResv0: {self.header.resv0}")
        print(f"\tCRC32: {hex(self.crc32.val)}")