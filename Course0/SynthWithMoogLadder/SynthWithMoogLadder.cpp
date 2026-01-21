#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod pod;
Oscillator osc;
MoogLadder flt;
Adsr env;
bool gate = false;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)

{

    pod.ProcessAllControls();

    float freq_param = pod.knob1.Process() * 1000.0f + 40.0f;
    float cutoff = pod.knob2.Process() * 8000.0f + 100.0f;

    float resonance = pod.encoder.Increment() * 0.02f;

    osc.SetFreq(freq_param);
	
    flt.SetFreq(cutoff);
    flt.SetRes(resonance);

    if(pod.button1.RisingEdge())
        gate = true;

    if(pod.button2.RisingEdge())
        gate = false;

    //env.SetGate(gate);

    for(size_t i = 0; i < size; i++)
    {

        float sig = osc.Process();
        float amp = env.Process(gate);
        float filt = flt.Process(sig * amp);

        out[ 0 ][ i ] = filt;
        out[ 1 ][ i ] = filt;

    }

}

int main(void)

{

	pod.Init();

	osc.Init(pod.AudioSampleRate());
	osc.SetWaveform(Oscillator::WAVE_SAW);
	flt.Init(pod.AudioSampleRate());
	//flt.SetDrive(.5f);
	env.Init(pod.AudioSampleRate());
	env.SetTime(ADSR_SEG_ATTACK, 0.01f);
	env.SetTime(ADSR_SEG_DECAY, 0.2f);
	env.SetTime(ADSR_SEG_RELEASE, 0.5f);
	env.SetSustainLevel(0.6f);
	
	pod.StartAdc();
	pod.StartAudio(AudioCallback);
	
	while(1) {}

}