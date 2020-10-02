/*
 * Copyright (c) 2014 Antoine Rousseau <antoine@metalu.net>
 * BSD Simplified License, see the file "LICENSE.txt" in this distribution.
 * See https://github.com/Ant1r/ofxPof for documentation and updates.
 */
#include "pofFluid.h"
#include "pofOpticalFlow.h"

extern "C" {
	void pofflowtools_setup(void)
	{
		pofFluid::setup();
		pofOpticalFlow::setup();
	}
}


