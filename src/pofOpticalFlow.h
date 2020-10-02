/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#pragma once

#include "pofBase.h"
#include "ofxFlowTools.h"
#include "ofxGui.h"

using namespace flowTools;

class pofOpticalFlow: public pofBase {
	public:
		pofOpticalFlow(t_class *Class, float w = 0, float h = 0);
		~pofOpticalFlow() {
			if(gui) delete gui;
			if(velocityTexture) pofBase::textures.erase(velocityTexture);
			if(decayTexture) pofBase::textures.erase(decayTexture);
			if(opticalFlow) delete opticalFlow;
		}
		
		virtual void draw();
		static void setup(void);
		virtual void message(int  arc, t_atom *argv);
		
		float 				width, height; // display size
		int					drawWidth;
		int					drawHeight;
		ftOpticalFlow		*opticalFlow;
		ofxPanel			*gui;

		t_symbol 			*sourceTexture;
		t_symbol 			*velocityTexture;
		t_symbol 			*decayTexture;
		bool 				showGui;

		void				setupGui();
		void				drawGui();

};

