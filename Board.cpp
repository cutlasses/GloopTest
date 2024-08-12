#include "Board.h"
#include "EncoderParams.h"
#include "LooperTypes.h"
#include "Util.h"

extern daisy::DaisySeed hw;

namespace Gloop
{    
#if defined(BOARD_V1_4_1)
    constexpr daisy::Pin screen_dc_pin      = daisy::seed::D6;
    constexpr daisy::Pin screen_reset_pin   = daisy::seed::D27;

    constexpr daisy::Pin enc1_a_pin     = daisy::seed::D30;
    constexpr daisy::Pin enc1_b_pin     = daisy::seed::D29;
    constexpr daisy::Pin enc1_sw_pin    = daisy::seed::D24;
    constexpr daisy::Pin enc2_a_pin     = daisy::seed::D2;
    constexpr daisy::Pin enc2_b_pin     = daisy::seed::D3;
    constexpr daisy::Pin enc2_sw_pin    = daisy::seed::D1;
    constexpr daisy::Pin enc3_a_pin     = daisy::seed::D5;
    constexpr daisy::Pin enc3_b_pin     = daisy::seed::D4;
    constexpr daisy::Pin enc3_sw_pin    = daisy::seed::D0;
    constexpr daisy::Pin enc4_a_pin     = daisy::seed::D31;
    constexpr daisy::Pin enc4_b_pin     = daisy::seed::D22;
    constexpr daisy::Pin enc4_sw_pin    = daisy::seed::D23;

    constexpr daisy::Pin sw1_pin = daisy::seed::D28;
    constexpr daisy::Pin sw2_pin = daisy::seed::D16;
    constexpr daisy::Pin sw3_pin = daisy::seed::D25;

    constexpr daisy::Pin led_pin_1 = daisy::seed::D13;
    constexpr daisy::Pin led_pin_2 = daisy::seed::D14;
    constexpr daisy::Pin led_pin_3 = daisy::seed::D15;
    constexpr daisy::Pin led_pin_4 = daisy::seed::D21;

    constexpr daisy::Pin trig_pin = daisy::seed::D26;

    constexpr daisy::Pin multiplex_in_pin       = daisy::seed::A4;
    constexpr daisy::Pin multiplex_select_pin_0 = daisy::seed::D18;
    constexpr daisy::Pin multiplex_select_pin_1 = daisy::seed::D17;
    constexpr daisy::Pin multiplex_select_pin_2 = daisy::seed::D20;    

#elif defined(BOARD_V1_4)
    constexpr daisy::Pin screen_dc_pin      = daisy::seed::D13;
    constexpr daisy::Pin screen_reset_pin   = daisy::seed::D14;

    constexpr daisy::Pin enc1_a_pin     = daisy::seed::D15;
    constexpr daisy::Pin enc1_b_pin     = daisy::seed::D23;
    constexpr daisy::Pin enc1_sw_pin    = daisy::seed::D21;
    constexpr daisy::Pin enc2_a_pin     = daisy::seed::D31;
    constexpr daisy::Pin enc2_b_pin     = daisy::seed::D16;
    constexpr daisy::Pin enc2_sw_pin    = daisy::seed::D22;
    constexpr daisy::Pin enc3_a_pin     = daisy::seed::D6;
    constexpr daisy::Pin enc3_b_pin     = daisy::seed::D28;
    constexpr daisy::Pin enc3_sw_pin    = daisy::seed::D26;
    constexpr daisy::Pin enc4_a_pin     = daisy::seed::D25;
    constexpr daisy::Pin enc4_b_pin     = daisy::seed::D1;
    constexpr daisy::Pin enc4_sw_pin    = daisy::seed::D24;

    constexpr daisy::Pin sw1_pin = daisy::seed::D27;
    constexpr daisy::Pin sw2_pin = daisy::seed::D2;
    constexpr daisy::Pin sw3_pin = daisy::seed::D0;

    constexpr daisy::Pin led_pin_1 = daisy::seed::D3;
    constexpr daisy::Pin led_pin_2 = daisy::seed::D4;
    constexpr daisy::Pin led_pin_3 = daisy::seed::D29;
    constexpr daisy::Pin led_pin_4 = daisy::seed::D30;

    constexpr daisy::Pin trig_pin = daisy::seed::D5;

    constexpr daisy::Pin multiplex_in_pin       = daisy::seed::A4;
    constexpr daisy::Pin multiplex_select_pin_0 = daisy::seed::D18;
    constexpr daisy::Pin multiplex_select_pin_1 = daisy::seed::D17;
    constexpr daisy::Pin multiplex_select_pin_2 = daisy::seed::D20;    

#elif defined(BOARD_V1_3)
    constexpr daisy::Pin screen_dc_pin      = daisy::seed::D13;
    constexpr daisy::Pin screen_reset_pin   = daisy::seed::D14;

    constexpr daisy::Pin enc1_a_pin         = daisy::seed::D30;
    constexpr daisy::Pin enc1_b_pin         = daisy::seed::D29;
    constexpr daisy::Pin enc1_sw_pin        = daisy::seed::D0;
    constexpr daisy::Pin enc2_a_pin         = daisy::seed::D26;
    constexpr daisy::Pin enc2_b_pin         = daisy::seed::D25;
    constexpr daisy::Pin enc2_sw_pin        = daisy::seed::D24;
    constexpr daisy::Pin enc3_a_pin         = daisy::seed::D22;
    constexpr daisy::Pin enc3_b_pin         = daisy::seed::D21;
    constexpr daisy::Pin enc3_sw_pin        = daisy::seed::D20;
    constexpr daisy::Pin enc4_a_pin         = daisy::seed::D18;
    constexpr daisy::Pin enc4_b_pin         = daisy::seed::D17;
    constexpr daisy::Pin enc4_sw_pin        = daisy::seed::D16;

    constexpr daisy::Pin mux_pin        = daisy::seed::A0;

    constexpr daisy::Pin sw1_pin        = daisy::seed::D2;
    constexpr daisy::Pin sw2_pin        = daisy::seed::D6;
    constexpr daisy::Pin sw3_pin        = daisy::seed::D1;

    constexpr daisy::Pin led_pin_1      = daisy::seed::D28;
    constexpr daisy::Pin led_pin_2      = daisy::seed::D27;
    constexpr daisy::Pin led_pin_3      = daisy::seed::D23;
    constexpr daisy::Pin led_pin_4      = daisy::seed::D19;

    constexpr daisy::Pin trig_pin       = daisy::seed::D9;

    constexpr daisy::Pin multiplex_in_pin       = daisy::seed::A0;
    constexpr daisy::Pin multiplex_select_pin_0 = daisy::seed::D5;
    constexpr daisy::Pin multiplex_select_pin_1 = daisy::seed::D4;
    constexpr daisy::Pin multiplex_select_pin_2 = daisy::seed::D3;     

#else // !BOARD_V1_3

#ifdef CV_PRESENT // board has CV capabilities
static_assert(false);
#endif

    constexpr daisy::Pin screen_dc_pin = daisy::seed::D13;
    constexpr daisy::Pin screen_reset_pin = daisy::seed::D14;

    constexpr daisy::Pin enc1_a_pin = daisy::seed::D30;
    constexpr daisy::Pin enc1_b_pin = daisy::seed::D29;
    constexpr daisy::Pin enc1_sw_pin = daisy::seed::D0;
    constexpr daisy::Pin enc2_a_pin = daisy::seed::D28;
    constexpr daisy::Pin enc2_b_pin = daisy::seed::D27;
    constexpr daisy::Pin enc2_sw_pin = daisy::seed::D26;
    constexpr daisy::Pin enc3_a_pin = daisy::seed::D25;
    constexpr daisy::Pin enc3_b_pin = daisy::seed::D24;
    constexpr daisy::Pin enc3_sw_pin = daisy::seed::D23;
    constexpr daisy::Pin enc4_a_pin = daisy::seed::D22;
    constexpr daisy::Pin enc4_b_pin = daisy::seed::D21;
    constexpr daisy::Pin enc4_sw_pin = daisy::seed::D20;

    constexpr daisy::Pin pot1_pin = daisy::seed::A4;
    constexpr daisy::Pin pot2_pin = daisy::seed::A3;
    constexpr daisy::Pin pot3_pin = daisy::seed::A2;
    constexpr daisy::Pin pot4_pin = daisy::seed::A1;

    constexpr daisy::Pin sw1_pin = daisy::seed::D5;
    constexpr daisy::Pin sw2_pin = daisy::seed::D15;
    constexpr daisy::Pin sw3_pin = daisy::seed::D1;

    constexpr daisy::Pin led_pin_1 = daisy::seed::D2;
    constexpr daisy::Pin led_pin_2 = daisy::seed::D3;
    constexpr daisy::Pin led_pin_3 = daisy::seed::D4;
    constexpr daisy::Pin led_pin_4 = daisy::seed::D6;
#endif // !BOARD_V1_3

// global data
static SoundEngine::SampleType DSY_SDRAM_BSS g_buffer_data_1[LOOPER_BUFFER_SIZE];

static Encoder g_encoders[NUM_ENCODERS];

static constexpr daisy::Pin led_pins[NUM_PLAY_HEADS] = { led_pin_1, led_pin_2, led_pin_3, led_pin_4 };
static Led g_leds[NUM_PLAY_HEADS];

static Switch g_record_switch;
static Switch g_clear_switch;
static Switch g_menu_switch;

static Screen g_screen;

namespace HWPlatform
{
    void delay_ms(uint32_t ms)
    {
        hw.DelayMs(ms);
    }

    uint32_t time_now_ms()
    {
        return daisy::System::GetNow();
    }

    SoundEngine::SampleType* buffer_data_1()
    {
        return &(g_buffer_data_1[0]);
    }

    Switch& menu_switch()
    {
        return g_menu_switch;
    }

    Switch& record_switch()
    {
        return g_record_switch;
    }

    Switch& clear_switch()
    {
        return g_clear_switch;
    }

    Encoder& encoder(int encoder_index)
    {
        return g_encoders[encoder_index];
    }

    Led& led(int led_index)
    {
        return g_leds[led_index];
    }

    Screen& screen()
    {
        return g_screen;
    }

    void enable_irq()
    {
        __enable_irq();
    }

    void disable_irq()
    {
        __disable_irq();
    }
}

#if defined(BOARD_V1_3) || defined(BOARD_V1_4) || defined(BOARD_V1_4_1)

    static daisy::GPIO trig_pin_gpio;

    CalibrationData local_calibration_data[NUM_CV_INS]; // cache locally (vs in Flash)

    CalibrationData::CalibrationData(float value_0V, float value_1V, float value_3V)
    {
        offset_0V           = 0.5f - value_0V;

        const float delta   = value_3V - value_1V;
        scale_VOct          = 24.0f / delta;
        offset_VOct         = 12.0f - (scale_VOct * value_1V);
    }

    float CalibrationData::calibrated_read(float v) const
    {
        return v + offset_0V;
    }

    int32_t CalibrationData::calibrate_read_VOct(float v) const
    {
        return Utils::round_to_int<int32_t>(offset_VOct + (scale_VOct * v));
    }

#ifdef DEBUG_OUTPUT
        void CalibrationData::print() const
        {
            DEBUG_TEXT_LINE(" (offset0V:%d, offsetVOct:%d, scale:%d)",  static_cast<int32_t>(offset_0V)*100,
                                                                        static_cast<int32_t>(offset_VOct)*100,
                                                                        static_cast<int32_t>(scale_VOct)*100);
        }
#endif

    ///////////////////////

    namespace HWPlatform
    {
        void initialise_hw()
        {
            g_encoders[0].Init(enc1_b_pin, enc1_a_pin, enc1_sw_pin);
            g_encoders[1].Init(enc2_b_pin, enc2_a_pin, enc2_sw_pin);
            g_encoders[2].Init(enc3_b_pin, enc3_a_pin, enc3_sw_pin);
            g_encoders[3].Init(enc4_b_pin, enc4_a_pin, enc4_sw_pin);

            for( int p = 0; p < NUM_PLAY_HEADS; ++p )
            {
                g_leds[p].Init( led_pins[p], daisy::GPIO::Mode::OUTPUT, daisy::GPIO::Pull::NOPULL );
            }

            daisy::AdcChannelConfig adc_config;
            adc_config.InitMux(multiplex_in_pin, 8, multiplex_select_pin_0, multiplex_select_pin_1, multiplex_select_pin_2);

            hw.adc.Init(&adc_config, 1);
            hw.adc.Start();       

            trig_pin_gpio.Init(trig_pin, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::NOPULL, daisy::GPIO::Speed::LOW); 
        
            g_record_switch.Init(sw1_pin);
            g_clear_switch.Init(sw2_pin);
            g_menu_switch.Init(sw3_pin);        
        }

        void initialise_screen()
        {
            Screen::Config display_config;
            display_config.driver_config.transport_config.pin_config.dc    = screen_dc_pin;
            display_config.driver_config.transport_config.pin_config.reset = screen_reset_pin;

            g_screen.Init(display_config);         
        }        

        void calibrate(const float* values_0V, const float* values_1V, const float* values_3V, int num_values)
        {
            ASSERT(num_values==NUM_CV_INS);
            for(int v = 0; v < num_values; ++v)
            {
                local_calibration_data[v] = CalibrationData(values_0V[v], values_1V[v], values_3V[v]);
            }
        }

        void set_calibration_data(const void* data, size_t size)
        {
            if(size == calibration_data_size)
            {
                DEBUG_TEXT("set_calibration_data ");
                const CalibrationData* calibration_data = reinterpret_cast<const CalibrationData*>(data);

                for(int cvi = 0; cvi < NUM_CV_INS; ++cvi)
                {
                    const CalibrationData& cd = calibration_data[cvi];                
                    local_calibration_data[cvi] = cd;

    #ifdef DEBUG_OUTPUT
                    cd.print();
    #endif      
                }
                DEBUG_TEXT("\n");
            }
            else
            {
                ASSERT(false);
            }
        }

        void fill_calibration_data(void* data, size_t size)
        {
            if(size == calibration_data_size)
            {
                DEBUG_TEXT("fill_calibration_data ");
                CalibrationData* calibration_data = reinterpret_cast<CalibrationData*>(data);

                for(int cvi = 0; cvi < NUM_CV_INS; ++cvi)
                {
                    CalibrationData& cd = calibration_data[cvi];
                    cd = local_calibration_data[cvi];

    #ifdef DEBUG_OUTPUT
                    cd.print();
    #endif                 
                }
                DEBUG_TEXT("\n");
            }
            else
            {
                ASSERT(false);
            }
        }

        float read_multiplexed_input(int input_index)
        {
            return hw.adc.GetMuxFloat(0, input_index);
        }

        float read_pot(int pot_index)
        {
            // pin order is non-sequential (due to board layout)
            switch(pot_index)
            {
                case 0:
                {
                    return read_multiplexed_input(0);
                }
                case 1:
                {
                    return read_multiplexed_input(3);
                }
                case 2:
                {
                    return read_multiplexed_input(6);
                }
                case 3:
                {
                    return read_multiplexed_input(7);
                }
            }

            ASSERT(false);
            return 0.0f;
        }

        float read_cv(int cv_index)
        {
            // pin order is non-sequential (due to board layout)
            float cv = 0.0f;
            switch(cv_index)
            {
                case 0:
                {
                    cv = read_multiplexed_input(2);
                    break;
                }
                case 1:
                {
                    cv = read_multiplexed_input(1);
                    break;
                }
                case 2:
                {
                    cv = read_multiplexed_input(4);
                    break;
                }
                case 3:
                {
                    cv = read_multiplexed_input(5);
                    break;
                }
                default:
                {
                    ASSERT(false);  
                }
            }

            // invert due to inverting op-amp
            return Utils::clamp(1.0f - cv, 0.0f, 1.0f);
        }

        CVData read_cv_scaled_and_calibrated(int cv_index, CV_CONVERSION_MODE mode)
        {
            auto constexpr scale_cv = [](CV_CONVERSION_MODE mode, float cv)
            {
                if(mode == CV_CONVERSION_MODE::POSITIVE_AND_NEGATIVE)
                {
                    return (cv-0.5f)*2.0f; // map 0 to 1 to range -1 to 1;
                }
                else
                {
                    return ((cv-0.5f)*4.0f)-1.0f; // map 0.5 to 1 to range -1 to 1
                }
            };

            static_assert(scale_cv(CV_CONVERSION_MODE::POSITIVE_AND_NEGATIVE, 0.0f) == -1.0f);
            static_assert(scale_cv(CV_CONVERSION_MODE::POSITIVE_AND_NEGATIVE, 1.0f) == 1.0f);
            static_assert(scale_cv(CV_CONVERSION_MODE::POSITIVE_ONLY, 1.0f) == 1.0f);
            static_assert(scale_cv(CV_CONVERSION_MODE::POSITIVE_ONLY, 0.5f) == -1.0f);

            const float raw_cv = read_cv(cv_index);

            const CalibrationData& cd = local_calibration_data[cv_index];
            const float cv = cd.calibrated_read(raw_cv);
            const float cv_scaled = scale_cv(mode, cv);

            constexpr float trigger_threshold = 0.75f; // 0.5 is 0V
            const bool cv_triggered = raw_cv > trigger_threshold;

            const float calibrated_cv = Utils::clamp(cv_scaled, -1.0f, 1.0f); 

            const int cv_VOct = cd.calibrate_read_VOct(raw_cv);

            return { cv_triggered, calibrated_cv, cv_VOct };
        }

        bool read_trigger()
        {
            return trig_pin_gpio.Read();
        }       
    } // namespace HWPlatform

#else // !BOARD_V1_3

    namespace HWPlatform
    {
        void initialise_adc()
        {
            g_encoders[0].Init(enc1_b_pin, enc1_a_pin, enc1_sw_pin);
            g_encoders[1].Init(enc2_b_pin, enc2_a_pin, enc2_sw_pin);
            g_encoders[2].Init(enc3_b_pin, enc3_a_pin, enc3_sw_pin);
            g_encoders[3].Init(enc4_b_pin, enc4_a_pin, enc4_sw_pin);

            for( int p = 0; p < NUM_PLAY_HEADS; ++p )
            {
                g_leds[p].Init( led_pins[p], daisy::GPIO::Mode::OUTPUT, daisy::GPIO::Pull::NOPULL );
            }

            constexpr daisy::Pin pot_pins[NUM_PLAY_HEADS] = { pot1_pin, pot2_pin, pot3_pin, pot4_pin };

            daisy::AdcChannelConfig adc_config[NUM_PLAY_HEADS];
            for( int p = 0; p < NUM_PLAY_HEADS; ++p )
            {
                adc_config[p].InitSingle(pot_pins[p]);
            }
            hw.adc.Init(adc_config, NUM_PLAY_HEADS);
            hw.adc.Start();

            g_record_switch.Init(sw1_pin);
            g_clear_switch.Init(sw2_pin);
            g_menu_switch.Init(sw3_pin);               
        }

        void initialise_screen()
        {
            Screen::Config display_config;
            display_config.driver_config.transport_config.pin_config.dc    = screen_dc_pin;
            display_config.driver_config.transport_config.pin_config.reset = screen_reset_pin;

            g_screen.Init(display_config);         
        }

        void calibrate_to_ground()
        {
            // nothing to do here
        }

        void set_calibration_data(const void* data, size_t size)
        {

        }

        void fill_calibration_data(void* data, uint8_t size)
        {
            
        }    

        float read_pot(int pot_index)
        {
            return hw.adc.GetFloat(pot_index);
        }

        float read_cv(int cv_index)
        {
            // no CV in on this board
            return 0.0f;
        }

        CVData read_cv_scaled_and_calibrated(int cv_index, CV_CONVERSION_MODE mode)
        {
            // no CV in on this board
            return {};
        }   

        bool read_trigger()
        {
            return false;
        }
    } // namespace HWPlatform

#endif // !BOARD_V1_3
}