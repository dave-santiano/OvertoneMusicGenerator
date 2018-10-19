#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    bufferSize = 8096;
    sampleRate = 48000;
    nProminentFrequencies = 7;
    recording = false;

    fft = ofxFft::create(bufferSize, OF_FFT_WINDOW_HAMMING);

    audioInputData.assign(bufferSize, 0.0f);

    settings.setApi(ofSoundDevice::Api::MS_DS);
    settings.setInListener(this);
    settings.bufferSize = bufferSize;
    settings.numInputChannels  = 1;
    settings.numOutputChannels = 0;
    settings.sampleRate = sampleRate;
    soundStream.setup(settings);

    audioBins.resize(fft->getBinSize());
    middleBins.resize(fft->getBinSize());

    ofBackground(200, 120, 120);
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    if (!middleBins.empty()) {
        for (int i = 0; i < 1024; i++) {
            ofSetColor(120, 200, 200);
            float newValue = ofMap(middleBins[i], 0, 1.0, 0.0, ofGetHeight());
            ofDrawRectangle((i * (ofGetWidth() / 1024)), 0, (ofGetWidth() / 1024), newValue);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'r') {
        recording = true;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    if (key == 'r') {
        recording = false;
        prominentFrequencies = getProminentFrequencies();

        for (int i = 1; i < prominentFrequencies.size(); i++) {
            float frequency = binToFrequency(prominentFrequencies[i][1], sampleRate, bufferSize);
            std::cout << "#" << i << " Frequency: " << frequency << " Bin#" << prominentFrequencies[i][1] << " content:" << prominentFrequencies[i][0] << std::endl;
            ofLog(OF_LOG_NOTICE,"#" + ofToString(i) + " Frequency: " + ofToString(frequency) + " Bin#" + ofToString(prominentFrequencies[i][1]) + " content:" + ofToString(prominentFrequencies[i][0]));
        }

        //std::cout << "\n" << std::endl;
        middleBins.assign(bufferSize, 0.0f);
        prominentFrequencies = { {0.0f, 0} };
    }
}


vector<vector<float>> ofApp::getProminentFrequencies() {
    float tempMax  = 1.0f;
    bool inRange  = false;
    int binRange = 5;

    for (int i = 0; i < nProminentFrequencies; i++) {
        float maxFrequency = 0.0f;
        float maxFrequencyIndex = 0;

        for (int j = 4; j < middleBins.size() / 2; j++) {
            if (middleBins[j] < tempMax && middleBins[j] > maxFrequency) {
                for (int k = 0; k < prominentFrequencies.size(); k++) {
                    if (j <= (prominentFrequencies[k][1] + binRange) && j >= (prominentFrequencies[k][1] - binRange)) {
                        inRange = true;
                    }
                }

                if (inRange == false) {
                    maxFrequency = middleBins[j];
                    maxFrequencyIndex = j;
                }
            }
            inRange = false;
        }

        vector<float> frequencyIndexPair = { maxFrequency, maxFrequencyIndex };
        prominentFrequencies.push_back(frequencyIndexPair);

        tempMax = maxFrequency;
    }
    return prominentFrequencies;
}

//--------------------------------------------------------------

void ofApp::audioIn(ofSoundBuffer& buffer) {
    float maxValue = 0.0f;
    if (recording == true) {

        for (size_t i = 0; i < bufferSize; i++) {
            audioInputData[i] = buffer[i];
        }

        for (size_t i = 0; i < bufferSize; i++) {
            if (abs(audioInputData[i] > maxValue)) {
                maxValue = abs(audioInputData[i]);
            }
        }

        //divide by max value to get values between 0.0 and 1.0
        for (size_t i = 0; i < bufferSize; i++) {
            audioInputData[i] /= maxValue;
        }

        fft->setSignal(audioInputData);

        float* curFft = fft->getAmplitude();

        memcpy(&audioBins[0], curFft, sizeof(float)*fft->getBinSize());

        maxValue = 0;

        for (size_t i = 0; i < fft->getBinSize(); i++) {
            if (abs(audioBins[i] > maxValue)) {
                maxValue = abs(audioBins[i]);
            }
        }

        for (size_t i = 0; i < fft->getBinSize(); i++) {
            audioBins[i] /= maxValue;
        }

        //soundMutex.lock();
        middleBins = audioBins;
        //soundMutex.unlock();
    }
}

float ofApp::binToFrequency(float sample, float sampleRate, float fftSize) {
    float frequency = sample * (sampleRate / fftSize);
    return frequency;
}
