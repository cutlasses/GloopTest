#pragma once

#include <cstdint>

#include "daisy_seed.h"
#include "HardwareComponents.h"
#include "LooperTypes.h"

#if defined(BOARD_V1_3) || defined(BOARD_V1_4) || defined(BOARD_V1_4_1)
#define CV_PRESENT // board has CV capabilities
#endif

// abstract the differences between the different board layouts

// BOARD_V1_3 has a multiplexor to add CV inputs
// BOARD_V1_4 uses Daisy Seed DFM2

namespace Gloop
{
    struct CVData
    {
        bool cv_trigger = false;               
        float cv_calibrated = 0.0f; // cv calibrated and scaled with mode
        int32_t cv_VOct = 0;
    };

    class CalibrationData
    {
        float offset_0V = 0.0f;
        float offset_VOct = 0.0f;
        float scale_VOct = 1.0f;

    public: 

        CalibrationData() = default;
        CalibrationData(float value_0V, float value_1V, float value_3V);

        float calibrated_read(float v) const;
        int32_t calibrate_read_VOct(float v) const;

#ifdef DEBUG_OUTPUT
        void print() const;
#endif
    };

    namespace HWPlatform
    {
        void initialise_hw();
        void initialise_screen();

        void calibrate(const float* values_0V, const float* values_1V, const float* values_3V, int num_values);

        void set_calibration_data(const void* data, size_t size);
        void fill_calibration_data(void* data, size_t size);

        void delay_ms(uint32_t ms);
        uint32_t time_now_ms();

        SoundEngine::SampleType* buffer_data_1();

        float read_pot(int pot_index);
        float read_cv(int cv_index);
        CVData read_cv_scaled_and_calibrated(int cv_index, CV_CONVERSION_MODE mode);
        bool read_trigger();

        Switch& menu_switch();
        Switch& record_switch();
        Switch& clear_switch();

        Encoder& encoder(int encoder_index);
        Led& led(int led_index);

        Screen& screen();

        void enable_irq();
        void disable_irq();
    }

} // namespace Gloop

constexpr size_t calibration_data_size = sizeof(Gloop::CalibrationData) * Gloop::NUM_CV_INS;