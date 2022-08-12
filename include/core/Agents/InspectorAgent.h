/*
 *  InspectorAgent.h
 *  roborobo
 *
 *  Created by Nicolas on 17/12/08.
 *  Copyright 2008. All rights reserved.
 *
 */


#ifndef INSPECTORAGENT_H
#define INSPECTORAGENT_H

#include "Agents/Agent.h"

class InspectorAgent : public Agent
{
	private:

	public:
		InspectorAgent();
		virtual ~InspectorAgent() {};
		//InspectorAgent(SDL_Surface *__mask);	// NOT IMPLEMENTED
		
		// controller step
		void stepBehavior() override;
		void stepBehavior(const Uint8* __keyboardStates) override;
    
		void reset() override;
		
		//Moves the dot
		void move(int recursiveIt = 0) override;

		
		//Shows the dot on the screen
		void show(SDL_Surface *surface) override;
		
		/*
		//Sets the camera over the dot
		void set_camera();
	
		void getCoord(int &coord_x, int &coord_y)
		{
			coord_x=x;
			coord_y=y;
		}
	
		void setCoord (int __x, int __y) // agent is centered on point
		{
			x = __x - gDotWidth/2 ; 
			y = __y - gDotHeight/2 ;
		}
		*/
};



#endif /* INSPECTORAGENT_H */

