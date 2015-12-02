/*
* Copyright (c) 2013 Dan Wilcox <danomatika@gmail.com>
*
* BSD Simplified License.
* For information on usage and redistribution, and for a DISCLAIMER OF ALL
* WARRANTIES, see the file, "LICENSE.txt," in this distribution.
*
* See https://github.com/danomatika/ofxMidi for documentation
*
*/
#include "ofApp.h"

#include "ofxPubSubOsc.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    ofBackground(255);
    ofSetLogLevel(OF_LOG_VERBOSE);

    // print the available output ports to the console
    midiOut.listPorts(); // via instance
                         //ofxMidiOut::listPorts(); // via static too

                         // connect
    midiOut.openPort(1);
    //midiOut.openPort(1); // by number
                         //midiOut.openPort("IAC Driver Pure Data In"); // by name
                         //midiOut.openVirtualPort("ofxMidiOut"); // open a virtual port

    channel = 1;
    currentPgm = 1;
    note = 0;
    velocity = 0;
    pan = 0;
    bend = 0;
    touch = 0;
    polytouch = 0;

    pitchBend = 0;

    receiver.setup(8000);
    eegState = 0.5f;

    jaw = 0;
    ofxSubscribeOsc(8100, "/face/jaw", jaw);
    ofxSubscribeOsc(8100, "/face/bend", bend);
    ofxSubscribeOsc(8100, "/face/direction", pan);

    eegState2 = 0;

    velocities.resize(100, 0);
}

//--------------------------------------------------------------
void ofApp::update() {
    while (receiver.hasWaitingMessages()) {
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);

        // check for mouse moved message
        if (m.getAddress() == "/muse/classifier/1") { // close eyes
            eegState = ofClamp(eegState - 0.1f, 0, 1);
            eegState2 = ofClamp(eegState2 - 0.1f, 0, 1);
        }
        if (m.getAddress() == "/muse/classifier/2") { // stare
/*            if (eegState < 0.5f)
                eegState = ofClamp(eegState + 0.1f, 0, 0.5f);
            else
                eegState = ofClamp(eegState - 0.1f, 0.5f, 1);*/
            eegState = ofClamp(eegState + 0.1f, 0, 1);
        }
        if (m.getAddress() == "/muse/classifier/3") { // distract
            eegState = ofClamp(eegState + 0.1f, 0, 1);
        }
    }
    //note = ofMap(eegState, 0, 1, 0, 127, true);
    /*
    if ((int)(eegState * 10) <= 0)
        note = 60;
    else if ((int)(eegState * 10) == 1)
        note = 62;
    else if ((int)(eegState * 10) == 2)
        note = 64;
    else if ((int)(eegState * 10) == 3)
        note = 65;
    else if ((int)(eegState * 10) == 4)
        note = 67;
    else if ((int)(eegState * 10) == 5)
        note = 69;
    else if ((int)(eegState * 10) == 6)
        note = 71;
    else
        note = 72;
        
    note += 24 + ofNoise(ofGetElapsedTimef());
    */
    //note = ofMap(eegState, 0, 1, 70, 80) + ofNoise(ofGetElapsedTimef());
    note = ofMap(jaw, 0.2f, 1, 70, 80);
    velocity = 64;

    if (jaw >= 0.2f)
    {
        //midiOut.sendNoteOn(channel, note, velocity);
        velocities.at(note) += 10;
        if (velocities.at(note) > 100) velocities.at(note) = 100;

        //midiOut.sendNoteOn(channel + 1, note + 4, (1 - bend) * velocity);
        for (int i = 70; i < 81; i++)
        {
            if (i != note)
                velocities.at(i) -= 10;
            if (velocities.at(i) <= 0)
            {
                velocities.at(i) = 0;
                midiOut.sendNoteOff(channel, i);
            }
            else
            {
                midiOut.sendNoteOn(channel, i, velocities.at(i));
            }
        }
    }
    else
    {
    }

    //bend = ofMap(mouseX, 0, ofGetWidth(), 0, MIDI_MAX_BEND);
    //float bendVel = ofMap(bend, 0, 1, 0, 2) * 3.1415f;
    //float pitchBend = ofMap(sinf(ofGetElapsedTimef() * bendVel) * bend, -1, 1, 0, MIDI_MAX_BEND);
    //float bendVel2 = ofMap(bend, 1, 0, 0.1f, 10) * 3.1415f;
    //float pitchBend2 = ofMap(sinf(ofGetElapsedTimef() * bendVel2) * bend, -1, 1, 0, MIDI_MAX_BEND);
    //float pitchBend = ofMap(bend, 0, 1, 0, MIDI_MAX_BEND);
    pitchBend = ofClamp(pitchBend + ofMap(bend, 0, 1, -100, 100), 0, MIDI_MAX_BEND);
    midiOut.sendPitchBend(channel, pitchBend);
    //midiOut.sendPitchBend(channel + 1, pitchBend2);

    midiOut.sendControlChange(channel, 10, ofMap(pan, -1, 1, 0, 127));
    midiOut.sendControlChange(channel + 1, 10, ofMap(pan, -1, 1, 0, 127));
}

//--------------------------------------------------------------
void ofApp::draw() {

    // let's see something
    ofSetColor(0);
    stringstream text;
    text << "connected to port " << midiOut.getPort()
        << " \"" << midiOut.getName() << "\"" << endl
        << "is virtual?: " << midiOut.isVirtual() << endl << endl
        << "sending to channel " << channel << endl << endl
        << "current program: " << currentPgm << endl << endl
        << "note: " << note << endl
        << "velocity: " << velocity << endl
        << "pan: " << pan << endl
        << "bend: " << bend << endl
        << "touch: " << touch << endl
        << "eeg: " << eegState << endl
        << "polytouch: " << polytouch;
    ofDrawBitmapString(text.str(), 20, 20);
}

//--------------------------------------------------------------
void ofApp::exit() {

    // clean up
    midiOut.closePort();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

    if ('1' <= key && key <= '9')
    {
        vector<unsigned char> data(2);
        data[0] = (unsigned char)0xC0;
        data[1] = key - '0';
        midiOut.sendMidiBytes(data);
    }

    // send a note on if the key is a letter or a number
    if (isalnum((unsigned char)key)) {

        // scale the ascii values to midi velocity range 0-127
        // see an ascii table: http://www.asciitable.com/
        note = ofMap(key, 48, 122, 0, 127);
        velocity = 64;
        //midiOut.sendNoteOn(channel, note, velocity);
    }

    if (key == 'l') {
        ofxMidiOut::listPorts();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
    midiOut.sendPolyAftertouch(channel, ofMap(x, 0, ofGetWidth(), 64, 127), ofMap(y, 0, ofGetHeight(), 64, 127));
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::mouseReleased() {
}
