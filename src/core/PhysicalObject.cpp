/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 *
 */

#include "World/PhysicalObject.h"
#include "RoboroboMain/roborobo.h"
#include "Utilities/Misc.h"
#include "World/World.h"

PhysicalObject::PhysicalObject( int __id ) // a unique and consistent __id should be given as argument
{
    _id = __id;

    this->lastRelocationDate = gWorld->getIterations();
    //std::cout << "[DEBUG]" << lastRelocationDate << "\n";
    registered = false;
    init();
}

void PhysicalObject::init()
{
    double x = 0.0, y = 0.0;
    
	std::string s = "";
	std::stringstream out;
	out << getId();
    
    regrowTime = 0;
    
    s = "physicalObject[";
	s += out.str();
	s += "].regrowTimeMax";
	if ( gProperties.hasProperty( s ) )
	{
		convertFromString<int>(regrowTimeMax, gProperties.getProperty( s ), std::dec);
	}
    else
    {
        regrowTimeMax = gPhysicalObjectDefaultRegrowTimeMax;
    }
    
    s = "physicalObject[";
	s += out.str();
	s += "].overwrite";
	if ( gProperties.hasProperty( s ) )
        gProperties.checkAndGetPropertyValue(s,&overwrite,true);
    else
    {
        overwrite = gPhysicalObjectDefaultOverwrite;
    }
    
    s = "physicalObject[";
	s += out.str();
	s += "].relocate";
	if ( gProperties.hasProperty( s ) )
        gProperties.checkAndGetPropertyValue(s,&relocateObject,true);
    else
    {
        relocateObject = gPhysicalObjectDefaultRelocate;
    }
    
    s = "physicalObject[";
	s += out.str();
	s += "].visible";
	if ( gProperties.hasProperty( s ) )
        gProperties.checkAndGetPropertyValue(s,&_visible,true);
	else
        gProperties.checkAndGetPropertyValue("gPhysicalObjectsVisible", &_visible, true);
    
    Uint32 colorValue;
    
    s = "physicalObject[";
	s += out.str();
	s += "].displayColorRed";
	if ( gProperties.hasProperty( s ) )
	{
		convertFromString<Uint32>(colorValue, gProperties.getProperty( s ), std::dec);
        _displayColorRed = (colorValue & 0x000000FF);
	}
    else
    {
        _displayColorRed = gPhysicalObjectDefaultDisplayColorRed; // default
    }
    
    s = "physicalObject[";
	s += out.str();
	s += "].displayColorGreen";
	if ( gProperties.hasProperty( s ) )
	{
		convertFromString<Uint32>(colorValue, gProperties.getProperty( s ), std::dec);
        _displayColorGreen = (colorValue & 0x000000FF);
	}
    else
    {
        _displayColorGreen = gPhysicalObjectDefaultDisplayColorGreen; // default
    }
    
    s = "physicalObject[";
	s += out.str();
	s += "].displayColorBlue";
	if ( gProperties.hasProperty( s ) )
	{
		convertFromString<Uint32>(colorValue, gProperties.getProperty( s ), std::dec);
        _displayColorBlue = (colorValue & 0x000000FF);
	}
    else
    {
        _displayColorBlue = gPhysicalObjectDefaultDisplayColorBlue; // default
    }
    
    s = "physicalObject[";
	s += out.str();
	s += "].x";
	if ( gProperties.hasProperty( s ) )
	{
		convertFromString<double>(x, gProperties.getProperty( s ), std::dec);
	}
	else
	{
        x = -1.0;
	}
    
	s = "physicalObject[";
	s += out.str();
	s += "].y";
	if ( gProperties.hasProperty( s ) )
	{
		convertFromString<double>(y, gProperties.getProperty( s ), std::dec);
	}
	else
	{
        y = -1.0;
	}

    setCoordinates(x, y, false);
}

int PhysicalObject::findRandomLocation( )
{
    double x = 0.0, y = 0.0;
    
    int tries = 0;
    
    do {
        x = ( randint() % ( gPhysicalObjectsInitAreaWidth  ) ) + gPhysicalObjectsInitAreaX;
        y = ( randint() % ( gPhysicalObjectsInitAreaHeight ) ) + gPhysicalObjectsInitAreaY;
        
        //x = (randint() % (gAreaWidth-20)) + 10;  // deprecated
        //y = (randint() % (gAreaHeight-20)) + 10; // deprecated

        setCoordinates(x, y, false);
        
        tries++;
    } while ( canRegister() == false && tries < gLocationFinderMaxNbOfTrials );
    
    if ( tries == gLocationFinderMaxNbOfTrials )
    {
        if ( gLocationFinderExitOnFail == true )
        {
            std::cerr << "[CRITICAL] Random initialization of initial position for physical object #" << getId() << " after trying " << gLocationFinderMaxNbOfTrials << " random picks (all failed). There may be too few (none?) possible locations (you may try to manually set initial positions). EXITING.\n";
            exit(-1);
        }
        else
        {
            std::cerr << "[WARNING] Random initialization of initial position for physical object #" << getId() << " after trying " << gLocationFinderMaxNbOfTrials << " random picks (all failed). Retry later.\n";
            regrowTime = 1;
            setCoordinates(-1, -1, false);
        }
    }
    
    return tries;
}

void PhysicalObject::stepPhysicalObject()
{
    if ( registered == false && regrowTime != -1 )
    {
        if ( regrowTime > 0 )
        {
            regrowTime--;
        }
        else
        {
            if ( relocateObject == true )
            {
                findRandomLocation(); // fail: exit or return (x,y)=(-1,-1)
                if ( getXReal() != -1 ) // check if new location is possible
                {
                    //std::cout << "REGROW!\n";
                    _visible = true;
                    registered = true;
                    registerObject();
                }
            }
            else
            {
                if ( canRegister() == true || overwrite == true )
                {
                    _visible = true;
                    registered = true;
                    registerObject();
                }
            }
        }
    }
    else
    {
        if ( getXReal() != -1 ) // check if (new) location is possible
            if ( _visible )
            {
                if ( gPhysicalObjectsRedraw == true )
                {
                    registerObject();
                }
            }
    }
}

bool PhysicalObject::isInstanceOf ( int index ) // static
{
    if ( index >= gPhysicalObjectIndexStartOffset && index <  gPhysicalObjectIndexStartOffset + (int)gPhysicalObjects.size() )
        return true;
    else
        return false;
}


/*
 Given an object not registered in the environment, tries to re-register at once.
 Return false (and do nothing) if not possible at desired location.
 Exit if function is called when it should not -- calling this function can be critical, including registering multiple instances of the same object, thus breaking consistency.
*/
bool PhysicalObject::triggerRegrow()
{
    if ( registered == true )  // exit if object is already registered in the environment
    {
        std::cerr << "[CRITICAL] physical object #" << getId() << ": attempt to call triggerRegrow() method while object already exists in the environment. EXITING.\n";
        exit(-1);
    }
    
    if ( canRegister() == false ) // check is position is free.
    {
        return false;
    }
    else
    {
        regrowTime = gPhysicalObjectDefaultRegrowTimeMax;
        _visible = true;
        registered = true;
        registerObject();
    }
    
    return true;
}

std::string PhysicalObject::inspect( std::string prefix )
{
    return std::string(prefix + "PhysicalObject::inspect() not implemented.\n");
}

void PhysicalObject::relocate()
{
    if ( getXCenterPixel() != -1.0 && getYCenterPixel() != -1.0 ) // if registered
        unregisterObject();
    
    findRandomLocation();
    
    if ( getXCenterPixel() != -1.0 && getYCenterPixel() != -1.0 ) // not registered
    {
        registerObject();
    }
}

bool PhysicalObject::relocate(double x, double y )
{
    int backup_x = getXCenterPixel();
    int backup_y = getYCenterPixel();

    setCoordinates(x, y, false);
    
    if ( canRegister() == true )
    {
        return true;
    }
    else
    {
        setCoordinates(backup_x, backup_y, false);
        return false;
    }
}

void PhysicalObject::registerObject()
{
    //std::cout << "[DEBUG][0]" << lastRelocationDate << "\n";
    this->lastRelocationDate = gWorld->getIterations();
    //std::cout << "[DEBUG][1]" << lastRelocationDate << "\n";
}

int PhysicalObject::getTimestepSinceRelocation()
{
    int a = gWorld->getIterations();
    int b = this->lastRelocationDate;
    return (a - b);
}

void PhysicalObject::setId(int id)
{
    _id = id;
}

bool PhysicalObject::isRegistered()
{
    return registered;
}

