#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod hw;

Oscillator osc;

void AudioCallback(AudioHandle::InterleavingInputBuffer in, AudioHandle::InterleavingOutputBuffer out, size_t size)

{

    hw.ProcessAllControls();

    float amp = hw.knob1.Process();
    float freq = hw.knob2.Process() * 800 + 20;

    osc.SetAmp(amp);
    osc.SetFreq(freq);

    for(size_t i = 0; i < size; i++)
    {

        float sig = osc.Process();
        out[ i ] = sig;
        out[ i + 1 ] = sig;
    }

}

int main(void)

{

    hw.Init();
	hw.seed.StartLog( true );// Waits for a serial console to be connected 
	hw.SetAudioBlockSize(4);

    osc.Init(hw.AudioSampleRate());
    osc.SetWaveform(osc.WAVE_SIN);

	hw.StartAdc();
    hw.StartAudio(AudioCallback);

    while(1) {}

}
