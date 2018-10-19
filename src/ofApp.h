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

        float binToFrequency(float sanmple, float sampleRate, float fftSize);

        ofSoundStream soundStream;
        ofSoundStreamSettings settings;

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
        int bufferSize; 
        int sampleRate;
};
