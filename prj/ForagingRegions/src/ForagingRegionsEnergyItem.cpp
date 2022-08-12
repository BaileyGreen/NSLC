//
//  ForagingRegionsEnergyItem.cpp
//  roborobo3
//
//  Created by Nicolas Bredeche on 06/12/17.
//  Copyright © 2017 Nicolas Bredeche. All rights reserved.
//

#include "ForagingRegions/include/ForagingRegionsEnergyItem.h"
#include "RoboroboMain/roborobo.h"
#include "World/World.h"
#include "WorldModels/RobotWorldModel.h"
#include "Utilities/Misc.h"
#include "ForagingRegions/include/ForagingRegionsSharedData.h"

double leftRegionDensity = 0.99;

ForagingRegionsEnergyItem::ForagingRegionsEnergyItem( int __id ) : EnergyItem( __id )
{
    
}

void ForagingRegionsEnergyItem::step()
{
    EnergyItem::step();
 
    if ( activeIt == TemplateEESharedData::gEvaluationTime * 2 ) // relocate after xxx iterations (if not harvested inbetween)
    {
        relocate(); // incl. activeIt=0
        _visible = true;
    }
}

void ForagingRegionsEnergyItem::isTouched( int __idAgent )
{
    EnergyItem::isTouched(__idAgent);
}

void ForagingRegionsEnergyItem::isWalked( int __idAgent )
{
    EnergyItem::isWalked(__idAgent);

    relocate();
    _visible = true;
}

void ForagingRegionsEnergyItem::isPushed( int __id, std::tuple<double, double> __speed )
{
    EnergyItem::isPushed(__id,__speed);
}

void ForagingRegionsEnergyItem::setRegion( double offset, double range )
{
    _offsetRegion = offset;
    _range = range;
}

void ForagingRegionsEnergyItem::relocate()
{
    // * pick new coordinate
    
    unregisterObject();
    
    int border = 40;
    
    //double pi = atan(1)*4;
    
    do{
        
        double xPos;
        
        xPos = random01() * _range + _offsetRegion;
        
        // with a sigmoid
        //double value = random01();
        //xPos = (1/(1 + std::exp(-value*5 + 13)))*3000; // (1/(1 + Exp[-x*5 + 13])*3000) <===
        //xPos = std::pow(value,3); // x^3   <==
        //xPos = (1/(1 + std::exp(-value*5 + 5)))*2; // (1/(1 + Exp[-x*5 + 5]))
        //xPos = 1 - ( 1 / (1 + std::exp( -value * 10 + 5))); // 1 - (1/(1 + Exp[-x*10 + 5]))
        
        // with a gaussian
        //double sigma = 0.2;
        //double gaussianPeakValue = 1.0 / std::sqrt( 2. * pi * std::pow(sigma,2) );
        //xPos = sigma*randgaussian() / gaussianPeakValue;
        
        double x = xPos * ( gArenaWidth - 2*border ) + border;
        double y = random01() * ( gArenaHeight - 2*border ) + border;

        setCoordinates(x, y, false);
        
    } while ( canRegister() == false );
    
    registerObject();
    
    activeIt=0;
}
