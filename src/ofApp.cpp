#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    outputBufferSize = 256;
    fftSize = 8096;
    sampleRate = 48000;
    nProminentFrequencies = 7;
    recording = false;

    fft = ofxFft::create(fftSize, OF_FFT_WINDOW_HAMMING);

    audioInputData.assign(fftSize, 0.0f);

    frequency = 0;
    phase = 0;
    updateWaveform(256);
    referenceScaleMode = false;

    //Found with earlier experiments
    referenceScale = { 557.312, 693.676, 830.04, 136.364, 972.332, 1245.06, 1387.35 };

    
    outputAudioStreamSettings.setApi(ofSoundDevice::Api::MS_DS);
    outputAudioStreamSettings.setOutListener(this);
    outputAudioStreamSettings.bufferSize = outputBufferSize;
    outputAudioStreamSettings.numInputChannels = 0;
    outputAudioStreamSettings.numOutputChannels = 1;
    outputAudioStreamSettings.sampleRate = sampleRate;

    inputAudioStreamSettings.setApi(ofSoundDevice::Api::MS_DS);
    inputAudioStreamSettings.setInListener(this);
    inputAudioStreamSettings.bufferSize = fftSize;
    inputAudioStreamSettings.sampleRate = sampleRate;
    inputAudioStreamSettings.numInputChannels = 1;
    inputAudioStreamSettings.numOutputChannels = 0;

    inSoundStream.setup(inputAudioStreamSettings);
    outSoundStream.setup(outputAudioStreamSettings);

    audioBins.resize(fft->getBinSize());
    middleBins.resize(fft->getBinSize());

    ofBackground(200, 120, 120);
}

//--------------------------------------------------------------
void ofApp::update(){
    updateWaveform(256);
}

//--------------------------------------------------------------
void ofApp::draw(){
    if (!middleBins.empty()) {
        for (int i = 0; i < 1024; i++) {
            ofSetColor(120, 200, 200);
            float newValue = ofMap(middleBins[i], 0, 1.0, 0.0, ofGetHeight());
            ofDrawRectangle((i * (ofGetWidth() / 1024)), ofGetHeight(), (ofGetWidth() / 1024), newValue * -1);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 'r') {
        recording = true;
        prominentFrequencies = { {0.0f, 0} };
    }

    if (!prominentFrequencies.empty() && referenceScaleMode == false) {
        if (key == 'a') {
            frequency = binToFrequency(prominentFrequencies[0][1], sampleRate, fftSize);
        }
        if (key == 's') {
            frequency = binToFrequency(prominentFrequencies[1][1], sampleRate, fftSize);
        }
        if (key == 'd') {
            frequency = binToFrequency(prominentFrequencies[2][1], sampleRate, fftSize);
        }
        if (key == 'f') {
            frequency = binToFrequency(prominentFrequencies[3][1], sampleRate, fftSize);
        }
        if (key == 'g') {
            frequency = binToFrequency(prominentFrequencies[4][1], sampleRate, fftSize);
        }
        if (key == 'h') {
            frequency = binToFrequency(prominentFrequencies[5][1], sampleRate, fftSize);
        }
        if (key == 'j') {
            frequency = binToFrequency(prominentFrequencies[6][1], sampleRate, fftSize);
        }
    }
    else if(referenceScaleMode == true){
        if (key == 'a') {
            frequency = referenceScale[0];
        }
        if (key == 's') {
            frequency = referenceScale[1];
        }
        if (key == 'd') {
            frequency = referenceScale[2];
        }
        if (key == 'f') {
            frequency = referenceScale[3];
        }
        if (key == 'g') {
            frequency = referenceScale[4];
        }
        if (key == 'h') {
            frequency = referenceScale[5];
        }
        if (key == 'j') {
            frequency = referenceScale[6];
        }
    }

    if (key == 'o') {
        if (referenceScaleMode == false) {
            referenceScaleMode = true;
            ofLog(OF_LOG_NOTICE, "Reference Scale Mode on!");
        }
        else {
            referenceScaleMode = false;
            ofLog(OF_LOG_NOTICE, "Reference Scale Mode off!");

        }
    }

    if (key == 'c') {
        frequency = 0;
        ofLog(OF_LOG_NOTICE, "Frequency set back to 0!");
    }
}

//--------------------------------------------------------------d
void ofApp::keyReleased(int key){
    if (key == 'r') {
        recording = false;
        prominentFrequencies = getProminentFrequencies();
        printProminentFrequencies(prominentFrequencies);
        middleBins.assign(fftSize, 0.0f);
        //prominentFrequencies = { {0.0f, 0} };
    }
}

void ofApp::printProminentFrequencies(vector<vector<float>> prominentFrequencies) {
    for (int i = 0; i < prominentFrequencies.size(); i++) {
        float tempFrequency = binToFrequency(prominentFrequencies[i][1], sampleRate, fftSize);
        std::cout << "#" << i << " Frequency: " << tempFrequency << " Bin#" << prominentFrequencies[i][1] << " content:" << prominentFrequencies[i][0] << std::endl;
        ofLog(OF_LOG_NOTICE, "#" + ofToString(i) + " Frequency: " + ofToString(tempFrequency) + " Bin#" + ofToString(prominentFrequencies[i][1]) + " content:" + ofToString(prominentFrequencies[i][0]));
    }
    ofLog(OF_LOG_NOTICE, "\n");
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
        for (size_t i = 0; i < fftSize; i++) {
            audioInputData[i] = buffer[i];
        }

        for (size_t i = 0; i < fftSize; i++) {
            if (abs(audioInputData[i] > maxValue)) {
                maxValue = abs(audioInputData[i]);
            }
        }

        //divide by max value to get values between 0.0 and 1.0
        for (size_t i = 0; i < fftSize; i++) {
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

        middleBins = audioBins;
    }
}

void ofApp::audioOut(ofSoundBuffer& buffer) {

    float phaseStep = frequency / sampleRate;
    //if (!prominentFrequencies.empty()) {
    for (size_t i = 0; i < outputBufferSize; i++) {
        phase += phaseStep;
        int waveformIndex = (int)(phase * waveform.size()) % waveform.size();
        buffer[i] = waveform[waveformIndex];
    }
    //}
}

void ofApp::updateWaveform(int waveformResolution) {
    waveform.resize(waveformResolution);

    float waveformStep = (pi * 2.0f) / (float) waveform.size();

    for (int i = 0; i < waveform.size(); i++) {
        waveform[i] = sin(i * waveformStep);
    }
}

float ofApp::binToFrequency(float sample, float sampleRate, float fftSize) {
    float binFrequency = sample * (sampleRate / fftSize);
    return binFrequency;
}
