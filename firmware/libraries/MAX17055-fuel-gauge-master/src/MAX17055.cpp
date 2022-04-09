#include "MAX17055.h"
#include <Wire.h>

MAX17055::MAX17055() {
    setLSB();
}

void MAX17055::init() {
    setLSB();

    // Clear power on reset bit
    uint16_t statusRaw = readReg16Bit(REG_STATUS);
    Serial.printf("POR: %d\n", statusRaw & STATUS_POR);
    if (statusRaw & STATUS_POR == 1) {
        while (readReg16Bit(REG_FSTAT) & FSTAT_DNR) {
            Serial.print("x");
            delay(10);
        }
        // Store Hibernate Configuration
        //uint16_t hibCfg = readReg16Bit(REG_HIBCFG);
        // Exit Hibernate mode
        writeReg16Bit(REG_COMMAND, SOFT_WAKE_CMD_WAKE);
        writeReg16Bit(REG_HIBCFG, HIBERNATE_CMD_CLEAR);
        writeReg16Bit(REG_COMMAND, SOFT_WAKE_CMD_CLEAR);

        writeReg16Bit(REG_DESIGN_CAPACITY, 0x1388);
        writeReg16Bit(REG_DQACC , 0x1388 / 16) ; //Write dQAcc
        writeReg16Bit(REG_CHARGE_TERM_CURRENT , 0x0640) ; // Write IchgTerm
        writeReg16Bit(REG_EMPTY_VOLTAGE , 0xA561) ; // Write VEmpty
        writeReg16Bit(REG_LEARNCFG , 0x4486) ;// (Optional in the INI) Write LearnCFG

        //writeReg16Bit(REG_DPACC , dQAcc*44138/DesignCap); //Write dPAcc
        writeReg16Bit(REG_MODELCFG , 0x8000) ; // Write ModelCFG
        while (readReg16Bit(REG_MODELCFG) & 0x8000) {
            Serial.print(".");
            delay(10);
        } 

        //writeReg16Bit(REG_HIBCFG, hibCfg);
    }
    uint16_t status = readReg16Bit(0x00); //Read Status
    writeAndVerifyReg16Bit (REG_STATUS, status & 0xFFFD); 



    uint16_t model = readReg16Bit(REG_MODELCFG);
    Serial.print("Model: ");
    Serial.println(model, HEX);
    //writeReg16Bit(REG_MODELCFG, 0x8000);
}

uint16_t MAX17055::getCycleCount() {
    return readReg16Bit(REG_CYCLE_COUNT);
}

void  MAX17055::setCapacity(uint16_t batteryCapacity) {
   designCapacity = designCapacity;
   writeReg16Bit(REG_DESIGN_CAPACITY, designCapacity / capacityLSB);
}

void MAX17055::setBatteryModel(uint16_t model) {
    writeReg16Bit(REG_MODELCFG, 0x8000 | model);
}

void MAX17055::setVEmpty(float vEmpty) {
    uint16_t emptyVoltageRaw = readReg16Bit(REG_EMPTY_VOLTAGE);
    uint8_t vRecoveryRaw = emptyVoltageRaw & 0xFF;
    uint16_t vEmptyRaw = ((uint16_t)(vEmpty * 1000 / 10) << 7) | vRecoveryRaw;
    writeReg16Bit(REG_EMPTY_VOLTAGE, vEmptyRaw);
}
void MAX17055::setVRecovery(float vRecovery) {
    uint16_t emptyVoltageRaw = readReg16Bit(REG_EMPTY_VOLTAGE);
    uint16_t vEmptyRaw = (emptyVoltageRaw & 0xFF00) | (uint8_t)(vRecovery * 1000 / 40);
    writeReg16Bit(REG_EMPTY_VOLTAGE, vEmptyRaw);
}

uint16_t MAX17055::getCapacity() {
   uint16_t rawCapacity = readReg16Bit(REG_DESIGN_CAPACITY);
   return rawCapacity * capacityLSB;
} 

uint16_t MAX17055::getRemainingCapacity() {
    uint16_t rawRemainingCapacity = readReg16Bit(REG_REMAINING_CAPACITY);
    return rawRemainingCapacity * capacityLSB;
}

float MAX17055::getStateOfCharge() {
    uint16_t rawStateOfCharge = readReg16Bit(REG_STATE_OF_CHARGE);
    Serial.println(rawStateOfCharge, BIN);
    return rawStateOfCharge / 256.0;
}

float MAX17055::getCellVoltage() {
    uint16_t rawVoltage = readReg16Bit(REG_VOLTAGE);
    return rawVoltage * voltageLSB;
}

float MAX17055::getCurrent() {
    int16_t rawCurrent = readReg16Bit(REG_CURRENT);
    return rawCurrent * currentLSB;
}

float MAX17055::getAverageCurrent() {
    int16_t rawAverageCurrent = readReg16Bit(REG_AVERAGE_CURRENT);
    return rawAverageCurrent * currentLSB;
}

float MAX17055::getTemperature() {
    uint16_t rawTemperature = readReg16Bit(REG_TEMPERATURE);
    return rawTemperature / 256.0;
}



boolean MAX17055::isBatteryFullyCharged() {
    uint16_t fStatRaw = readReg16Bit(REG_FSTAT);
    return fStatRaw & FStatFlags::FSTAT_FQ;
}

boolean MAX17055::isDataReady() {
    uint16_t fStatRaw = readReg16Bit(REG_FSTAT);
    return !(fStatRaw & FStatFlags::FSTAT_DNR);
}

boolean MAX17055::isBatteryEmpty() {
    uint16_t fStatRaw = readReg16Bit(REG_FSTAT);
    return (fStatRaw & FStatFlags::FSTAT_EDET);
}

boolean MAX17055::isBatteryDischarging() {
    uint16_t averageCurrentRaw = readReg16Bit(REG_AVERAGE_CURRENT);
    return (averageCurrentRaw >> 15);
}

void MAX17055::setRSense(float senseResitor) {
    rSense = senseResitor;
    setLSB();
}
float MAX17055::getRSense() {
    return rSense;
}

uint16_t MAX17055::readReg16Bit(uint8_t reg) {
  uint16_t value = 0;  
  Wire.beginTransmission(i2cAddress); 
  Wire.write(reg);
  uint8_t last_status = Wire.endTransmission(false);
  
  Wire.requestFrom(i2cAddress, (uint8_t) 2); 
  value  = Wire.read();
  value |= (uint16_t)Wire.read() << 8;      // value low byte
  return value;

}
void MAX17055::writeReg16Bit(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(i2cAddress);
  Wire.write(reg);
  Wire.write( value       & 0xFF); // value low byte
  Wire.write((value >> 8) & 0xFF); // value high byte
  uint8_t last_status = Wire.endTransmission();
}
void MAX17055::writeAndVerifyReg16Bit(uint8_t reg, uint16_t value) {
    int attempts=0;
    uint16_t registerValueRead;
    do {
        writeReg16Bit (reg, value);
        delay(1);
        registerValueRead = readReg16Bit (reg);
    } while (value != registerValueRead && attempts++<3);
}
void MAX17055::setLSB() {
    capacityLSB = (5.0 / rSense);
    currentLSB = 1.5625 / rSense;
}

int16_t MAX17055::convertTwosComplement(uint16_t value) {
    return (0x8000 & value ? (int)(0x7FFF & value)-0x8000 : value);
}