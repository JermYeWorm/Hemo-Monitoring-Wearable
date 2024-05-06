# IPG Wearable Platform
A novel wearable health platform for hemodialysis patients to monitor post-surgery vascular flow characteristics in arteriovenous fistulas (AVFs) through impedance plethysmography (IPG). The goal is to enable real time processing by integrating both analog front end and edge ML, before wireless transmit. The current revision is IPG, ECG, and photoplethysmography (PPG) bio-monitoring enabled. The PPG system is capable of performing heart rate measurements and pulse oximetry.
<br><br>

## System Flowchart
![image](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/9509c466-9438-4529-a761-cc8f7f295b4b)
<br><br>

## Hardware
The wearable platform runs on a BLE capable nRF52840 MCU SoC within a NINA-B302 FIFA antenna module. 
<br><br>
The MCU pinout is chosen to match the I2C, SPI, and spare analog/digital pins of an Arduino Nano 33 BLE. This allows for the burning of an Arduino Nano 33 BLE bootloader and still function with no internal Arduino library modifications or need to access hidden pins. The MCU may also be programmed directly using the nRF Connect SDK.
<br><br>
The MCU is an open CPU and will require access through the SWD interface to enable programming through the USB port. A standard JLink with a 10 pin SWD cable connected to the J1 connector will allow for hardware debug and burning of bootloaders. 
<br><br>
![image](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/efba7726-850a-4812-9a9b-c988d411f924)
<br>
![image](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/6b7a42f4-19e5-4ef3-a293-142e4cccbfd8)
<br><br>

### Schematic
Hierarchical Schematic Structure:<br>
```
├── [1] ipg_wearable.SchDoc
        ├── [2] sensors.SchDoc
        ├── [3] connectors.SchDoc
        └── [4] power_supply.SchDoc
```
<br>

![1714973704757-3ba7de04-8b89-4758-92bf-7abcabce8b58_1](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/26b00149-5dd8-4226-9bc7-3e8e0e4e1620)
<br>

![1714973704757-3ba7de04-8b89-4758-92bf-7abcabce8b58_2](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/29d9bdce-02a3-41f3-9d10-136f45512d3f)
<br>

![1714973704757-3ba7de04-8b89-4758-92bf-7abcabce8b58_3](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/1b167fc5-132d-4a4d-93b4-5fcd07ea8abc)
<br>

![1714973704757-3ba7de04-8b89-4758-92bf-7abcabce8b58_4](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/7d4a8488-626c-43fd-980a-48f274413e8d)
<br><br>

### Board Layout
![image](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/0f92435f-ac0c-421e-9a39-34618c8f7516)
<br>_Fig 1. Top Layer (Signal)_<br><br>
![image](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/dd254d63-ee3c-4daf-88ca-807670cd68d3)
<br>_Fig 2. GND Layer_<br><br>
![image](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/f3cc15ef-bf16-436c-a69f-bfac4d7dbc25)
<br>_Fig 3. PWR Layer_<br><br>
![image](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/ee679546-e9ff-4c30-8e5e-7ae01409f6f2)
<br>_Fig 4. Bottom Layer (Signal)_<br><br>
![image](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/156e53fa-7c06-43ac-8e4b-818b9fdacad8)
<br>_Fig 5. Mechanical + Dimensions_<br><br>

### 3D Model
![v1-ipg-front](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/1760f4b1-e788-44fd-8f6d-9f1e72aecd05)
<br>_Fig 1. Front View_<br><br>

![v1-ipg-back](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/80fedf3b-ca3c-44aa-97c2-3347b09528e7)
<br>_Fig 2. Back View (PPG Components Visible)_<br><br>

![v1-ipg-iso](https://github.com/JermYeWorm/IPG-Wearable/assets/113321384/b7dc9d76-87b1-4f1e-a144-0eb8fcd5f38e)
<br>_Fig 3. Isometric View_<br><br>

## Contributors
Project Researcher - Jeremy Yun
<br>
Principal Investigator - Dr. Sahil Shah
<br>
