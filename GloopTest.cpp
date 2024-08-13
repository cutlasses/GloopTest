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

constexpr int SCREEN_WIDTH(128);
constexpr int SCREEN_HEIGHT(64);

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
	}

	// invert due to inverting op-amp
	const float invert_cv = 1.0f - cv;

	if(invert_cv < 0.0f)
	{
		return 0.0f;
	}
	else if(invert_cv > 1.0f)
	{
		return 1.0f;
	}
	return invert_cv;
}

void draw_test_screen()
{
	constexpr int line_length = 8;
	constexpr int half_line_length = line_length / 2;
	constexpr int x_extent = SCREEN_WIDTH - 1;
	constexpr int y_extent = SCREEN_HEIGHT - 1;
	constexpr int x_centre = x_extent / 2;
	constexpr int y_centre = y_extent / 2;

	// top left
	screen.DrawLine(0, 0, line_length, 0, true);
	screen.DrawLine(0, 0, 0, line_length, true);

	// top right
	screen.DrawLine(x_extent, 0, x_extent - line_length, 0, true);
	screen.DrawLine(x_extent, 0, x_extent, line_length, true);

	// bottom left
	screen.DrawLine(0, y_extent, 0, y_extent - line_length, true);
	screen.DrawLine(0, y_extent, line_length, y_extent, true);

	// bottom right
	screen.DrawLine(x_extent, y_extent, x_extent - line_length, y_extent, true);
	screen.DrawLine(x_extent, y_extent, x_extent, y_extent - line_length, true);

	// centre
	screen.DrawLine(x_centre - half_line_length, y_centre, x_centre + half_line_length, y_centre, true);
	screen.DrawLine(x_centre, y_centre - half_line_length, x_centre, y_centre + half_line_length, true);    
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

	initialise_hw();
	initialise_screen();

	while(1)
	{
		int cursor_x = 2;
		int cursor_y = 2;

		/// Debounce controls ///
		menu_switch.Debounce();
		record_switch.Debounce();
		clear_switch.Debounce();

		for(int i = 0; i < NUM_ENCODERS; ++i)
		{
			encoders[i].Debounce();
		}
			
		draw_test_screen();

		screen.SetCursor(cursor_x, cursor_y);

		if(menu_switch.RisingEdge())
		{
			screen.WriteString("Menu", Font_6x8, true);
		}
		else if(record_switch.RisingEdge())
		{
			screen.WriteString("Record", Font_6x8, true);
		}
		else if(clear_switch.RisingEdge())
		{
			screen.WriteString("Clear", Font_6x8, true);
		}

		cursor_y += 8;
		screen.SetCursor(cursor_x, cursor_y);

		/// Encoders ///
		char string_buffer[32];
		for(int i = 0; i < NUM_ENCODERS; ++i)
		{
			const daisy::Encoder& encoder = encoders[i];
			if(int32_t inc = encoder.Increment())
			{
				snprintf(string_buffer, sizeof(string_buffer), "Encoder %d inc %ld", i, inc);
				screen.WriteString(string_buffer, Font_6x8, true);
				break;
			}
			else if(encoder.RisingEdge())
			{
				snprintf(string_buffer, sizeof(string_buffer), "Encoder %d pressed", i);
				screen.WriteString(string_buffer, Font_6x8, true);
				break;
			}
		}

		cursor_y += 8;
		screen.SetCursor(cursor_x, cursor_y);

		// pots ///
		cursor_x = 2;
		for(int i = 0; i < NUM_PLAY_HEADS; ++i)
		{
			snprintf(string_buffer, sizeof(string_buffer), "%2f", read_pot(i));
			screen.WriteString(string_buffer, Font_6x8, true);

			cursor_x += strlen(string_buffer);
			screen.SetCursor(cursor_x, cursor_y);
		}

		cursor_x = 2;
		cursor_y += 8;

		/// trigger ///
		const bool trig = trig_pin_gpio.Read();
		if(trig)
		{
			screen.WriteString("TrigOn", Font_6x8, true);
		}
		else
		{
			screen.WriteString("TrigOff", Font_6x8, true);
		}

		cursor_x += strlen("TrigOff") + 6;

		/// CV ///
		for(int i = 0; i < NUM_PLAY_HEADS; ++i)
		{
			snprintf(string_buffer, sizeof(string_buffer), "%2f", read_cv(i));
			screen.WriteString(string_buffer, Font_6x8, true);

			cursor_x += strlen(string_buffer);
			screen.SetCursor(cursor_x, cursor_y);
		}

		screen.Update();

		hw.DelayMs(1);
	}
}
