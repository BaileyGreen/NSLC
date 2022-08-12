/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 *
 */

#include "TemplateBoids/include/TemplateBoidsWorldObserver.h"
#include "TemplateBoids/include/TemplateBoidsController.h"
#include "World/World.h"
#include "Utilities/Graphics.h"
#include "RoboroboMain/roborobo.h"
#include "WorldModels/RobotWorldModel.h"
#include "TemplateBoids/include/TemplateBoidsSharedData.h"

TemplateBoidsWorldObserver::TemplateBoidsWorldObserver( World* world ) : WorldObserver( world )
{
    _world = world;
    
    // ==== loading project-specific properties

    gProperties.checkAndGetPropertyValue("gEvaluationTime",&TemplateBoidsSharedData::gEvaluationTime,true);

    gProperties.checkAndGetPropertyValue("gMonitorPositions",&TemplateBoidsSharedData::gMonitorPositions,true);
    
    gProperties.checkAndGetPropertyValue("gSnapshots",&TemplateBoidsSharedData::gSnapshots,false);
    gProperties.checkAndGetPropertyValue("gSnapshotsFrequency",&TemplateBoidsSharedData::gSnapshotsFrequency,false);
    
    // * iteration and generation counters
    
    _generationItCount = -1;
    _generationCount = -1;
    
}

TemplateBoidsWorldObserver::~TemplateBoidsWorldObserver()
{
    // nothing to do.
}

void TemplateBoidsWorldObserver::initPre()
{
    // nothing to do.
}

void TemplateBoidsWorldObserver::initPost()
{
    // nothing to do.
}


void TemplateBoidsWorldObserver::stepPre()
{
    _generationItCount++;
    
    if( _generationItCount == TemplateBoidsSharedData::gEvaluationTime+1 ) // switch to next generation.
    {
        // update iterations and generations counters
        _generationItCount = 0;
        _generationCount++;
    }
    
    updateMonitoring();
    
    updateEnvironment();
    
}

void TemplateBoidsWorldObserver::stepPost()
{
    // nothing to do.
}

void TemplateBoidsWorldObserver::updateEnvironment()
{
    // ...
}

void TemplateBoidsWorldObserver::updateMonitoring()
{
    // * Log at end of each generation

    if( gWorld->getIterations() % TemplateBoidsSharedData::gEvaluationTime == 1 || gWorld->getIterations() % TemplateBoidsSharedData::gEvaluationTime == TemplateBoidsSharedData::gEvaluationTime-1 ) // beginning(+1) *and* end of generation. ("==1" is required to monitor the outcome of the first iteration)
    {
        monitorPopulation();
    }
    
    // * Every N generations, take a video (duration: one generation time)
    
    if ( TemplateBoidsSharedData::gSnapshots )
    {
        if ( ( gWorld->getIterations() ) % ( TemplateBoidsSharedData::gEvaluationTime * TemplateBoidsSharedData::gSnapshotsFrequency ) == 0 )
        {
            if ( gVerbose )
                std::cout << "[START] Video recording: generation #" << (gWorld->getIterations() / TemplateBoidsSharedData::gEvaluationTime ) << ".\n";
            gTrajectoryMonitorMode = 0;
            initTrajectoriesMonitor();
        }
        else
            if ( ( gWorld->getIterations() ) % ( TemplateBoidsSharedData::gEvaluationTime * TemplateBoidsSharedData::gSnapshotsFrequency ) == TemplateBoidsSharedData::gEvaluationTime - 1 )
            {
                if ( gVerbose )
                    std::cout << "[STOP]  Video recording: generation #" << (gWorld->getIterations() / TemplateBoidsSharedData::gEvaluationTime ) << ".\n";
                saveTrajectoryImage();
            }
    }    
}

void TemplateBoidsWorldObserver::monitorPopulation( bool localVerbose )
{
    // * monitoring: count number of active agents.
    
    int activeCount = 0;
    for ( int i = 0 ; i != gNbOfRobots ; i++ )
    {
        if ((std::dynamic_pointer_cast<TemplateBoidsController>(gWorld->getRobot(i)->getController()))->getWorldModel()
                    ->isAlive() == true)
            activeCount++;
    }
    
    if ( gVerbose && localVerbose )
    {
        std::cout << "[gen:" << (gWorld->getIterations()/TemplateBoidsSharedData::gEvaluationTime) << ";it:" << gWorld->getIterations() << ";pop:" << activeCount << "]\n";
    }
    
    // Logging, population-level: alive
    std::string sLog = std::string("") + std::to_string(gWorld->getIterations()) + ",pop,alive," + std::to_string(activeCount) + "\n";
    gLogManager->write(sLog);
    gLogManager->flush();
}
