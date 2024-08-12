#pragma once

#include <stddef.h>
#include <cstdint>

// sh1106
template <typename Transport>
class OLED_1_3_inch
{
    static constexpr size_t width = 128;
    static constexpr size_t height = 64;
    static constexpr uint8_t display_offset = 0x00;
    static constexpr uint8_t start_line_addr =  0x40;

  public:
    struct Config
    {
        typename Transport::Config transport_config;
    };

    void Init(Config config)
    {
        transport_.Init(config.transport_config);

        // Init routine...

        // Display Off
        transport_.SendCommand(0xaE);

        // Display Clock Divide Ratio
        transport_.SendCommand(0xD5);
        transport_.SendCommand(0x80);
        // Multiplex Ratio
        transport_.SendCommand(0xA8);
        transport_.SendCommand(0x3F);
        // COM Pins
        transport_.SendCommand(0xDA);
        transport_.SendCommand(0x12);

        // Display Offset
        transport_.SendCommand(0xD3);
        transport_.SendCommand(display_offset);
        // Start Line Address
        transport_.SendCommand(start_line_addr);
        // Normal Display
        transport_.SendCommand(0xA6);
        // All On Resume
        transport_.SendCommand(0xA4);
        // Charge Pump
        transport_.SendCommand(0x8D);
        transport_.SendCommand(0x14);
        // Set Segment Remap
        transport_.SendCommand(0xA1);
        // COM Output Scan Direction
        transport_.SendCommand(0xC8);
        // Contrast Control
        transport_.SendCommand(0x81);
        transport_.SendCommand(0x8F);
        // Pre Charge
        transport_.SendCommand(0xD9);
        transport_.SendCommand(0x25);
        // VCOM Detect
        transport_.SendCommand(0xDB);
        transport_.SendCommand(0x34);


        // Display On
        transport_.SendCommand(0xAF); //--turn on oled panel
    };

    size_t Width() const { return width; };
    size_t Height() const { return height; };

    void DrawPixel(uint_fast8_t x, uint_fast8_t y, bool on)
    {
        if(x >= width || y >= height)
            return;
        if(on)
            buffer_[x + (y / 8) * width] |= (1 << (y % 8));
        else
            buffer_[x + (y / 8) * width] &= ~(1 << (y % 8));
    }

    void Fill(bool on)
    {
        for(size_t i = 0; i < sizeof(buffer_); i++)
        {
            buffer_[i] = on ? 0xff : 0x00;
        }
    };

    /**
     * Update the display 
    */
    void Update()
    {
        uint8_t i;

        for(i = 0; i < (height / 8); i++)
        {
            transport_.SendCommand(0xB0 + i);

            transport_.SendCommand(0x02); // low column start address
            transport_.SendCommand(0x10); // high column start address

            transport_.SendData(&buffer_[width * i], width);
        }
    };

  private:
    Transport transport_;
    uint8_t   buffer_[width * height / 8];
};
