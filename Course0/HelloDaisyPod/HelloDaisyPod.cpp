#include "daisy_pod.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

DaisyPod hw;
Color led1Color, led2Color;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	hw.ProcessAllControls();
	for (size_t i = 0; i < size; i++)
	{
		out[0][i] = in[0][i];
		out[1][i] = in[1][i];
	}

	//Color::PresetColor::RED;
	hw.ClearLeds();
	hw.led1.SetColor(led1Color);
	hw.led2.SetColor(led2Color);
	hw.UpdateLeds();

}

int main(void)
{
	hw.Init();
	hw.seed.StartLog( true );
	hw.seed.PrintLine( "Hello Seed from Pod" );
	led1Color.Init(Color::PresetColor::BLUE);
	led2Color.Init(Color::PresetColor::GREEN);
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAdc();
	hw.StartAudio(AudioCallback);
	while(1) {}

}