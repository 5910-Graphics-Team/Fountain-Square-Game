#include <iostream>

int main() {
	std::cout << '\a';
}
float frequency_hz = 440.0;
float delta = 2.0 * M_PI * frequency_hz / sample_rate;
float phase = 0;
// source: https://www.youtube.com/watch?v=jNSiZqSQis4&t=329s at 27:44
// sine wave 
// buffer style 'for a middleware framework'
void audioCallback(float** inputChannelData,
	int numInputChannels,
	float** outputChannelData,
	int numOutputChannels,
	int numFrames) 
{
	for (int i = 0; i < numFrames; ++i) {
		float next_sample = std::sin(phase);
		phase = std::fmod(phase + delta);
		for (int j = 0; j < numOutputChannels; ++j) {
			outputChannelData[i][j] = next_sample;
		}
	}
}