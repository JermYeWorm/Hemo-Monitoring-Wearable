# About
Software and firmware for flashing .msbl files using the [MAX32630FTHR](https://www.maximintegrated.com/en/products/microcontrollers/MAX32630FTHR.html).  This software flashes .msbl files onto a [MAX32664](https://www.maximintegrated.com/en/products/interface/signal-integrity/MAX32664.html) Biometric Sensor Hub.  The MAX32664 can be on one of Maxim's reference designs or on a custom PCB, provided that the necessary hardware connections are exposed correctly.
- RSTN
- MFIO
- SLAVE_SCL (external pull-up required)
- SLAVE_SDA (external pull-up required)
- VSS (GND)

Additionally, VDD power must be supplied to the MAX32664.  This can come from the MAX32630FTHR or from the target design.  This software uses 3.3V logic level.
It's recommended to power the MAX32664 from 3.3V for the flash, otherwise use a logic level translator to 1.8V.

# Usage
For a detailed usage guide, see:  [KA-13834](https://maximsupport.microsoftcrmportals.com/en-us/knowledgebase/article/KA-13834)

1.  Connect the SWD cable between the smaller MAX32625PICO programmer board ("pico") to the MAX32630FTHR ("fthr").

2.  Connect both boards to the host PC via the micro-usb ports.

3.  Drag and drop "MAX32630FTHR_msbl_flasher.bin" onto the DAPLINK drive to flash the "fthr" board.

4.  Remove the connections for the "pico" board.

5.  Connect the "fthr" board to the MAX32664.  For pin-outs on the "fthr" board, see [MAX32630FTHR Datasheet](https://datasheets.maximintegrated.com/en/ds/MAX32630FTHR.pdf)
	- Connect P3_5 to SLAVE_SCL (external pull-up required)
	- Connect P3_4 to SLAVE_SDA (external pull-up required)
	- Connect P5_6 to RSTN
	- Connect P5_4 to MFIO
	- Connect GND to VSS (GND) on the design
	- (Optional) Connect 3V3 to VDD, otherwise supply power from the MAX32664's board

6.  Open a command prompt and "cd" into this directory

7.  Run the command below to flash an msbl file.  Run flash.exe -h for help with the host program.

	`"flash.exe -f [msbl filename] -p [MAX32630FTHR COM Port]`

	Ex:  `"flash.exe -f "MAX32664C_OB07_WHRM_AEC_SCD_WSPO2_C_33.13.12.msbl" -p "COM16"`

	If the msbl file is not located next to the executable, you will need to pass in the full filepath.  
	Ex:  `-f "C:\Documents\MAX32664\msbl files\MAX32664C_OB07_WHRM_AEC_SCD_WSPO2_C_33.13.12.msbl"`

8.  Alternatively, the flashing program can be run as a Python (Python 3+) script.  "cd" into the Python directory and run "pip install -r requirements.txt".  Then, use...

	`"python flash.py -f [msbl filename] -p [your COM port]`
	
9. Alternatively, on Windows the included "run.bat" file can be used. This file will prompt the user to select from a list of .msbl files that were copied in the same location as the "flash.exe" file. Once the file has been selected by the assigned list number, then the console prompts the user to enter the assigned COM port to the MAX32630FTHR. Once this has been entered the program proceeds to flash the msbl file.

# Source Code
Full source code is available [here](https://github.com/MaximIntegratedTechSupport/MAX32630FTHR_msbl_flasher), and is dependent on mbed-os.  

The project can be built with mbed-cli, which is not straightforward to set up.  See https://maximsupport.microsoftcrmportals.com/en-us/knowledgebase/article/KA-15675.  With mbed-cli set up, run:

	mbed init
	mbed deploy (this will take a while)
	
and...
	
	mbed compile
	
... to build the firmware.

The flash.exe executable was generated from the Python source files with pyinstaller:
	
	pip install pyinstaller
	pip install auto-py-to-exe
	pyinstaller --noconfirm --onefile --console flash.py
