//////////////////////////////////////////////////////////////////////////////////////////
//
//  Header for the MAX30001
//
//  This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
/////////////////////////////////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <SPI.h>

#define WREG 0x00
#define RREG 0x01

#define   NO_OP           0x00
#define   STATUS          0x01
#define   EN_INT          0x02
#define   EN_INT2         0x03
#define   MNGR_INT        0x04
#define   MNGR_DYN        0x05
#define   SW_RST          0x08
#define   SYNCH           0x09
#define   FIFO_RST        0x0A
#define   INFO            0x0F
#define   CNFG_GEN        0x10
#define   CNFG_CAL        0x12
#define   CNFG_EMUX       0x14
#define   CNFG_ECG        0x15
#define   CNFG_BMUX       0x17
#define   CNFG_BIOZ       0x18
#define   CNFG_PACE       0x1A
#define   CNFG_RTOR1      0x1D
#define   CNFG_RTOR2      0x1E
#define   ECG_FIFO_BURST  0x20
#define   ECG_FIFO        0x21
#define   BIOZ_FIFO_BURST 0x22
#define   BIOZ_FIFO       0x23
#define   RTOR            0x25
#define   PACE0_BURST     0x30
#define   PACE0_A         0x31
#define   PACE0_B         0x32
#define   PACE0_C         0x33
#define   PACE1_BURST     0x34
#define   PACE1_A         0x35
#define   PACE1_B         0x36
#define   PACE1_C         0x37
#define   PACE2_BURST     0x38
#define   PACE2_A         0x39
#define   PACE2_B         0x3A
#define   PACE2_C         0x3B
#define   PACE3_BURST     0x3C
#define   PACE3_A         0x3D
#define   PACE3_B         0x3E
#define   PACE3_C         0x3F
#define   PACE4_BURST     0x40
#define   PACE4_A         0x41
#define   PACE4_B         0x42
#define   PACE4_C         0x43
#define   PACE5_BURST     0x44
#define   PACE5_A         0x45
#define   PACE5_B         0x46
#define   PACE5_C         0x47
#define   NO_OP           0x7F

#define MAX30001_CS_PIN   10
#define CLK_PIN           6
#define RTOR_INTR_MASK    0x04

typedef enum
{
  SAMPLINGRATE_128 = 128, 
  SAMPLINGRATE_256 = 256, 
  SAMPLINGRATE_512 = 512
} sampRate;



typedef struct
{
    signed long value;
    uint8_t e_tag;
    uint8_t p_tag;
} ecg_data;

typedef struct
{
    signed long value;
    uint8_t b_tag;
} bioz_data;

class MAX30001
{
  public:
  	unsigned int heartRate;
  	unsigned int RRinterval;
    ecg_data ecgData;
    bioz_data biozData;

    void max30001Begin();
    void max30001BeginRtoRMode();
    void max30001SwReset(void);
    void getHRandRR(void);
    void getEcgSample(void);
    void getBioZSample(void);
    bool max30001ReadInfo(void);
    void max30001SetSamplingRate(uint16_t samplingRate);
    void max30001RegRead(uint8_t Reg_address, uint8_t * buff);
    void max30001RegWrite (unsigned char WRITE_ADDRESS, unsigned long data);
    uint8_t max30001GetEcgGain(void);
    uint8_t max30001GetBioZGain(void);

  private:
    void max30001ReadData(int num_samples, uint8_t * readBuffer);
    void max30001Synch(void);
    void max30001FIFO_Rst(void);
};


/*-------------------------------------------------------------------------------------------------------------*/


//////////////////////////////////////////////////////////////////////////////////////////
//
//  Library code for the MAX30001
//
//  Arduino connections:
//
//  |MAX30001 pin label| Pin Function         |Arduino Connection|
//  |----------------- |:--------------------:|-----------------:|
//  | POCI             | Peripheral Out       |  D12             |
//  | PICO             | Peripheral In        |  D11             |
//  | SCLK             | Serial Clock         |  D13             |
//  | CS               | Chip Select          |  D10             |
//  | VCC              | Digital VDD          |  +5V             |
//  | GND              | Digital Gnd          |  Gnd             |
//  | FCLK             | 32K CLOCK            |  -               |
//  | INT1             | Interrupt1           |  D01             |
//  | INT2             | Interrupt2           |  -               |
//
//  This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
/////////////////////////////////////////////////////////////////////////////////////////

void MAX30001::max30001RegWrite(unsigned char WRITE_ADDRESS, unsigned long data)
{
    // now combine the register address and the command into one byte:
    byte dataToSend = (WRITE_ADDRESS << 1) | WREG;

     // take the chip select low to select the device:
    digitalWrite(MAX30001_CS_PIN, LOW);

    delay(2);
    SPI.transfer(dataToSend);
    SPI.transfer(data >> 16);
    SPI.transfer(data >> 8);
    SPI.transfer(data);
    delay(2);

    // take the chip select high to de-select:
    digitalWrite(MAX30001_CS_PIN, HIGH);
}


void MAX30001::max30001SwReset(void)
{
    max30001RegWrite(SW_RST, 0x000000);
    delay(100);
}

void MAX30001::max30001Synch(void)
{
    max30001RegWrite(SYNCH, 0x000000);
}

void MAX30001::max30001FIFO_Rst(void) {
    max30001RegWrite(FIFO_RST, 0x000000);
}

void MAX30001::max30001RegRead(uint8_t Reg_address, uint8_t * buff)
{
    uint8_t spiTxBuff;

    digitalWrite(MAX30001_CS_PIN, LOW);

    spiTxBuff = (Reg_address << 1 ) | RREG;
    SPI.transfer(spiTxBuff);              // Send register location

    for ( int i = 0; i < 3; i++)
    {
       buff[i] = SPI.transfer(0xFF);
    }

    digitalWrite(MAX30001_CS_PIN, HIGH);
}

bool MAX30001::max30001ReadInfo(void)
{
    uint8_t spiTxBuff;
    uint8_t readBuff[4] ;

    digitalWrite(MAX30001_CS_PIN, LOW);

    spiTxBuff = (INFO << 1 ) | RREG;
    SPI.transfer(spiTxBuff);              // Send register location

    for ( int i = 0; i < 3; i++)
    {
       readBuff[i] = SPI.transfer(0xFF);
    }

    digitalWrite(MAX30001_CS_PIN, HIGH);

    if((readBuff[0] & 0xF0) == 0x50 ){

      Serial.println("Max30001 is ready.");
      Serial.print("Rev ID:  ");
      Serial.println((readBuff[0] & 0xF0));

      return true;
    }else{
      Serial.println("Max30001 read info error.\n");
      return false;
    }

    return false;
}

void MAX30001::max30001ReadData(int num_samples, uint8_t * readBuffer)
{
    uint8_t spiTxBuff;
    digitalWrite(MAX30001_CS_PIN, LOW);

    spiTxBuff = (ECG_FIFO_BURST << 1 ) | RREG;
    SPI.transfer(spiTxBuff);                  // Send register location

    for ( int i = 0; i < num_samples*3; ++i)
    {
      readBuffer[i] = SPI.transfer(0x00);
    }

    digitalWrite(MAX30001_CS_PIN, HIGH);
}

void MAX30001::max30001Begin()
{
    max30001SwReset();
    delay(100);
    max30001RegWrite(CNFG_GEN, 0x081007);
    delay(100);
    max30001RegWrite(CNFG_CAL, 0x720000);
    delay(100);
    max30001RegWrite(CNFG_EMUX,0x0B0000);
    delay(100);
    max30001RegWrite(CNFG_ECG, 0x805000);     // d23 - d22 : 10 for 250sps , 00:500 sps
    delay(100);

    max30001RegWrite(CNFG_RTOR1, 0x3FC600);
    max30001Synch();
    delay(100);
}

void MAX30001::max30001BeginRtoRMode()
{
    max30001SwReset();
    delay(100);
    max30001RegWrite(CNFG_GEN, 0x080004);
    delay(100);
    max30001RegWrite(CNFG_CAL, 0x720000);   // 0x700000
    delay(100);
    max30001RegWrite(CNFG_EMUX,0x0B0000);
    delay(100);
    max30001RegWrite(CNFG_ECG, 0x805000);   // d23 - d22 : 10 for 250sps , 00:500 sps
    delay(100);
    max30001RegWrite(CNFG_RTOR1, 0x3FC600);
    delay(100);
    max30001RegWrite(EN_INT, 0x000401);
    delay(100);
    max30001Synch();
    delay(100);
}

//not tested
void MAX30001::max30001SetSamplingRate(uint16_t samplingRate)
{
    uint8_t regBuff[4] = {0};
    max30001RegRead(CNFG_ECG, regBuff);

    switch(samplingRate){
        case SAMPLINGRATE_128:
                regBuff[0] = (regBuff[0] | 0x80 );
            break;

        case SAMPLINGRATE_256:
                regBuff[0] = (regBuff[0] | 0x40 );
            break;

        case SAMPLINGRATE_512:
                regBuff[0] = (regBuff[0] | 0x00 );
            break;

        default :
            Serial.println("Invalid sampling rate, please choose between {128, 256, 512}.");
            break;
    }

    unsigned long cnfgEcg;
    memcpy(&cnfgEcg, regBuff, 4);

    Serial.print("cnfg ECG ");
    Serial.println((cnfgEcg));
    max30001RegWrite(CNFG_ECG, (cnfgEcg >> 8));
}

uint8_t MAX30001::max30001GetEcgGain(void)
{
    uint8_t regReadBuff[4];
    uint8_t ecg_gains[] = {20, 40, 80, 160};
    // uint8_t ecg_gains[4];
    // ecg_gains[0] = 20;
    // ecg_gains[1] = 40;
    // ecg_gains[2] = 80;
    // ecg_gains[3] = 160;
    uint8_t gain_idx = 0;

    max30001RegRead(CNFG_ECG, regReadBuff);
    gain_idx = regReadBuff[0] & 0x03;

    return ecg_gains[gain_idx];
}

uint8_t MAX30001::max30001GetBioZGain(void)
{
    uint8_t regReadBuff[4];
    uint8_t bioz_gains[] = {10, 20, 40, 80};
    uint8_t gain_idx = 0;

    max30001RegRead(CNFG_BIOZ, regReadBuff);
    gain_idx = regReadBuff[0] & 0x03;

    return bioz_gains[gain_idx];
}

void MAX30001::getHRandRR(void)
{
    uint8_t regReadBuff[4];
    max30001RegRead(RTOR, regReadBuff);

    unsigned long RTOR_msb = (unsigned long) (regReadBuff[0]);
    unsigned char RTOR_lsb = (unsigned char) (regReadBuff[1]);
    unsigned long rtor = (RTOR_msb << 8 | RTOR_lsb);
    rtor = ((rtor >> 2) & 0x3FFF) ;

    float hr =  60 /((float)rtor*0.0078125);
    heartRate = (unsigned int)hr;

    unsigned int RR = (unsigned int)rtor* (7.8125) ;  //8ms
    RRinterval = RR;
}

void MAX30001::getEcgSample(void)
{
    signed long ecg_reading;
    uint8_t regReadBuff[4];
    max30001RegRead(ECG_FIFO, regReadBuff);

    unsigned long data0 = (unsigned long) (regReadBuff[0]);
    data0 = data0 << 16;
    unsigned long data1 = (unsigned long) (regReadBuff[1]);
    data1 = data1 << 8;
    unsigned long data2 = (unsigned long) (regReadBuff[2]);
    data2 = data2 & 0xC0;                                   // Extract ECG_FIFO[7:6]
    unsigned long data = (unsigned long) (data0 | data1 | data2);
    ecgData.value = (signed long) ( data >> 6);             // ECG ADC data is ECG_FIFO[23:6]

    ecgData.e_tag = (regReadBuff[2] & 0x38) >> 3;           // Align ETAG[2:0] to 8-bit val. 0b00XXX000 -> 0b00000XXX
    ecgData.p_tag = regReadBuff[2] & 0x07;                  // PTAG[2:0] is already aligned. 0b00000XXX

    if(ecgData.e_tag & 0x7){                                // FIFO Overflow Exception: ETAG[2:0] = 111, send RST
        max30001FIFO_Rst();
    }
}

void MAX30001::getBioZSample(void)
{
    uint8_t regReadBuff[4];
    max30001RegRead(BIOZ_FIFO, regReadBuff);

    unsigned long data0 = (unsigned long) (regReadBuff[0]);
    data0 = data0 << 16;
    unsigned long data1 = (unsigned long) (regReadBuff[1]);
    data1 = data1 << 8;
    unsigned long data2 = (unsigned long) (regReadBuff[2]);
    data2 = data2 & 0xF0;                                   // Extract BIOZ_FIFO[7:4]
    unsigned long data = (unsigned long) (data0 | data1 | data2);
    biozData.value = (signed long) (data >> 4);             // BioZ ADC data is BIOZ_FIFO[23:4]

    biozData.b_tag = regReadBuff[2] & 0x07;                 // BTAG[2:0] is BIOZ_FIFO[2:0]
}






/*-------------------------------------------------------------------------------------------------------------*/


//////////////////////////////////////////////////////////////////////////////////////////
//
//    MAX30001 ECG Datastream Example
//
//    This example plots live ECG data via the Arduino Plotter.
//
//    Arduino connections:
//
//  |MAX30001 pin label| Pin Function         |Arduino Connection|
//  |----------------- |:--------------------:|-----------------:|
//  | POCI             | Peripheral Out       |  D12             |
//  | PICO             | Peripheral In        |  D11             |
//  | SCLK             | Serial Clock         |  D13             |
//  | CS               | Chip Select          |  D10             |
//  | VCC              | Digital VDD          |  +5V             |
//  | GND              | Digital Gnd          |  Gnd             |
//  | FCLK             | 32K CLOCK            |  -               |
//  | INT1             | Interrupt1           |  D01             |
//  | INT2             | Interrupt2           |  -               |
//
//    This software is licensed under the MIT License(http://opensource.org/licenses/MIT).
//
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
//   NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
//   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//
/////////////////////////////////////////////////////////////////////////////////////////


MAX30001 myMAXChip;
uint8_t cur_ecg_gain  = 20;

void printEcgSample()
{
    long raw_ecg_adc = myMAXChip.ecgData.value;
    double formatted_ecg_data = (raw_ecg_adc * 1000) / (2^17 * cur_ecg_gain);  // See datasheet for equation
    Serial.print("ECG reading (mV): ");
    Serial.println(formatted_ecg_data);
    Serial.print("ETAG[2:0]: ");
    Serial.println(myMAXChip.ecgData.e_tag);
    Serial.print("PTAG[2:0]: ");
    Serial.println(myMAXChip.ecgData.p_tag);
}

void setup()
{
    Serial.begin(115200);
    pinMode(MAX30001_CS_PIN, OUTPUT);
    digitalWrite(MAX30001_CS_PIN, HIGH); // Disable SPI communication

    SPI.begin();
    SPI.beginTransaction(SPISettings(12000000, MSBFIRST, SPI_MODE0));
    // SPI.setBitOrder(MSBFIRST);          // See datasheet for SPI communication settings
    // SPI.setDataMode(SPI_MODE0);

    bool ret = myMAXChip.max30001ReadInfo();
    if(ret == true){
      Serial.println("Max30001 read ID success.");
    }
    else{
      while(ret == false){
        // Stay here until the issue is fixed.
        ret = myMAXChip.max30001ReadInfo();
        Serial.println("Failed to read ID, please make sure all the pins are properly connected.");
        delay(10000);
      }
    }

    delay(10000);

    Serial.println("Running initialization...");
    myMAXChip.max30001Begin();   // Run initialization

    cur_ecg_gain = myMAXChip.max30001GetEcgGain();
}

void loop()
{
    myMAXChip.getEcgSample();   // Read the ECG FIFO store ADC reading to myMAXChip.ecgData structure
    long raw_ecg_adc = myMAXChip.ecgData.value;
    double formatted_ecg_data = (raw_ecg_adc * 1000) / (2^17 * cur_ecg_gain);  // Vref = 1000 mV by default

    Serial.print("ECG reading (mV): ");
    Serial.println(formatted_ecg_data);
    Serial.print("ETAG[2:0]: ");
    Serial.println(myMAXChip.ecgData.e_tag);
    Serial.print("PTAG[2:0]: ");
    Serial.println(myMAXChip.ecgData.p_tag);

    // Serial.println(formatted_ecg_data);
    // printECGSample();
    delay(1000);
}