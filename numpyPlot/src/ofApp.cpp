#include "ofApp.h"

#include "ofxNumpy.h"
#include "ofxPubSubOsc.h"

//--------------------------------------------------------------
void ofApp::setup() {
    ofHideCursor();

    std::function<void(ofxOscMessage &)> f = [&](ofxOscMessage& m) {
        samplePrev = sample;
        sample.x = m.getArgAsFloat(0);
        sample.y = m.getArgAsFloat(1);
        sampleIndex = m.getArgAsInt(2);

        yNew.push_back(sample);

        auto p0 = sample;
        p0.x = ofMap(p0.x, minXY.x, maxXY.x, -width * 0.5f + 10, width * 0.5f - 10);
        p0.y = ofMap(p0.y, minXY.y, maxXY.y, -height * 0.5f + 10, height * 0.5f - 10);
        for (int i = 0; i < yNew.size(); i++)
        {
            auto p1 = yNew.at(i);
            p1.x = ofMap(p1.x, minXY.x, maxXY.x, -width * 0.5f + 10, width * 0.5f - 10);
            p1.y = ofMap(p1.y, minXY.y, maxXY.y, -height * 0.5f + 10, height * 0.5f - 10);
            float dist = p0.distance(p1);
            if (dist < distThreshold)
            {
                stringsNew.addVertex(p0);
                stringsNew.addVertex(p1);
                stringsNew.addColor(ofFloatColor::fromHsb((float)i / (refreshSec * 10) * 0.75f, 1, 1, ofMap(dist, distThreshold * 0.5f, 0, 0, lineAlpha)));
                stringsNew.addColor(ofFloatColor::fromHsb((float)(yNew.size() - 1) / (refreshSec * 10) * 0.75f, 1, 1, ofMap(dist, distThreshold * 0.5f, 0, 0, lineAlpha)));
                stringsNew.addIndex(stringsNew.getNumVertices() - 2);
                stringsNew.addIndex(stringsNew.getNumVertices() - 1);
            }
        }
    };
    ofxSubscribeOsc(8000, "/muse/tsne", f);
    sampleIndex = 0;
    //ofSetWindowShape(640, 640);
    //ofSetWindowShape(560, 315);
    ofxNumpy::load("C:\\Users\\ocad\\Documents\\bci_workshop\\tsneResult.npy", y);

    cnpy::NpyArray t;
    size_t dim, n;
    double* data;
    vector<string> filenames;
    filenames.push_back("C:\\Users\\ocad\\Documents\\bci_workshop\\t0.npy");
    for (auto filename : filenames)
    {
        t = cnpy::npy_load(filename);
        ofxNumpy::getSize(t, dim, n);
        data = t.data<double>();
        for (int i = 0; i < n / dim; i++)
        {
            vector<float> feat_vector(dim);
            for (int j = 0; j < dim; j++)
            {
                feat_vector.at(j) = *data;
                data++;
            }
            feat_matrix.push_back(feat_vector);
        }
    }

    float minimum = 1000;
    float maximum = -1000;
    for (int i = 0; i < feat_matrix.size(); i++)
    {
        float a = 0;
//        a += feat_matrix.at(i).at(8);
//        a += feat_matrix.at(i).at(9);
//        a += feat_matrix.at(i).at(10);
//        a += feat_matrix.at(i).at(11);
        alphas.push_back(a);
        minimum = min(minimum, a);
        maximum = max(maximum, a);
    }
    ofLogError() << minimum << " " << maximum;
    ofLogError() << feat_matrix.at(0).at(0);

    ofDisableArbTex();
    softPoint.loadImage(ofToDataPath("Soft64.png"));
    softPoint.resize(16, 16);
    ofEnableArbTex();

    minXY.x = maxXY.x = y.at(0).x;
    minXY.y = maxXY.y = y.at(0).y;
    for (auto& p : y)
    {
        minXY.x = min(minXY.x, p.x);
        maxXY.x = max(maxXY.x, p.x);
        minXY.y = min(minXY.y, p.y);
        maxXY.y = max(maxXY.y, p.y);
    }
    ofLogError() << minXY << " " << maxXY;

    strings.setMode(OF_PRIMITIVE_LINES);
    stringsNew.setMode(OF_PRIMITIVE_LINES);
    for (int i = 0; i < y.size(); i++)
    {
        for (int j = i; j < y.size(); j++)
        {
            auto p0 = y.at(i);
            auto p1 = y.at(j);
            p0.x = ofMap(p0.x, minXY.x, maxXY.x, -width * 0.5f + 10, width * 0.5f - 10);
            p0.y = ofMap(p0.y, minXY.y, maxXY.y, -height * 0.5f + 10, height * 0.5f - 10);
            p1.x = ofMap(p1.x, minXY.x, maxXY.x, -width * 0.5f + 10, width * 0.5f - 10);
            p1.y = ofMap(p1.y, minXY.y, maxXY.y, -height * 0.5f + 10, height * 0.5f - 10);
            float dist = p0.distance(p1);
            float distThreshold = 100;
            if (dist < distThreshold)
            {
                strings.addVertex(p0);
                strings.addVertex(p1);
                //strings.addColor(ofFloatColor::black);
                //strings.addColor(ofFloatColor::black);
                strings.addColor(ofFloatColor::fromHsb((float)i / y.size() * 0.75f, 1, 1, ofMap(dist, distThreshold, 0, 0, 0.5f)));
                strings.addColor(ofFloatColor::fromHsb((float)j / y.size() * 0.75f, 1, 1, ofMap(dist, distThreshold, 0, 0, 0.5f)));
                strings.addIndex(strings.getNumVertices() - 2);
                strings.addIndex(strings.getNumVertices() - 1);
            }
        }
    }

    gui.setup();
    gui.add(sliderChannel.setup("Channel", 0, 0, 15));
    gui.add(sliderUpperLimit.setup("Upper Limit", 2, 0, 6));
    gui.add(toggleColor.setup("Color", false));
    gui.add(refreshSec.setup("Refresh Sec", 5, 1, 20));
    gui.add(mouseDebug.setup("Mouse Debug", false));
    gui.add(distThreshold.setup("Distance", 150, 50, 300));
    gui.add(lineAlpha.setup("Line Alpha", 0.5f, 0, 1));
    gui.loadFromFile("settings.xml");
    drawGui = false;

    fbo.allocate(width * 2, height * 2, GL_RGB);

    ofSetWindowPosition(1920 + 400, -50);
    ofSetWindowShape(1024 + 200, 768 + 200);
    //ofSetFullscreen(true);
}

//--------------------------------------------------------------
void ofApp::update(){
    if (mouseDebug && ofGetFrameNum() % 15 == 0)
    {
        ofVec2f newPos;
        newPos.x = mouseX;
        newPos.y = mouseY;
        newPosPrev = newPos;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofEnableAlphaBlending();

    //ofBackgroundGradient(ofColor(40, 40, 40), ofColor(0, 0, 0), OF_GRADIENT_CIRCULAR);
    ofBackground(0, 255);
    //ofSetColor(ofFloatColor::white);
    //ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

    int count = 0;
    ofVec2f pPrev;

    ofFloatColor c;

    ofSetLineWidth(2);

    ofPushMatrix();
    ofTranslate(ofGetWidth() * 0.5f, ofGetHeight() * 0.5f);
    stringsNew.draw();
    
    for (auto& p : y)
    {
        //ofSetColor(ofFloatColor::fromHsb((float)count / y.size(), 1, 1, 0.1));
        //ofDrawCircle(p, 5 * 0.1f);

        float radius = 5;
        if(toggleColor)
            ofSetColor(ofFloatColor::fromHsb(0, 1, 1, ofMap(feat_matrix.at(count).at(sliderChannel), 0, sliderUpperLimit, 0, 0.2f, true)));
        else
            ofSetColor(ofFloatColor::fromHsb((float)count / y.size() * 0.75f, 1, 1, 0.5f));

        ofVec2f newPos;
        newPos.x = ofMap(p.x, minXY.x, maxXY.x, -width * 0.5f + 10, width * 0.5f - 10);
        newPos.y = ofMap(p.y, minXY.y, maxXY.y, -height * 0.5f + 10, height * 0.5f - 10);

        ofDrawCircle(newPos, radius);
        //softPoint.draw(newPos, radius * 2, radius * 2);
        //if(count > 0)
        //    ofLine(pPrev, newPos);
        //pPrev = newPos;

        count++;
    }
    /*
    count = 0;
    for (auto& p : yNew)
    {
        float radius = 10;
        if (toggleColor)
            ofSetColor(ofFloatColor::fromHsb(0, 1, 1, ofMap(feat_matrix.at(count).at(sliderChannel), 0, sliderUpperLimit, 0, 0.2f, true)));
        else
            ofSetColor(ofFloatColor::fromHsb((float)count / (refreshSec * 10) * 0.75f, 1, 1, 0.2f));

        ofVec2f newPos;
        newPos.x = ofMap(p.x, minXY.x, maxXY.x, -width * 0.5f + 10, width * 0.5f - 10);
        newPos.y = ofMap(p.y, minXY.y, maxXY.y, -height * 0.5f + 10, height * 0.5f - 10);

        //softPoint.draw(newPos, radius * 2, radius * 2);
        count++;
    }
    */
    ofPopMatrix();

    if(drawGui)
        gui.draw();

    if (yNew.size() >= refreshSec * 10)
    {
        fbo.begin();
        ofEnableAlphaBlending();
        ofEnableAntiAliasing();

        auto start = ofColor(40, 40, 40), end = ofColor(0, 0, 0);
        ofVboMesh gradientMesh;
        float w = width * 2, h = height * 2;
        gradientMesh.clear();
        gradientMesh.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
        gradientMesh.setUsage(GL_STREAM_DRAW);
            // this could be optimized by building a single mesh once, then copying
            // it and just adding the colors whenever the function is called.
            ofVec2f center(w / 2, h / 2);
            gradientMesh.addVertex(center);
            gradientMesh.addColor(start);
            int n = 32; // circular gradient resolution
            float angleBisector = TWO_PI / (n * 2);
            float smallRadius = ofDist(0, 0, w / 2, h / 2);
            float bigRadius = smallRadius / cos(angleBisector);
            for (int i = 0; i <= n; i++) {
                float theta = i * TWO_PI / n;
                gradientMesh.addVertex(center + ofVec2f(sin(theta), cos(theta)) * bigRadius);
                gradientMesh.addColor(end);
            }
        GLboolean depthMaskEnabled;
        glGetBooleanv(GL_DEPTH_WRITEMASK, &depthMaskEnabled);
        glDepthMask(GL_FALSE);
        gradientMesh.draw();
        if (depthMaskEnabled) {
            glDepthMask(GL_TRUE);
        }

        ofTranslate(width * 0.5f * 2, height * 0.5f * 2);
        ofScale(2, 2);

        count = 0;
        for (auto& p : y)
        {
            ofSetColor(ofFloatColor::fromHsb((float)count / y.size() * 0.75f, 1, 1, 0.5f));

            ofVec2f newPos;
            newPos.x = ofMap(p.x, minXY.x, maxXY.x, -width * 0.5f + 10, width * 0.5f - 10);
            newPos.y = ofMap(p.y, minXY.y, maxXY.y, -height * 0.5f + 10, height * 0.5f - 10);
            float radius = 5;
            ofDrawCircle(newPos, radius);
            count++;
        }

        ofSetColor(255, 255);
        stringsNew.draw();
        fbo.end();

        ofImage image;
        image.allocate(width * 2, height * 2, OF_IMAGE_COLOR);
        fbo.readToPixels(image.getPixels());
        image.resize(width, height);
        image.update();
        image.save(ofGetTimestampString() + ".png");
        yNew.clear();
        stringsNew.clear();
        stringsNew.setMode(OF_PRIMITIVE_LINES);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == OF_KEY_TAB)
    {
        drawGui = !drawGui;
        if (drawGui)
            ofShowCursor();
        else
            ofHideCursor();
    }
    if (key == 'f')
    {
        ofToggleFullscreen();
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
