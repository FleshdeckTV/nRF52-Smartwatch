

#ifndef MAX17055_h
#define MAX17055_h

#include <Arduino.h>

class MAX17055 {
    public:
        enum RegisterAddresses {
            REG_STATUS                  = 0x00,
            REG_VALRTTH                 = 0x01,
            REG_TALRTTH                 = 0x02,
            REG_SALRTTH                 = 0x03,
            REG_AT_RATE                 = 0x04,
            REG_REMAINING_CAPACITY      = 0x05,
            REG_STATE_OF_CHARGE         = 0x06,
            REG_TEMPERATURE             = 0x08,
            REG_VOLTAGE                 = 0x09,
            REG_CURRENT                 = 0x0a,
            REG_AVERAGE_CURRENT         = 0x0b,
            REG_MIXCAP                  = 0x0f,
            REG_FULL_CHARGE_CAPACITY    = 0x10,
            REG_TIME_TO_EMPTY           = 0x11,
            REG_QR_TABLE00              = 0x12,
            REG_CONFIG                  = 0x1D,
            REG_AVERAGE_TEMPERATURE     = 0x16,
            REG_CYCLE_COUNT             = 0x17,
            REG_DESIGN_CAPACITY         = 0x18,
            REG_AVERAGE_VOLTAGE         = 0x19,
            REG_MAX_MIN_TEMP            = 0x1a,
            REG_MAX_MIN_VOLT            = 0x1b,
            REG_MAX_MIN_CURR            = 0x1c,
            REG_CHARGE_TERM_CURRENT     = 0x1e,
            REG_TIME_TO_FULL            = 0x20,
            REG_DEVICE_NAME             = 0x21,
            REG_QR_TABLE10              = 0x22,
            REG_FULLCAPNOM              = 0x23,
            REG_LEARNCFG                = 0x28,
            REG_QR_TABLE20              = 0x32,
            REG_RCOMP0                  = 0x38,
            REG_TEMPCO                  = 0x39,
            REG_EMPTY_VOLTAGE           = 0x3a,
            REG_FSTAT                   = 0x3d,
            REG_TIMER                   = 0x3e,
            REG_QR_TABLE30              = 0x42,
            REG_DQACC                   = 0x45,
            REG_DPACC                   = 0x46,
            REG_VFSOC0                  = 0x48,
            REG_COMMAND                 = 0x60,
            REG_LOCK1                   = 0x62,
            REG_LOCK2                   = 0x63,
            REG_OCV_TABLE_START         = 0x80,
            REG_STATUS2                 = 0xb0,
            REG_IALRTTH                 = 0xb4,
            REG_HIBCFG                  = 0xba,
            REG_CONFIG2                 = 0xbb,
            REG_TIMERH                  = 0xbe,
            REG_MODELCFG                = 0xdb,
            REG_VFSOC                   = 0xff,
        };

        enum StatFlags {
            STATUS_POR                  = BIT(1),
            STATUS_IMN                  = BIT(2),
            STATUS_BST                  = BIT(3),
            STATUS_IMX                  = BIT(6),
            STATUS_VMN                  = BIT(8),
            STATUS_TMN                  = BIT(9),
            STATUS_SMN                  = BIT(10),
            STATUS_VMX                  = BIT(12),
            STATUS_TMX                  = BIT(13),
            STATUS_SMX                  = BIT(14),
        };

        enum FStatFlags {
            // Data Not Ready
            FSTAT_DNR                   = 0x0001,
            // Fully Qualified = Battery Full
            FSTAT_FQ                    = 0x0080,
            // Empty detection
            FSTAT_EDET                  = 0x0100
        };

        enum Commands {
            SOFT_WAKE_CMD_CLEAR         = 0x0000,
            SOFT_WAKE_CMD_WAKE          = 0x0090,
            HIBERNATE_CMD_CLEAR         = 0x0000,
        };

        enum Model {
            LithiumPolymer              = 0x0000,
            LifePo4                     = 0x0060
        };

        MAX17055(void);
        void init();
        void setRSense(float rSense);
        float getRSense();
        void  setCapacity(uint16_t batteryCapacity);
        void setBatteryModel(uint16_t model);
        uint16_t getCapacity();
        uint16_t getRemainingCapacity();
        float getStateOfCharge();
        float getCellVoltage();
        void setVEmpty(float vEmpty);
        void setVRecovery(float vRecovery);
        float getTemperature();
        float getCurrent();
        float getAverageCurrent();
        uint16_t getCycleCount();
        uint16_t getTimeToEmpty();
        boolean isBatteryFullyCharged();
        boolean isDataReady();
        boolean isBatteryEmpty();
        boolean isBatteryDischarging();

    private:
        float rSense = 0.01; //default internal resist sensor
        uint16_t designCapacity = 2500;
        uint8_t i2cAddress = 0x36;
        float capacityLSB;
        float voltageLSB = 0.078125;
        float currentLSB = 1.5625;

        void setLSB();
        uint16_t readReg16Bit(uint8_t reg);
        void writeReg16Bit(uint8_t reg, uint16_t value);
        void writeAndVerifyReg16Bit(uint8_t reg, uint16_t value);

        int16_t convertTwosComplement(uint16_t value);
};

#endif