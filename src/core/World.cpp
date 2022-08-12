/*
 *  World.cpp
 *  roborobo
 *
 *  Created by Nicolas on 16/01/09.
 *
 */

#include "World/World.h"
#include "zsu/Properties.h"
#include "Config/GlobalConfigurationLoader.h"
#include "Utilities/Graphics.h"
#include "RoboroboMain/roborobo.h"
#include "Observers/WorldObserver.h"
#include <iostream>
#include <vector>

//#include "tbb/blocked_range.h"
//#include "tbb/parallel_for.h"
//#include "tbb/task_scheduler_init.h"

/*
struct executor
{
    std::vector<int>& _vect;
    executor(std::vector<int>& t) : _vect(t) {}
    //    executor(executor& e,tbb::split):_tasks(e._tasks) {}
    
    void operator()(const tbb::blocked_range<size_t>& r) const
    {
        for (size_t i=r.begin();i!=r.end();++i)
            _vect[i] = 0;
        std::cout << "testing TBB\n";  // will be messed up because of // cout.
    }
};

int initElement() { return (0); }

static void testTBB2()
{
    tbb::task_scheduler_init init;  // Automatic number of threads
    // tbb::task_scheduler_init init(2);  // Explicit number of threads
    
    std::vector<int> vect(100000000); //100000000);
    //std::generate(vect.begin(), vect.end(), initElement);
    
    executor exec(vect);
    tbb::parallel_for(tbb::blocked_range<size_t>(0,vect.size()-1),executor(vect));
    //tbb::parallel_for(size_t(0), size_t(vect.size()),executor(vect));
    std::cerr << std::endl;
}
*/


/********/

World::World()
{
    //testTBB2();
    
	_iterations = 0;
	_agentsVariation = false;
	_worldObserver = gConfigurationLoader->make_WorldObserver(this);
}



World::~World()
{
    /*
     * gRobots are now freed at exit thanks to shared ptr
    for (int i = 0; i != gNbOfRobots; i++)
    {
        if (gRobots[i] != nullptr)
            delete gRobots[i];
    }

    delete _worldObserver;
     */
}

void World::initWorld(std::function<void()> callbackOnceObjectsAreCreated)
{
	// *** load environment and agents files
    if( loadFiles() == false )
	{
		std::cout << "[CRITICAL] cannot load image files." << std::endl;
		exit(-1);
	}

    // * set or test are for initial position of agents and objects
    
    if ( gPhysicalObjectsInitAreaWidth == -1 )
        gPhysicalObjectsInitAreaWidth = gAreaWidth - 20;
    if ( gPhysicalObjectsInitAreaHeight == -1 )
        gPhysicalObjectsInitAreaHeight = gAreaHeight - 20;
    if ( gAgentsInitAreaWidth == -1 )
        gAgentsInitAreaWidth = gAreaWidth;
    if ( gAgentsInitAreaHeight == -1 )
        gAgentsInitAreaHeight = gAreaHeight;
    
    if (
        gPhysicalObjectsInitAreaX < 0 || gPhysicalObjectsInitAreaX > gAreaWidth ||
        gPhysicalObjectsInitAreaY < 0 || gPhysicalObjectsInitAreaY > gAreaHeight ||
        gPhysicalObjectsInitAreaWidth <= 0 || gPhysicalObjectsInitAreaWidth > gPhysicalObjectsInitAreaX + gAreaWidth ||
        gPhysicalObjectsInitAreaHeight <= 0 || gPhysicalObjectsInitAreaHeight > gPhysicalObjectsInitAreaY + gAreaHeight ||
        gAgentsInitAreaX < 0 || gAgentsInitAreaX > gAreaWidth ||
        gAgentsInitAreaY < 0 || gAgentsInitAreaY > gAreaHeight ||
        gAgentsInitAreaWidth < 0 || gAgentsInitAreaWidth > gAgentsInitAreaX + gAreaWidth ||
        gAgentsInitAreaHeight < 0 || gAgentsInitAreaHeight > gAgentsInitAreaY + gAreaHeight
        )
    {
        std::cerr << "[ERROR] Incorrect values for *InitArea* parameters for agents and/or objects.\n";
        exit(-1);
    }
    
    
    // *** create robot mask from image
    
    // * Analyse agent mask and make it into a list of coordinates
    
    int nbPointsInMask = 0;
    
    // count number of significant pixels in mask.
    
    for ( int i = 0 ; i != gRobotWidth ; i++ )
        for ( int j = 0 ; j != gRobotHeight ; j++ )
            if ( getPixel32( gRobotMaskImage , i , j) != SDL_MapRGBA( gRobotMaskImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) )
            {
                nbPointsInMask++;
            }
    
    gRobotMaskData.resize(nbPointsInMask);
    for ( int i = 0 ; i != nbPointsInMask ; i++)
        (gRobotMaskData.at(i)).reserve(2);
    
    // count number of significant pixels in mask.
    
    int currentIndex = 0;
    for ( int i = 0 ; i != gRobotWidth ; i++ )
        for ( int j = 0 ; j != gRobotHeight ; j++ )
            if ( getPixel32( gRobotMaskImage , i , j) != SDL_MapRGBA( gRobotMaskImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) )
            {
                gRobotMaskData[currentIndex][0]=i;
                gRobotMaskData[currentIndex][1]=j;
                currentIndex++;
            }

    
    // *** Set up the world with landmarks, objects and robots
    
    _worldObserver->initPre();
    
    for ( int i = 0 ; i != gNbOfLandmarks ; i++)
    {
        gLandmarks.push_back(std::make_shared<LandmarkObject>());
    }
    
    for ( int i = 0 ; i != gNbOfPhysicalObjects ; i++)
    {
        PhysicalObjectFactory::makeObject();
    }

    for (int i = 0; i != gInitialNumberOfRobots; i++)
    {
        auto robot = std::make_shared<Robot>(this);
        this->addRobot(robot);
    }

    callbackOnceObjectsAreCreated();
    _worldObserver->initPost();
}

void World::updateWorld(const Uint8 *_keyboardStates)
{
    // Remark: objects and agents are updated *in random order* so as to avoid "effect" order (e.g. low index agents moves first).
    //    This is very important to avoid possible nasty effect from ordering such as "agents with low indexes moves first"
    //    outcome: among many iterations, the effect of ordering is reduced.
    //    This means that roborobo is turn-based, with stochastic update ordering within one turn


    // * update physical object, if any (in random order)

    int shuffledObjectIndex[gNbOfPhysicalObjects];
    for (int i = 0; i < gNbOfPhysicalObjects; i++)
    {
        shuffledObjectIndex[i] = i;
    }

    std::shuffle(&shuffledObjectIndex[0], &shuffledObjectIndex[gNbOfPhysicalObjects], engine);
    //std::random_shuffle(&shuffledObjectIndex[0], &shuffledObjectIndex[gNbOfPhysicalObjects]); // [!n] remove after 2018-5-1 - random_shuffle was not seeded, and uses rand() - thanks Amine.

    for (int i = 0; i < gNbOfPhysicalObjects; i++)
    {
        gPhysicalObjects[shuffledObjectIndex[i]]->step();
    }

    // * update landmark object, if any

    for (auto it = gLandmarks.begin(); it < gLandmarks.end(); it++)
    {
        (*it)->step();
    }

    // * update world level observer (*before* updating agent state and location)

    _worldObserver->stepPre();

    // * update agents

    int shuffledRobotIndex[gNbOfRobots];
    for (int i = 0; i < gNbOfRobots; i++)
    {
        shuffledRobotIndex[i] = i;
    }

    std::shuffle(&shuffledRobotIndex[0], &shuffledRobotIndex[gNbOfRobots], engine);
    //std::random_shuffle(&shuffledRobotIndex[0], &shuffledRobotIndex[gNbOfRobots]); // [!n] remove after 2018-5-1 - random_shuffle was not seeded, and uses rand() - thanks Amine.



    // update agent level observers
    for (int i = 0; i != gNbOfRobots; i++)
    {
        gRobots[shuffledRobotIndex[i]]->callObserverPre();
    }

    // controller step
    for (int i = 0; i < gNbOfRobots; i++)
    {
        if (_keyboardStates == nullptr)
            gRobots[shuffledRobotIndex[i]]->stepBehavior();
        else
        {
            if (shuffledRobotIndex[i] == gRobotIndexFocus)
                gRobots[shuffledRobotIndex[i]]->stepBehavior(_keyboardStates);
            else
                gRobots[shuffledRobotIndex[i]]->stepBehavior();
        }
    }

    // move the agent -- apply (limited) physics
    for (int i = 0; i < gNbOfRobots; i++)
    {
        // unregister itself (otw: own sensor may see oneself)
        if (gRobots[shuffledRobotIndex[i]]->isRegistered())
        {
            gRobots[shuffledRobotIndex[i]]->unregisterRobot();
        }

        // move agent
        gRobots[shuffledRobotIndex[i]]->move();
        
        // register robot (remark: always register is fine with small robots and/or high density)
        gRobots[shuffledRobotIndex[i]]->registerRobot();
    }
    
    // update agent level observers (post robots' move)
    for ( int i = 0 ; i != gNbOfRobots ; i++ )
        gRobots[shuffledRobotIndex[i]]->callObserverPost();

    
    // * update world level observer (*after* updating agent state and location)
    
    _worldObserver->stepPost();
    
    gLogManager->flush();
    
	_iterations++;

}


bool World::loadFiles()
{
	bool returnValue = true;

    // Load the dot image
    
    gRobotMaskImage = load_image( gRobotMaskImageFilename );
    gRobotDisplayImage = load_image( gRobotDisplayImageFilename );
	
	// Load the agent specifications image
    
	gRobotSpecsImage = load_image( gRobotSpecsImageFilename ); // no jpg (color loss)	

    // Load the foreground image (active borders)
    
    gForegroundImage = load_image( gForegroundImageFilename );
	if ( gForegroundImageFilename.compare(gForegroundImageFilename.length()-3, 3, "jpg", 0, 3) == 0 )
	{
		std::cerr << "[ERROR] foreground: BMP format is *mandatory*\n";
		returnValue = false;
	}

    gEnvironmentImage = load_image(gEnvironmentImageFilename);
    if (gEnvironmentImageFilename.compare(gEnvironmentImageFilename.length() - 3, 3, "jpg", 0, 3) == 0)
    {
        std::cerr << "[ERROR] environment: BMP format is *mandatory*\n";
        returnValue = false;
    }

    if ( gEnvironmentImage->h != gArenaHeight || gEnvironmentImage->w != gArenaWidth )
    {
        std::cerr << "[ERROR] gArenaWidth=" << gArenaWidth <<" and gArenaHeight="<< gArenaHeight << " do not match Environment image dimensions (" << gEnvironmentImage->w << "," << gEnvironmentImage->h <<")\n";
        returnValue = false;
    }

    //gTrajectoryMonitorImage = load_image( gEnvironmentImageFilename ); // prepare for logging trajectories (useful if requested in the config file)   ---- // Created in roborobo::initTrajectoriesMonitor

    // load background image

    gBackgroundImage = load_image(gBackgroundImageFilename);

    // Load the ground type image

    gFootprintImage = load_image(gFootprintImageFilename);
    gFootprintImageBackup = load_image(gFootprintImageFilename); // backup for rewriting original values

    // Managing problems with loading files (agent mask and specs)

    if (gRobotMaskImage == nullptr)
    {
        std::cerr << "[ERROR] Could not load agent mask image\n";
        returnValue = false;
    }

    if (gRobotDisplayImage == nullptr)
    {
        std::cerr << "[ERROR] Could not load agent display image\n";
        returnValue = false;
    }

    if (gRobotSpecsImage == nullptr)
    {
        std::cerr << "[ERROR] Could not load agent specification image\n";
        returnValue = false;
    }

    //If there was a problem in loading the foreground image
    if (gForegroundImage == nullptr)
    {
        std::cerr << "[ERROR] Could not load foreground image\n";
        returnValue = false;
    }

    if (gEnvironmentImage == nullptr)
    {
        std::cerr << "[ERROR] Could not load environment image\n";
        returnValue = false;
    }


    //no background image (not a critical error)
    if (gBackgroundImage == nullptr)
    {
        std::cout << "[WARNING] could not load background image (will proceed anyway)\n";
    }

    // mandatory: image dimensions must be more than display screen dimensions (otw: underfitting)
    if (gForegroundImage->w < gArenaWidth || gForegroundImage->h < gArenaHeight)
    {
        std::cerr << "[ERROR] foreground image dimensions must be " << gArenaWidth << "x" << gArenaHeight
                  << " or higher (given: " << gForegroundImage->w << "x" << gForegroundImage->h << ") \n";
        returnValue = false;
    }

    //If there was a problem in loading the ground type image
    if (gFootprintImage == nullptr)
    {
        std::cerr << "[ERROR] Could not load ground image\n";
        returnValue = false;
    }
    else
    {
        if ((gFootprintImage->w != gForegroundImage->w) || (gFootprintImage->h != gForegroundImage->h))
        {
            std::cerr << "[ERROR] Ground image dimensions do not match that of the foreground image\n";
            returnValue = false;
        }
    }
	
	// set reference dimensions
	gRobotWidth = gRobotMaskImage->w ;
	gRobotHeight = gRobotMaskImage->h ;
	
	if ( gMaxTranslationalSpeed > gRobotWidth || gMaxTranslationalSpeed > gRobotHeight )
	{
		std::cerr << "[ERROR] gMaxTranslationalSpeed value *should not* be superior to agent dimensions (image width and/or height) -- may impact collision accuracy (e.g. teleporting through walls)\n";
		returnValue = false;
	}
	
    // set area width&height dimension
    
	gAreaWidth = gForegroundImage->w;
	gAreaHeight = gForegroundImage->h;

	// set transparency color
    
	SDL_SetColorKey( gRobotMaskImage, SDL_TRUE, SDL_MapRGBA( gRobotMaskImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) );
   	SDL_SetColorKey( gRobotDisplayImage, SDL_TRUE, SDL_MapRGBA( gRobotMaskImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) );

	SDL_SetColorKey( gForegroundImage, SDL_TRUE, SDL_MapRGBA( gForegroundImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) );
	SDL_SetColorKey( gEnvironmentImage, SDL_TRUE, SDL_MapRGBA( gEnvironmentImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) );

	// preparing Environment Image (ie. anything not color white is an obstacle)
	for ( int x = 0 ; x != gEnvironmentImage->w ; x++ )
		for ( int y = 0 ; y != gEnvironmentImage->h ; y++ )
		{
			Uint32 pixel = getPixel32(gEnvironmentImage,x,y);
			if ( pixel != SDL_MapRGBA( gEnvironmentImage->format, 0xFF, 0xFF, 0xFF, SDL_ALPHA_OPAQUE ) )
				//putPixel32( gEnvironmentImage, x, y,  SDL_MapRGBA( gEnvironmentImage->format, 0, 0, pixel&0x0000FF, SDL_ALPHA_OPAQUE ) ); // only the blue component is used
                putPixel32( gEnvironmentImage, x, y,  SDL_MapRGBA( gEnvironmentImage->format, 0, 0, 0, SDL_ALPHA_OPAQUE ) ); // BLACK
		}

    //If everything loaded fine
	if ( returnValue == false ) 
		return false;
	else
		return true;
}


std::shared_ptr<Robot> World::getRobot(int index)
{
    return gRobots[index];
}

bool World::isRobotRegistered( int index )
{
	return getRobot(index)->isRegistered();
}

// Cf. World.h for explanation about why this function should NEVER be called -- function is implemented as is only to avoid any further temptation.
void World::deleteRobot (int index )
{
    std::cerr << "[CRITICAL] World::deleteRobot should NEVER be called (nor implemented)\n\t Reason: ordering by index must be kept.\t Workaround: inactivate+unregister robot. Possibly recycle later.\n";
    exit(-1);
}

void World::addRobot(std::shared_ptr<Robot> robot)
{
    gRobots.push_back(robot);
    _agentsVariation = true;
}

void World::deleteLandmark (int __index )
{
	gLandmarks.erase(gLandmarks.begin() + __index );
    gNbOfLandmarks--;
}

void World::addLandmark( std::shared_ptr<LandmarkObject> objectToAdd )
{
	gLandmarks.push_back(objectToAdd);
    gNbOfLandmarks++;
}

int World::getIterations()
{
	return _iterations;
}

std::shared_ptr<WorldObserver> World::getWorldObserver()
{
    return _worldObserver;
}

int World::getNbOfRobots()
{
	return (int)gRobots.size();
}

