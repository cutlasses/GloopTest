#pragma once

#include "daisy_seed.h"

class i2c_bus
{
    daisy::I2CHandle            m_i2c;

public:

    i2c_bus()
    {
        daisy::I2CHandle::Config i2c_config;
        i2c_config.periph           = daisy::I2CHandle::Config::Peripheral::I2C_1;
        i2c_config.pin_config.scl   = daisy::seed::D11;
        i2c_config.pin_config.sda   = daisy::seed::D12;
        i2c_config.mode             = daisy::I2CHandle::Config::Mode::I2C_MASTER;
        i2c_config.speed            = daisy::I2CHandle::Config::Speed::I2C_100KHZ;

        m_i2c.Init(i2c_config);
    }

    bool read_data(uint8_t i2c_address, uint8_t* data, uint16_t size, uint32_t timeout)
    {
        const daisy::I2CHandle::Result res = m_i2c.ReceiveBlocking( i2c_address,
                                                                    data,
                                                                    size,
                                                                    timeout );

        return res == daisy::I2CHandle::Result::OK;        
    }

    bool write_data(uint8_t i2c_address, uint8_t* data, uint16_t size, uint32_t timeout)
    {
        const daisy::I2CHandle::Result res = m_i2c.TransmitBlocking( i2c_address,
                                                                    data,
                                                                    size,
                                                                    timeout );

        return res == daisy::I2CHandle::Result::OK;        
    }
};