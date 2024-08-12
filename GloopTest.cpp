#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"

#include "DisplayDriver.h"

using Screen = daisy::OledDisplay<OLED_1_3_inch<daisy::SSD130x4WireSpiTransport>>; 


daisy::DaisySeed hw;

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

constexpr int NUM_ENCODERS = 4;
constexpr int NUM_PLAY_HEADS = 4;

daisy::Encoder encoders[NUM_ENCODERS];

constexpr daisy::Pin led_pins[NUM_PLAY_HEADS] = { led_pin_1, led_pin_2, led_pin_3, led_pin_4 };
daisy::GPIO leds[NUM_PLAY_HEADS];

daisy::GPIO trig_pin_gpio;

daisy::Switch record_switch;
daisy::Switch clear_switch;
daisy::Switch menu_switch;

Screen screen;

        void initialise_hw()
        {
            encoders[0].Init(enc1_b_pin, enc1_a_pin, enc1_sw_pin);
            encoders[1].Init(enc2_b_pin, enc2_a_pin, enc2_sw_pin);
            encoders[2].Init(enc3_b_pin, enc3_a_pin, enc3_sw_pin);
            encoders[3].Init(enc4_b_pin, enc4_a_pin, enc4_sw_pin);

            for( int p = 0; p < NUM_PLAY_HEADS; ++p )
            {
                leds[p].Init( led_pins[p], daisy::GPIO::Mode::OUTPUT, daisy::GPIO::Pull::NOPULL );
            }

            daisy::AdcChannelConfig adc_config;
            adc_config.InitMux(multiplex_in_pin, 8, multiplex_select_pin_0, multiplex_select_pin_1, multiplex_select_pin_2);

            hw.adc.Init(&adc_config, 1);
            hw.adc.Start();       

            trig_pin_gpio.Init(trig_pin, daisy::GPIO::Mode::INPUT, daisy::GPIO::Pull::NOPULL, daisy::GPIO::Speed::LOW); 
        
            record_switch.Init(sw1_pin);
            clear_switch.Init(sw2_pin);
            menu_switch.Init(sw3_pin);        
        }

        void initialise_screen()
        {
            Screen::Config display_config;
            display_config.driver_config.transport_config.pin_config.dc    = screen_dc_pin;
            display_config.driver_config.transport_config.pin_config.reset = screen_reset_pin;

            screen.Init(display_config);         
        }        


void audio_callback(const float* in, float* out, size_t size)
{
	for( size_t s = 0; s < size; ++s )
	{
		out[s] = in[s];
	}
}

int main(void)
{
	hw.Init();

	hw.SetAudioBlockSize(48); // number of samples handled per callback
	hw.SetAudioSampleRate(daisy::SaiHandle::Config::SampleRate::SAI_48KHZ);

	hw.StartAudio(audio_callback);

	while(1)
	{

	}
}
