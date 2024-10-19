# Hemodynamic Health Monitoring Wearable Platform
This device is an open-source, programmable, highly-integrated wearable platform capable of simultaneously measuring multiple hemodynamic biosignals. The goal is to enable real time processing by integrating both analog front end and edge ML, before wireless transmit. The current revision is electrical bioimpedance (EBI), electrocardiogram (ECG), and photoplethysmography (PPG) bio-monitoring enabled, and contains an onboard microprocessor and BLE transmit module.
<br><br>
The ongoing work on this project is with the Shah lab at the University of Maryland, College Park.
<br><br>
![image](https://github.com/JermYeWorm/Hemodynamic-Monitoring-Wearable/assets/113321384/a1fd0d2d-41c1-4abf-bccf-92a026fa436c)
<br><br>


## System Flowchart
![image](https://github.com/JermYeWorm/Hemodynamic-Monitoring-Wearable/assets/113321384/130fc2e2-656f-4bfe-95c9-b60f719724cd)
<br><br>

## Hardware
All PCB schematic capture and board layout was done with Altium Designer 2024.

The wearable platform runs on a BLE capable nRF52840 MCU SoC within a NINA-B302 FIFA antenna module. 
<br><br>
The MCU pinout is chosen to match the I2C, SPI, and spare analog/digital pins of an Arduino Nano 33 BLE. This allows for programmming via Arduino IDE after the burning of an Arduino Nano 33 BLE bootloader without the need for internal Arduino library pinout modifications. The MCU may also be programmed directly using the nRF Connect SDK.
<br><br>
The MCU is an open CPU and will require access through the SWD interface to enable programming through the USB port. A standard JLink with a 10 pin SWD cable connected to the J1 connector will allow for hardware debug and burning of bootloaders. 
<br><br>
![image](https://github.com/JermYeWorm/Hemodynamic-Monitoring-Wearable/assets/113321384/dd65ab83-1b07-4aad-ad64-3bf811d65519)
<br>
![image](https://github.com/JermYeWorm/Hemodynamic-Monitoring-Wearable/assets/113321384/3e39affa-d2bf-464c-91aa-b6333be164a8)
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

### Fabricated Device
![2](https://github.com/user-attachments/assets/1f72a4c6-8dc6-4642-bdf3-25f1adf20798)
<br>_Fig 1. Front View_<br><br>

![1](https://github.com/user-attachments/assets/10afcda5-7862-4310-afaf-526c9464fa2c)
<br>_Fig 2. Back View (PPG Components Visible)_<br><br>

## Results
![image](https://github.com/JermYeWorm/Hemodynamic-Monitoring-Wearable/assets/113321384/ee90e40f-74b3-4544-9d96-b8c64cb00384)
<br>_**(Top)** ECG with real time R-to-R detection using the onboard biopotential front end. **(Bottom)** EBI demonstration with electrodes placed across the thoracic cavity during respiration._<br><br>

## Contributors
Project Researcher - Jeremy Yun
<br>
Principal Investigator - Dr. Sahil Shah
<br>
