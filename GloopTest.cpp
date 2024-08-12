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

int main(void)
{
	hw.Init();

	hw.SetAudioBlockSize(SoundEngine::AUDIO_BUFFER_SIZE); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

	hw.StartAudio(audio_callback);

	while(1)
	{

	}
}
