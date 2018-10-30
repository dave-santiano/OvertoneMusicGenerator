#pragma once

#include "ofMain.h"
#include "ofxFft.h"


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);

        vector<vector<float>> getProminentFrequencies();
        void audioIn(ofSoundBuffer& buffer);

        void updateWaveform(int waveformResolution);
        void audioOut(ofSoundBuffer& buffer);
        void playProminentFreqencies();
        vector<float> referenceScale;
        vector<float> waveform;
        double phase;
        float frequency;
        bool referenceScaleMode;



        void printProminentFrequencies(vector<vector<float>> prominentFrequencies);

        float binToFrequency(float sanmple, float sampleRate, float fftSize);

        ofSoundStream outSoundStream;
        ofSoundStream inSoundStream;
        ofSoundStreamSettings outputAudioStreamSettings;
        ofSoundStreamSettings inputAudioStreamSettings;



        vector<float> audioBins;
        vector<float> middleBins;
        vector<float> audioInputData;
        vector<vector<float>> prominentFrequencies;

        ofxFft* fft;
        mutex soundMutex;
        ofMutex waveformMutex;

        bool recording;
        int nProminentFrequencies;


    private:
        const double pi = 3.14159265358979323846;
        int outputBufferSize;
        int fftSize;
        int sampleRate;
};
