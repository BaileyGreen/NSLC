/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 *
 */

#include "World/SquareObject.h"
#include "RoboroboMain/roborobo.h"
#include "Utilities/Graphics.h"
#include "Utilities/Misc.h"

SquareObject::SquareObject( int __id ) : PhysicalObject( __id ) // a unique and consistent __id should be given as argument
{
    _footprintDisplayColorRed = 0xF0;
    _footprintDisplayColorGreen = 0xF0;
    _footprintDisplayColorBlue = 0xF0;

    if (__id >= 0)
    {
        std::string s = "";
        std::stringstream out;
        out << getId();

        s = "physicalObject[";
        s += out.str();
        s += "].solid_w";
        if ( gProperties.hasProperty( s ) )
            convertFromString<int>(_solid_w, gProperties.getProperty( s ), std::dec);
        else
        {
            if ( gVerbose )
                std::cerr << "[MISSING] Physical Object #" << _id << " (of super type SquareObject) missing solid_w (integer, >=0). Get default value.\n";
            gProperties.checkAndGetPropertyValue("gPhysicalObjectDefaultSolid_w", &_solid_w, true);
        }

        s = "physicalObject[";
        s += out.str();
        s += "].solid_h";
        if ( gProperties.hasProperty( s ) )
            convertFromString<int>(_solid_h, gProperties.getProperty( s ), std::dec);
        else
        {
            if ( gVerbose )
                std::cerr << "[MISSING] Physical Object #" << _id << " (of super type SquareObject) missing solid_h (integer, >=0). Get default value.\n";
            gProperties.checkAndGetPropertyValue("gPhysicalObjectDefaultSolid_h", &_solid_h, true);
        }

        s = "physicalObject[";
        s += out.str();
        s += "].soft_w";
        if ( gProperties.hasProperty( s ) )
            convertFromString<int>(_soft_w, gProperties.getProperty( s ), std::dec);
        else
        {
            if ( gVerbose )
                std::cerr << "[MISSING] Physical Object #" << _id << " (of super type SquareObject) missing soft_w (integer, >=0). Get default value.\n";
            gProperties.checkAndGetPropertyValue("gPhysicalObjectDefaultSoft_w", &_soft_w, true);
        }

        s = "physicalObject[";
        s += out.str();
        s += "].soft_h";
        if ( gProperties.hasProperty( s ) )
            convertFromString<int>(_soft_h, gProperties.getProperty( s ), std::dec);
        else
        {
            if ( gVerbose )
                std::cerr << "[MISSING] Physical Object #" << _id << " (of super type SquareObject) missing soft_h (integer, >=0). Get default value.\n";
            gProperties.checkAndGetPropertyValue("gPhysicalObjectDefaultSoft_h", &_soft_h, true);
        }

        double x = 0.0, y = 0.0;
        x = getXCenterPixel();
        y = getYCenterPixel();

        int tries = 0;
        bool randomLocation = false;

        if ( x == -1.0 || y == -1.0 )
        {
            tries = tries + findRandomLocation();
            randomLocation = true;
        }
        else
        {
            if ( canRegister() == false )  // i.e. user-defined location, but cannot register. Pick random.
            {
                std::cerr << "[CRITICAL] cannot use user-defined initial location (" << x << "," << y << ") for physical object #" << getId() << " (localization failed). EXITING.";
                exit(-1);
            }
            randomLocation = false;
        }

        if ( gVerbose )
        {
            std::cout << "[INFO] Physical Object #" << getId() << "  (of super type SquareObject) positioned at ( "<< std::setw(5) << getXCenterPixel() << " , " << std::setw(5) << getYCenterPixel() << " ) -- ";
            if ( randomLocation == false )
                std::cout << "[user-defined position]\n";
            else
            {
                std::cout << "[random pick after " << tries;
                if ( tries <= 1 )
                    std::cout << " try]";
                else
                    std::cout << " tries]";
                std::cout << "\n";
            }
        }

        if ( _visible )
        {
            registerObject();
        }

        registered = true;
    }
}

void SquareObject::show(SDL_Surface *surface) // display on screen (called in the step() method if gDisplayMode=0 and _visible=true)
{
    //  draw footprint

    Uint32 color = SDL_MapRGBA(surface->format,_footprintDisplayColorRed,_footprintDisplayColorGreen,_footprintDisplayColorBlue,SDL_ALPHA_OPAQUE);

    for (Sint16 xColor = getXCenterPixel() - Sint16(_soft_w/2) ; xColor < getXCenterPixel() + Sint16(_soft_w/2) ; xColor++)
    {
        for (Sint16 yColor = getYCenterPixel() - Sint16(_soft_h/2) ; yColor < getYCenterPixel() + Sint16 (_soft_h/2); yColor ++)
        {
            putPixel32_pbc(surface, xColor, yColor,  color);
        }
    }
    
    // draw object
    
    color = SDL_MapRGBA(surface->format,_displayColorRed,_displayColorGreen,_displayColorBlue,SDL_ALPHA_OPAQUE);
    
	for (Sint16 xColor = getXCenterPixel() - Sint16(_solid_w/2) ; xColor < getXCenterPixel() + Sint16(_solid_w/2) ; xColor++)
	{
		for (Sint16 yColor = getYCenterPixel() - Sint16(_solid_h/2) ; yColor < getYCenterPixel() + Sint16 (_solid_h/2); yColor ++)
		{
            putPixel32_pbc(surface, xColor, yColor,  color);
		}
	}
}

void SquareObject::hide()
{
    //  hide footprint (draw white)

    Uint8 r = 0xFF;
    Uint8 g = 0xFF;
    Uint8 b = 0xFF;
    
    Uint32 color = SDL_MapRGBA(gArena->format,r,g,b,SDL_ALPHA_OPAQUE);
    
    for (Sint16 xColor = getXCenterPixel() - Sint16(_soft_w/2) ; xColor < getXCenterPixel() + Sint16(_soft_w/2) ; xColor++)
    {
        for (Sint16 yColor = getYCenterPixel() - Sint16(_soft_h/2) ; yColor < getYCenterPixel() + Sint16 (_soft_h/2); yColor ++)
        {
            putPixel32_pbc(gArena, xColor, yColor,  color);
        }
    }
    
    // hide object (draw white)
    
	for (Sint16 xColor = getXCenterPixel() - Sint16(_solid_w/2) ; xColor < getXCenterPixel() + Sint16(_solid_w/2) ; xColor++)
	{
		for (Sint16 yColor = getYCenterPixel() - Sint16(_solid_h/2) ; yColor < getYCenterPixel() + Sint16 (_solid_h/2); yColor ++)
		{
            putPixel32_pbc(gArena, xColor, yColor,  color);
		}
	}
}

bool SquareObject::canRegister()
{
    // test shape
	for (Sint16 xColor = getXCenterPixel() - Sint16(_solid_w/2) ; xColor < getXCenterPixel() + Sint16(_solid_w/2) ; xColor++)
	{
		for (Sint16 yColor = getYCenterPixel() - Sint16(_solid_h/2) ; yColor < getYCenterPixel() + Sint16 (_solid_h/2); yColor ++)
        {
            Uint32 pixel = getPixel32_pbc( gEnvironmentImage, xColor, yColor);
            if ( pixel != SDL_MapRGBA( gEnvironmentImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) )
                return false; // collision!
        }
    }
    
    //  test footprint (pixels from both ground image and environment image must be empty)
    for (Sint16 xColor = getXCenterPixel() - Sint16(_soft_w/2) ; xColor < getXCenterPixel() + Sint16(_soft_w/2) ; xColor++)
    {
        for (Sint16 yColor = getYCenterPixel() - Sint16(_soft_h/2) ; yColor < getYCenterPixel() + Sint16 (_soft_h/2); yColor ++)
        {
            Uint32 pixelFootprint = getPixel32_pbc( gFootprintImage, xColor, yColor);
            Uint32 pixelEnvironment = getPixel32_pbc( gEnvironmentImage, xColor, yColor);
            if (
                pixelEnvironment != SDL_MapRGBA( gEnvironmentImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) ||
                ( gFootprintImage_restoreOriginal == true  && pixelFootprint != getPixel32_pbc( gFootprintImageBackup, xColor, yColor ) ) || // case: ground as initialized or rewritten (i.e. white)
                ( gFootprintImage_restoreOriginal == false && pixelFootprint != SDL_MapRGBA( gFootprintImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) ) // case: only white ground
                )
                return false; // collision!
        }
    }
    
    return true;
}

void SquareObject::registerObject()
{
    PhysicalObject::registerObject(); // call to superclass method
    
    int id_converted = _id + gPhysicalObjectIndexStartOffset;
    
    //  draw footprint
    
    Uint32 color = SDL_MapRGBA( gFootprintImage->format, (Uint8)((id_converted & 0xFF0000)>>16), (Uint8)((id_converted & 0xFF00)>>8), (Uint8)(id_converted & 0xFF), SDL_ALPHA_OPAQUE );
    
    for (Sint16 xColor = getXCenterPixel() - Sint16(_soft_w/2) ; xColor < getXCenterPixel() + Sint16(_soft_w/2) ; xColor++)
    {
        for (Sint16 yColor = getYCenterPixel() - Sint16(_soft_h/2) ; yColor < getYCenterPixel() + Sint16 (_soft_h/2); yColor ++)
        {
            putPixel32_pbc(gFootprintImage, xColor, yColor,  color);
        }
    }
    
    // draw object
    
    color = SDL_MapRGBA( gEnvironmentImage->format, (Uint8)((id_converted & 0xFF0000)>>16), (Uint8)((id_converted & 0xFF00)>>8), (Uint8)(id_converted & 0xFF), SDL_ALPHA_OPAQUE );
    
	for (Sint16 xColor = getXCenterPixel() - Sint16(_solid_w/2) ; xColor < getXCenterPixel() + Sint16(_solid_w/2) ; xColor++)
	{
		for (Sint16 yColor = getYCenterPixel() - Sint16(_solid_h/2) ; yColor < getYCenterPixel() + Sint16 (_solid_h/2); yColor ++)
        {
            putPixel32_pbc(gEnvironmentImage, xColor, yColor,  color);
        }
    }
	registered = true;
}

void SquareObject::unregisterObject()
{
    //  draw footprint
    
    Uint32 color = SDL_MapRGBA( gFootprintImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE );
    
    for (Sint16 xColor = getXCenterPixel() - Sint16(_soft_w/2) ; xColor < getXCenterPixel() + Sint16(_soft_w/2) ; xColor++)
    {
        for (Sint16 yColor = getYCenterPixel() - Sint16(_soft_h/2) ; yColor < getYCenterPixel() + Sint16 (_soft_h/2); yColor ++)
        {
            if ( gFootprintImage_restoreOriginal == true )
            {
                color = getPixel32_pbc( gFootprintImageBackup, xColor, yColor);
                putPixel32_pbc(gFootprintImage, xColor, yColor,  color);
            }
            else
                putPixel32_pbc(gFootprintImage, xColor, yColor,  color);
        }
    }
    
    // draw object
    
    color = SDL_MapRGBA( gEnvironmentImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE );
    
	for (Sint16 xColor = getXCenterPixel() - Sint16(_solid_w/2) ; xColor < getXCenterPixel() + Sint16(_solid_w/2) ; xColor++)
	{
		for (Sint16 yColor = getYCenterPixel() - Sint16(_solid_h/2) ; yColor < getYCenterPixel() + Sint16 (_solid_h/2); yColor ++)
        {
            putPixel32_pbc(gEnvironmentImage, xColor, yColor,  color);//color);
        }
    }

	registered = false;
}

int SquareObject::getSolidW() const
{
    return _solid_w;
}

void SquareObject::setSolidW(int solidW)
{
    _solid_w = solidW;
}

int SquareObject::getSolidH() const
{
    return _solid_h;
}

void SquareObject::setSolidH(int solidH, bool force)
{
    if (isRegistered() && !force)
    {
        throw std::runtime_error("You must unregister object before changing their shape to avoid unwanted behaviour. "
                                 "You can set force to true if you know what you are doing");
    }
    _solid_h = solidH;
}

int SquareObject::getSoftW() const
{
    return _soft_w;
}

void SquareObject::setSoftW(int softW, bool force)
{
    if (isRegistered() && !force)
    {
        throw std::runtime_error("You must unregister object before changing their shape to avoid unwanted behaviour. "
                                 "You can set force to true if you know what you are doing");
    }
    _soft_w = softW;
}

int SquareObject::getSoftH() const
{
    return _soft_h;
}

void SquareObject::setSoftH(int softH, bool force)
{
    if (isRegistered() && !force)
    {
        throw std::runtime_error("You must unregister object before changing their shape to avoid unwanted behaviour. "
                                 "You can set force to true if you know what you are doing");
    }
    _soft_h = softH;
}
