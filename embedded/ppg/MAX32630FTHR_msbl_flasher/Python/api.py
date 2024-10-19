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

import serial
from colorama import Fore
from dataclasses import dataclass

@dataclass
class response:
    cmd: str
    ret: str
    err: int
    msg: str
    
status_codes = {
    0x00: "SUCCESS.  The write transaction was successful.",
    0x01: "ERR_UNAVAIL_CMD. Illegal Family Byte and/or Index Byte was used. Verify that the Family Byte, Index Byte are valid for the host command sent. Verify that the latest .msbl is flashed.",
    0x02: "ERR_UNAVAIL_FUNC. This function is not implemented. Verify that the Index Byte and Write Byte(s) are valid for the host command sent. Verify that the latest .msbl is flashed.",
    0x03: "ERR_DATA_FORMAT. Incorrect number of bytes sent for the requested Family Byte. Verify that the correct number of bytes are sent for the host command. Verify that the latest .msbl is flashed.",
    0x04: "ERR_INPUT_VALUE. Illegal configuration value was attempted to be set. Verify that the Index Byte is correct for Family Byte 0x44. Verify that the report period is not 0 for host command 0x10 0x02. Verify that the Write byte for host command 0x10 0x03 is in the valid range specified. Verify that the latest .msbl is flashed.",
    0x05: "Application mode: ERR_INVALID_MODE. Not used in application mode.\nBootloader mode: ERR_ BTLDR_TRY_AGAIN. Device is busy. Insert delay and resend the host command.",
    0x80: "ERR_BTLDR_GENERAL. General error while receiving/flashing a page during the bootloader sequence. Not used.",
    0x81: "ERR_BTLDR_CHECKSUM. Bootloader checksum error while decrypting/checking page data. Verify that the keyed .msbl file is compatible with MAX32664A/B/C/D.",
    0x82: "ERR_BTLDR_AUTH. Bootloader authorization error. Verify that the keyed .msbl file is compatible with MAX32664A/B/C/D.",
    0x83: "ERR_BTLDR_INVALID_APP. Bootloader detected that the application is not valid.",
    0xFE: "ERR_TRY_AGAIN. Device is busy, try again. Increase the delay before the command and increase the CMD_DELAY.",
    0xFF: "ERR_UNKNOWN. Unknown Error. Verify that the communications to the AFE/KX-122 are correct by reading the PART_ID/WHO_AM_I register. For MAX32664B/C, the MAX32664 is in deep sleep unless the host sets the MFIO pin low 250μs before and during the I2C communications."
} # See MAX32664 UG Table 5 (https://www.maximintegrated.com/en/design/technical-documents/app-notes/6/6806.html)

class bootloader_api():
    def __init__(self, port: str, BAUD=9600, timeout=3 ):
        self.s = serial.Serial(port=port, baudrate=BAUD, timeout=timeout)

    def send_cmd(self, cmd: str, suppress=False) -> response:  
        self.s.write ( bytes( f"{cmd}\n", "ASCII" ) )
        resp = self._parse_response( self.s.readline() )

        if resp is False:
            # Unexpected response/I2C communication failure
            err = f"Failed to parse expected response from command {cmd}!"
            if not suppress: raise( Exception( err ) )
            else:
                print(err)
                return False

        elif resp.err != 0:
            # Error code received from Sensor Hub
            err = ""
            if resp.err in status_codes.keys():
                err = f"{Fore.RED}Error code {hex(resp.err)} received from sensor hub during command {cmd}...  {status_codes[resp.err]}{Fore.WHITE}"
            else:
                err = f"{Fore.RED}Unknown error code {hex(resp.err)} received from sensor hub during command {cmd} with message: {resp.msg}...{Fore.WHITE}"

            if not suppress: raise( Exception( err ) )
            else:
                print(err)
                return False

        else:
            # Everything's ok, pass any return values received
            return resp.ret

    def flash_page(self, page: bytes):
        self.s.write(b"flash\n")
        self.s.write( bytes( page ) )
        resp = self._parse_response( self.s.readline() )

        if resp is False:
            # Unexpected response/I2C communication failure
            raise( Exception( "Failed to parse expected response when flashing page data!") )

        elif resp.err != 0:
            # Error code received from Sensor Hub
            err_msg = ""
            if resp.err in status_codes.keys():
                err_msg = f"{Fore.RED}Error code {hex(resp.err)} received from sensor hub while flashing page data...  {status_codes[resp.err]}{Fore.WHITE}"
            else:
                err_msg = f"{Fore.RED}Unknown error code {hex(resp.err)} received from sensor hub while flashing page data... {resp.msg}{Fore.WHITE}"

            raise(Exception(err_msg))

        else:
            # Page flashed OK
            return True


    def _parse_response(self, resp: bytes):
        # Response will be a string with the format:  cmd=[some command]$ret=[return value]$err=[error code]$msg=[error msg]
        # This function splits the string up into tokens and returns the response as a decoded dictionary
        copy = resp # Save a copy for error msg
        resp = resp.decode("ASCII").split('$')

        d = dict()
        for field in resp:
            key_pair = field.split('=')
            if len(key_pair) == 2: 
                d[key_pair[0]] = key_pair[1] # Construct a dictionary entry from the key-value pair.  Key will be first

        # Convert to integer values where we can for convenience
        for key in d.keys():
            try:
                d[key] = int(d[key])
            except:
                pass

        # Validate response
        if "cmd" not in d.keys() or "ret" not in d.keys() or "err" not in d.keys() or "msg" not in d.keys():
            return False
            #raise( Exception( f"Unexpected response from device - failed to parse fields.  Received:\n{copy}\n... and parsed into:\n{ret}" ) )

        return response( d["cmd"], d["ret"], d["err"], d["msg"] )
