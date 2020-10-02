/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofFluid.h"

t_class *pofFluid_class;

void *pofFluid_new(t_floatarg w,t_floatarg h, t_floatarg down)
{
	if(w == 0) w = 1280;
	if(h == 0) h = 720;
	if(down == 0) down = 4;
	pofFluid* obj = new pofFluid(pofFluid_class, w, h, down);

	floatinlet_new(&obj->pdobj->x_obj, &obj->width);
	floatinlet_new(&obj->pdobj->x_obj, &obj->height);
	return (void*) (obj->pdobj);
}

void pofFluid_free(void *x)
{
	delete (pofFluid*)(((PdObject*)x)->parent);
}

void pofFluid_velocity(void *x, t_symbol *texture, float f)
{
	pofFluid* px = (pofFluid*)(((PdObject*)x)->parent);
	px->velocityTexture = texture;
	px->velocityForce = f;
}

void pofFluid_density(void *x, t_symbol *texture, float f)
{
	pofFluid* px = (pofFluid*)(((PdObject*)x)->parent);
	px->densityTexture = texture;
	px->densityForce = f;
}

void pofFluid_showGui(void *x, float f)
{
	pofFluid* px = (pofFluid*)(((PdObject*)x)->parent);
	px->showGui = (f != 0);
}

pofFluid::pofFluid(t_class *Class, float w, float h, int down):
	pofBase(Class),width(w), height(h), drawWidth(w), drawHeight(h), downSample(down),
	fluidSimulation(NULL), gui(NULL),
	densityTexture(NULL), velocityTexture(NULL), pressureTexture(NULL), obstacleTexture(NULL),
	showGui(false)
{
}

void pofFluid::draw()
{
	if(!fluidSimulation) {
		fluidSimulation = new ftFluidSimulation();
		gui = new ofxPanel();
		int flowWidth = drawWidth / downSample;
		int flowHeight = drawHeight / downSample;

		fluidSimulation->setup(flowWidth, flowHeight, drawWidth, drawHeight);
		setupGui();
	}
	
	if(densityTexture && densityForce) {
		ofTexture *tex = pofBase::textures[densityTexture];
		if(tex) fluidSimulation->addDensity(*tex, densityForce);
	}
	if(velocityTexture && velocityForce) {
		ofTexture *tex = pofBase::textures[velocityTexture];
		if(tex) {
			fluidSimulation->addVelocity(*tex, velocityForce);
		}
	}

	ofPushStyle();
	fluidSimulation->update();
	fluidSimulation->draw(-width/2, -height/2, width, height);
	ofPopStyle();
	
	if(!showGui) return;

	ofPushMatrix();
	ofSetupScreen();
	ofPushStyle();
	ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	gui->draw();
	
	// HACK TO COMPENSATE FOR DISSAPEARING MOUSE
	ofEnableBlendMode(OF_BLENDMODE_SUBTRACT);
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 300.0);
	ofEnableBlendMode(OF_BLENDMODE_ADD);
	ofDrawCircle(ofGetMouseX(), ofGetMouseY(), ofGetWindowWidth() / 600.0);
	ofPopStyle();

	ofPopMatrix();
}

void pofFluid::setupGui() {
	
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
	gui->add(fluidSimulation->parameters);

	gui->minimizeAll();
}

void pofFluid::setup(void)
{
	//post("pofFluid_setup");
	pofFluid_class = class_new(gensym("poffluid"), (t_newmethod)pofFluid_new, (t_method)pofFluid_free,
		sizeof(PdObject), 0, A_DEFFLOAT, A_DEFFLOAT, A_DEFFLOAT, A_NULL);
	class_addmethod(pofFluid_class, (t_method)pofFluid_density, gensym("density"), A_SYMBOL, A_FLOAT, A_NULL);
	class_addmethod(pofFluid_class, (t_method)pofFluid_velocity, gensym("velocity"), A_SYMBOL, A_FLOAT, A_NULL);
	class_addmethod(pofFluid_class, (t_method)pofFluid_showGui, gensym("showgui"), A_FLOAT, A_NULL);
	class_addmethod(pofFluid_class, (t_method)tellGUI, gensym("param"), A_GIMME, A_NULL);
	POF_SETUP(pofFluid_class);
}

void pofFluid::message(int argc, t_atom *argv)
{
	t_atom ap[4];

	t_symbol *key = atom_getsymbol(argv); 
	argv++; argc--;
	ofParameterGroup group = fluidSimulation->parameters;
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
