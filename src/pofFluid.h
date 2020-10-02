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

class pofFluid: public pofBase {
	public:
		pofFluid(t_class *Class, float w = 0, float h = 0, int down = 4);
		~pofFluid() {
			if(gui) delete gui;
			if(fluidSimulation) delete fluidSimulation;
		}
		
		virtual void draw();
		static void setup(void);
		virtual void message(int  arc, t_atom *argv);
		
		float 				width, height; // display size
		int					drawWidth;
		int					drawHeight;
		int					downSample;
		ftFluidSimulation	*fluidSimulation;
		ofxPanel			*gui;

		t_symbol 			*densityTexture;
		t_symbol 			*velocityTexture;
		t_symbol 			*pressureTexture;
		t_symbol 			*obstacleTexture;
		float 				densityForce;
		float 				velocityForce;
		float 				pressureForce;
		bool 				showGui;

		void				setupGui();
		void				drawGui();

};

