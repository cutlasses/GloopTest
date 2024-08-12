#include "daisy_seed.h"
#include "daisysp.h"

#include "BitCrush.h"
#include "DebugOutput.h"
#include "GloopInterface.h"
#include "i2c.h"
#include "Looper.h"
#include "Profiler.h"

using namespace daisy;
using namespace daisysp;

DaisySeed hw;

Gloop::Looper looper;
Gloop::Interface interface;

daisysp::DelayLine<SoundEngine::SampleType, int(SoundEngine::SAMPLE_RATE*SoundEngine::MAX_DELAY_TIME)> delay_line; // 0.5 second delay line

bool apply_effects_to_channel[2] = {false, false};

float g_delay_feedback = 0.5f; // 50%
float g_delay_wet_mix = 0.6f; // 60% dry

float g_reverb_wet_mix = 0.3f;

ReverbSc g_reverb;
constexpr float reverb_feedback = 0.85f;
constexpr float reverb_lp_freq = 2000.0f;

BitCrush g_bit_crush;
constexpr int32_t min_bc_bit_depth = 1;
constexpr int32_t max_bc_bit_depth = 32;
constexpr int32_t min_bc_rate_div = 1;
constexpr int32_t max_bc_rate_div = 16;
bool g_bc_post = false;


void check_toggle_record_mode();

void audio_callback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	DECLARE_PROFILE_SECTION("audio interrupt");

	// CAN LEAD TO 2 TOGGLES START/STOP check_toggle_record_mode();

	looper.update( in[0], out[0], out[1], size );

	// apply effects //

	const float delay_dry_mix = 1.0f - g_delay_wet_mix;
	const float reverb_dry_mix = 1.0f - g_reverb_wet_mix;

	for( size_t s = 0; s < size; ++s )
	{
		const bool A_effects = apply_effects_to_channel[0];
		const bool B_effects = apply_effects_to_channel[1];

		SoundEngine::SampleType sample;
		
		if( A_effects && B_effects )
		{
			sample = out[0][s] + out[1][s];
		}
		else if( A_effects )
		{
			sample = out[0][s];
		}
		else
		{
			sample = out[1][s];
		}

		if( !g_bc_post )
		{
			sample = g_bit_crush.process(sample);
		}

		SoundEngine::SampleType delay_sample = delay_line.Read();
		delay_line.Write( sample + (delay_sample * g_delay_feedback) );

		sample = sample * delay_dry_mix + delay_sample * g_delay_wet_mix;

		const SoundEngine::SampleType reverb_in = sample * g_reverb_wet_mix;
		float out_l, out_r;
		g_reverb.Process( reverb_in, reverb_in, &out_l, &out_r );

		sample = sample * reverb_dry_mix + out_l;

		if( g_bc_post )
		{
			sample = g_bit_crush.process(sample);
		}		
		
#ifdef CLICK_DETECTION
		static SoundEngine::SampleType prev_sample = sample;
#endif
		DETECT_CLICK(prev_sample, sample);

		if( A_effects && B_effects )
		{
			out[0][s] = sample;
			out[1][s] = sample;
		}
		else if( A_effects )
		{
			out[0][s] = sample;
		}
		else
		{
			out[1][s] = sample;
		}		
	}
}

void apply_encoders_to_active_play_head()
{
	const int active_play_head = interface.active_head();

	if( interface.toggle_direction() )
	{
		looper.toggle_direction( active_play_head );
	}

	if( interface.toogle_speed_mode() )
	{
		looper.toggle_speed_mode( active_play_head );
	}

	const bool fine_adjust = interface.fine_adjust();

	const int position_inc = interface.loop_position_inc();
	if( position_inc != 0 )
	{
		looper.inc_loop_position( active_play_head, position_inc, fine_adjust );
	}

	const int loop_size_inc = interface.loop_size_inc();
	if( loop_size_inc != 0 )
	{
		looper.inc_loop_size( active_play_head, loop_size_inc, fine_adjust );
	}

	const int jitter_inc = interface.loop_jitter_inc();
	if( jitter_inc != 0 )
	{
		looper.inc_loop_jitter( active_play_head, jitter_inc );
	}

	const int speed_inc = interface.speed_inc();
	if( speed_inc != 0 )
	{
		looper.inc_speed( active_play_head, speed_inc, fine_adjust );
	}
}

void initialise_loop_mode()
{
	static_assert(Gloop::NUM_PLAY_HEADS== Gloop::NUM_CV_INS);
	for( int cv = 0; cv < Gloop::NUM_CV_INS; ++cv )
	{
		const Gloop::Interface::CvTargetDetails target_details = interface.cv_target_details(cv);

		if(target_details.play_head == Gloop::Interface::CvTargetDetails::ALL_HEADS)
		{
			for(int ph = 0; ph < Gloop::NUM_PLAY_HEADS; ++ph)
			{
				if(target_details.target == CV_TARGET::START_LOOP_ONE_SHOT)
				{
					looper.initialise_loop_mode(cv, Gloop::LOOP_MODE::ONE_SHOT);
				}
				else
				{
					looper.initialise_loop_mode(cv, Gloop::LOOP_MODE::CONTINUOUS);
				}				
			}
		}
		else
		{
			if(target_details.target == CV_TARGET::START_LOOP_ONE_SHOT)
			{
				looper.initialise_loop_mode(cv, Gloop::LOOP_MODE::ONE_SHOT);
			}
			else
			{
				looper.initialise_loop_mode(cv, Gloop::LOOP_MODE::CONTINUOUS);
			}
		}
	}		
}

void apply_cv_to_all_play_heads()
{
	struct Modulation
	{
		Gloop::CVData m_speed = {};
		float m_position = 0.0f;
		float m_size = 0.0f;
		bool m_trigger_start = false;
		Gloop::LOOP_MODE m_loop_mode = Gloop::LOOP_MODE::CONTINUOUS;

		void set_modulation(CV_TARGET target, const Gloop::CVData& mod)
		{
			switch (target)
			{
				case CV_TARGET::SPEED:
				{
					m_speed = mod;
					break;
				}
				case CV_TARGET::POSITION:
				{
					m_position = mod.cv_calibrated;
					break;
				}
				case CV_TARGET::SIZE:
				{
					m_size = mod.cv_calibrated;
					break;
				}
				case CV_TARGET::START_LOOP:
				{
					m_trigger_start = mod.cv_trigger;
					m_loop_mode = Gloop::LOOP_MODE::CONTINUOUS;
					break;
				}
				case CV_TARGET::START_LOOP_ONE_SHOT:
				{
					m_trigger_start = mod.cv_trigger;
					m_loop_mode = Gloop::LOOP_MODE::ONE_SHOT;
					break;
				}			
				default:
				{
					break;
				}
			}			
		}
	};
	Modulation pending_mods[Gloop::NUM_PLAY_HEADS];

	// accumlate all of the active mods, leave the rest at 0
	// (can't just zero all mods then reset, incase we get an interrupt after zeroing and before setting)
	for( int cv = 0; cv < Gloop::NUM_CV_INS; ++cv )
	{
		const Gloop::Interface::CvTargetDetails& target_details = interface.cv_target_details(cv);

		if(target_details.play_head == Gloop::Interface::CvTargetDetails::ALL_HEADS)
		{
			for( int p = 0; p < Gloop::NUM_PLAY_HEADS; ++p )
			{
				Modulation& mod = pending_mods[p];
				mod.set_modulation(target_details.target, target_details.mod);
			}
		}
		else
		{
			Modulation& mod = pending_mods[target_details.play_head];
			mod.set_modulation(target_details.target, target_details.mod);
		}
	}

	for( int p = 0; p < Gloop::NUM_PLAY_HEADS; ++p )
	{
		const Modulation& mod = pending_mods[p];

		looper.set_speed_modulation(p, mod.m_speed);
		looper.set_loop_position_modulation(p, mod.m_position);
		looper.set_loop_size_modulation(p, mod.m_size);
		looper.set_loop_mode(p, mod.m_loop_mode);

		if(mod.m_trigger_start)
		{
			looper.jump_to_location(p, 0, Gloop::SET_HEAD_POSITION);
		}
	}
}

void check_toggle_record_mode()
{
	if( interface.toggle_record_mode() )
	{
		if( looper.is_playing() )
		{
			if( looper.is_recording() )
			{
				looper.stop_record();
			}
			else if(interface.allow_overdub())
			{
				looper.start_record();
			}
		}
		else
		{
			looper.start_record();
		}
	}	
}

int main(void)
{
	hw.Init();

#ifdef DEBUG_OUTPUT
	//hw.StartLog(true);
#endif

	i2c_bus i2c;

	looper.initialise();
	interface.initialise();

	delay_line.Init();

	float delay_time = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::DELAY_TIME).get_float();
	size_t delay_samples = delay_time * SoundEngine::SAMPLE_RATE;	
	delay_line.SetDelay(delay_samples);

	g_reverb.Init(SoundEngine::SAMPLE_RATE);
	g_reverb.SetFeedback(reverb_feedback);
    g_reverb.SetLpFreq(reverb_lp_freq);

	hw.SetAudioBlockSize(SoundEngine::AUDIO_BUFFER_SIZE); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	static_assert(SoundEngine::SAMPLE_RATE == 48000);
	hw.StartAudio(audio_callback);

	initialise_loop_mode();

	//Gloop::looped_index_test();

	while(1)
	{
		DECLARE_PROFILE_SECTION("main loop");

		interface.update( i2c, looper );

		delay_time = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::DELAY_TIME).get_float();
		delay_samples = delay_time * SoundEngine::SAMPLE_RATE;	
		delay_line.SetDelay(delay_samples);

		g_delay_feedback = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::DELAY_FEEDBACK).get_float();
		g_delay_wet_mix = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::DELAY_MIX).get_float();	
		g_reverb_wet_mix = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::REVERB_MIX).get_float();

		const float bit_depth_f = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::BIT_CRUSH_DEPTH).get_float();
		const int32_t bit_depth = Utils::round_to_int<int32_t>( Utils::lerp(min_bc_bit_depth, max_bc_bit_depth, bit_depth_f) );
		const float bit_rate_f = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::BIT_CRUSH_RATE_DIV).get_float();
		const int32_t bit_rate_div = Utils::round_to_int<int32_t>( Utils::lerp(min_bc_rate_div, max_bc_rate_div, (1.0f-bit_rate_f)) );
		g_bc_post = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::BIT_CRUSH_POST).get_bool();
		g_bit_crush.set_bit_depth(bit_depth);
		g_bit_crush.set_sample_rate_reduction(bit_rate_div);

		apply_effects_to_channel[0] = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::EFFECTS_A_ENABLED).get_bool();
		apply_effects_to_channel[1] = interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::EFFECTS_B_ENABLED).get_bool();

		// apply interface changes
		check_toggle_record_mode();
		
		if( interface.clear_loop() )
		{
			looper.clear_loop();
			initialise_loop_mode();
		}

		apply_encoders_to_active_play_head();

#ifdef CV_PRESENT
		apply_cv_to_all_play_heads();
#endif

		looper.set_tape_effect_amount( interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::TAPE_EFFECT).get_float() );
		looper.set_input_mix_to_output( true, interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::IN_A_MIX).get_float() );
		looper.set_input_mix_to_output( false, interface.parameter_value_encoder_type(Gloop::PARAMETER_TYPE::IN_B_MIX).get_float() );

		const GRID_QUANTISATION gq = interface.parameter_value_encoder_type(Gloop::GRID).get_grid_quantisation();
		looper.set_grid_resolution(grid_quantisation_to_int(gq));

		// set gain from pots and cv
		for( int p = 0; p < Gloop::NUM_PLAY_HEADS; ++p )
		{
			const Gloop::PARAMETER_TYPE om = Gloop::PARAMETER_TYPE(int(Gloop::HEAD_0_AB) + p);
			looper.set_output_mode(p, interface.parameter_value_encoder_type(om).get_output_mode());

			looper.set_gain( p, interface.head_gain(p) );
		}

		const int trigger = interface.sequence_trigger();
		const int play_head = 0; // external triggers set position of head 0
		if( trigger >= 0 )
		{
			looper.jump_to_location(play_head, trigger, interface.sequence_mode());
		}

#ifdef PROFILER_ENABLED
		Profiler::show();
#endif
	}
}
