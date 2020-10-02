/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofOpticalFlow.h"

t_class *pofOpticalFlow_class;

void *pofOpticalFlow_new(t_floatarg w,t_floatarg h)
{
	if(w == 0) w = 1280;
	if(h == 0) h = 720;
	pofOpticalFlow* obj = new pofOpticalFlow(pofOpticalFlow_class, w, h);

	floatinlet_new(&obj->pdobj->x_obj, &obj->width);
	floatinlet_new(&obj->pdobj->x_obj, &obj->height);
	return (void*) (obj->pdobj);
}

void pofOpticalFlow_free(void *x)
{
	delete (pofOpticalFlow*)(((PdObject*)x)->parent);
}

void pofOpticalFlow_velocity(void *x, t_symbol *texture)
{
	pofOpticalFlow* px = (pofOpticalFlow*)(((PdObject*)x)->parent);
	px->velocityTexture = texture;
}

void pofOpticalFlow_decay(void *x, t_symbol *texture)
{
	pofOpticalFlow* px = (pofOpticalFlow*)(((PdObject*)x)->parent);
	px->decayTexture = texture;
}

void pofOpticalFlow_source(void *x, t_symbol *texture)
{
	pofOpticalFlow* px = (pofOpticalFlow*)(((PdObject*)x)->parent);
	px->sourceTexture = texture;
}

void pofOpticalFlow_showGui(void *x, float f)
{
	pofOpticalFlow* px = (pofOpticalFlow*)(((PdObject*)x)->parent);
	px->showGui = (f != 0);
}

pofOpticalFlow::pofOpticalFlow(t_class *Class, float w, float h):
	pofBase(Class),width(w), height(h), drawWidth(w), drawHeight(h),
	opticalFlow(NULL),
	sourceTexture(NULL), velocityTexture(NULL), decayTexture(NULL),
	showGui(false)
{
}

void pofOpticalFlow::draw()
{
	if(!opticalFlow) {
		opticalFlow = new ftOpticalFlow();
		gui = new ofxPanel();
		opticalFlow->setup(drawWidth, drawHeight);
		setupGui();
	}
	
	if(sourceTexture) {
		ofTexture *tex = pofBase::textures[sourceTexture];
		if(tex) {
			opticalFlow->setSource(*tex);
		}
	}

	ofPushStyle();
	opticalFlow->update();
	if(velocityTexture) {
		pofBase::textures[velocityTexture] = &opticalFlow->getOpticalFlow();
	}
	if(decayTexture) {
		pofBase::textures[decayTexture] = &opticalFlow->getOpticalFlowDecay();
	}
	ofPopStyle();
	
	if(!showGui) return;

	ofPushMatrix();
	ofSetupScreen();
	//ofTranslate(-ofGetWindowWidth()/2,-ofGetWindowHeight()/2);
	//ofTranslate(0,0,1.0);
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gui->draw();
	
	// HACK TO COMPENSATE FOR DISSAPEARING MOUSE
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 300.0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 600.0);
	ofPopStyle();

	//ofTranslate(ofGetWindowWidth()/2,ofGetWindowHeight()/2);
	ofPopMatrix();
}

void pofOpticalFlow::setupGui() {
	
	gui->setup("settings");
	gui->setDefaultBackgroundColor(ofColor(0, 0, 0, 127));
	gui->setDefaultFillColor(ofColor(160, 160, 160, 160));
	
	int guiColorSwitch = 0;
	ofColor guiHeaderColor[2];
	guiHeaderColor[0].set(160, 160, 80, 200);
	guiHeaderColor[1].set(80, 160, 160, 200);
	ofColor guiFillColor[2];
	guiFillColor[0].set(160, 160, 80, 200);
	guiFillColor[1].set(80, 160, 160, 200);
	
	gui->setDefaultHeaderBackgroundColor(guiHeaderColor[guiColorSwitch]);
	gui->setDefaultFillColor(guiFillColor[guiColorSwitch]);
	guiColorSwitch = 1 - guiColorSwitch;
	gui->add(opticalFlow->parameters);
	
	gui->minimizeAll();
}

void pofOpticalFlow::message(int argc, t_atom *argv)
{
	t_atom ap[4];

	t_symbol *key = atom_getsymbol(argv); 
	argv++; argc--;
	ofParameterGroup group = opticalFlow->parameters;
	while(argc > 1 && group.contains(atom_getsymbol(argv)->s_name)) {
		ofAbstractParameter &parameter = group.get(atom_getsymbol(argv)->s_name);
		if(parameter.type() == typeid(ofParameterGroup).name()) {
			group = group.getGroup(atom_getsymbol(argv)->s_name);
			argc--;
			argv++;
		} else {
			if(parameter.type() == typeid(ofParameter<float>).name()) parameter.cast<float>() = atom_getfloat(argv + 1);
			else if(parameter.type() == typeid(ofParameter<int>).name()) parameter.cast<int>() = atom_getfloat(argv + 1);
			else if(parameter.type() == typeid(ofParameter<bool>).name()) parameter.cast<bool>() = atom_getfloat(argv + 1);
			break;
		}
	}
}

void pofOpticalFlow::setup(void)
{
	//post("pofOpticalFlow_setup");
	pofOpticalFlow_class = class_new(gensym("pofopticalflow"), (t_newmethod)pofOpticalFlow_new, (t_method)pofOpticalFlow_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofOpticalFlow_class, (t_method)pofOpticalFlow_source, gensym("source"), A_SYMBOL, A_NULL);
	class_addmethod(pofOpticalFlow_class, (t_method)pofOpticalFlow_decay, gensym("decay"), A_SYMBOL, A_NULL);
	class_addmethod(pofOpticalFlow_class, (t_method)pofOpticalFlow_velocity, gensym("velocity"), A_SYMBOL, A_NULL);
	class_addmethod(pofOpticalFlow_class, (t_method)pofOpticalFlow_showGui, gensym("showgui"), A_FLOAT, A_NULL);
	class_addmethod(pofOpticalFlow_class, (t_method)tellGUI, gensym("param"), A_GIMME, A_NULL);
	POF_SETUP(pofOpticalFlow_class);
}


