/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 *
 */

#include "ForagingRegions/include/ForagingRegionsController.h"
#include "ForagingRegions/include/ForagingRegionsSharedData.h"
#include "World/World.h"
#include "RoboroboMain/roborobo.h"
#include "WorldModels/RobotWorldModel.h"
#include <algorithm>

using namespace Neural;

ForagingRegionsController::ForagingRegionsController(std::shared_ptr<RobotWorldModel> wm) : TemplateEEController(wm)
{
    // superclass constructor called before this baseclass constructor.
    resetFitness(); // superconstructor calls parent method.
    
    lastSeenObjectIdPerSensorList = new int [_wm->_cameraSensorsNb];
    for ( int i = 0 ; i < _wm->_cameraSensorsNb ; i++ )
        lastSeenObjectIdPerSensorList[i] = -1;
    
    //lastSeenObjectIdOnFloorSensor = -1;
}

ForagingRegionsController::~ForagingRegionsController()
{
    // superclass destructor automatically called after this baseclass destructor.
    
    delete [] lastSeenObjectIdPerSensorList;
}

void ForagingRegionsController::stepController()
{
    TemplateEEController::stepController();
    
    // * register all objects seen through distance sensors and check if caught an object (or not, then, who did?)
    // This code block is used to check if the current robot got one object it was perceiving, or if the object was "stolen" by someone else.

    bool localDebug = false;
    
    int firstCheckedObjectId = -1;
    int lastCheckedObjectId = -1; // used to consider unique object ids.
    
    int objectOnFloorIndex = _wm->getGroundSensorValue();
    if ( PhysicalObject::isInstanceOf(objectOnFloorIndex) )
    {
        objectOnFloorIndex = objectOnFloorIndex - gPhysicalObjectIndexStartOffset; // note that object may have already disappeared, though its trace remains in the floor sensor.
    }
    else
    {
        objectOnFloorIndex = -1;
    }

    std::shared_ptr<PhysicalObject> lastWalkedObject;
    if ( objectOnFloorIndex != -1 )
        lastWalkedObject = gPhysicalObjects[objectOnFloorIndex];
    
    for(int i  = 0; i < _wm->_cameraSensorsNb; i++)
    {
        if ( lastSeenObjectIdPerSensorList[i] != -1 )
        {
            int targetId = lastSeenObjectIdPerSensorList[i];
            
            if ( firstCheckedObjectId == -1 )
                firstCheckedObjectId = targetId;

            std::shared_ptr<PhysicalObject> object = gPhysicalObjects[targetId];
            
            if ( object->getTimestepSinceRelocation() == 0 )
            {
                if ( objectOnFloorIndex != -1 && object->getId() == lastWalkedObject->getId() )
                {
                    if ( targetId != lastCheckedObjectId && !( i == _wm->_cameraSensorsNb - 1 && targetId != firstCheckedObjectId ) )
                    {
                        if ( localDebug )
                            std::cout << "[DEBUG] robot #" << _wm->getId() << " says: I gathered object no." << object->getId() << "!\n";
                    }
                    else
                    {
                        if ( localDebug )
                            std::cout << "[DEBUG] robot #" << _wm->getId() << " says: I gathered object no." << object->getId() << "! (already said that)\n";
                    }
                }
                else
                {
                    if ( targetId != lastCheckedObjectId && !( i == _wm->_cameraSensorsNb - 1 && targetId != firstCheckedObjectId ) )
                    {
                        if ( localDebug )
                            std::cout << "[DEBUG] robot #" << _wm->getId() << " says: object no." << object->getId() << " disappeared! (not me!)\n";
                        this->regret += ForagingRegionsSharedData::regretValue; // so frustrating!
                    }
                    else
                    {
                        if ( localDebug )
                            std::cout << "[DEBUG] robot #" << _wm->getId() << " says: object no." << object->getId() << " disappeared! (not me!) (already said that)\n";
                    }
                }
            }

            if ( lastCheckedObjectId != targetId )  // note that distance sensors cannot list obj1,obj2,obj1 due to similar object size. ie.: continuity hypothesis wrt object sensing (partial occlusion by another robot is not a problem, as this part of the code is executed only in an object is detected).
                lastCheckedObjectId = targetId;
            
        }
    }
    
    // store current sensor values for next step.
    for(int i  = 0; i < _wm->_cameraSensorsNb; i++)
    {
        int objectId = _wm->getObjectIdFromCameraSensor(i);
        
        if ( PhysicalObject::isInstanceOf(objectId) )
        {
            lastSeenObjectIdPerSensorList[i] = objectId - gPhysicalObjectIndexStartOffset;
        }
        else
        {
            lastSeenObjectIdPerSensorList[i] = -1;
        }
    }
    
}

void ForagingRegionsController::initController()
{
    TemplateEEController::initController();
}

void ForagingRegionsController::stepEvolution()
{
    TemplateEEController::stepEvolution();
}

void ForagingRegionsController::initEvolution()
{
    TemplateEEController::initEvolution();
}

void ForagingRegionsController::performSelection()
{
    switch ( TemplateEESharedData::gSelectionMethod )
    {
        case 0:
        case 1:
        case 2:
        case 3:
            TemplateEEController::performSelection();
            break;
        case 4:
            selectNaiveMO();
            break;
        default:
            std::cerr << "[ERROR] unknown selection method (gSelectionMethod = " << TemplateEESharedData::gSelectionMethod << ")\n";
            exit(-1);
    }
}

void ForagingRegionsController::selectNaiveMO()
{
    auto fitnessItUnderFocus = _fitnessValueList.begin();
    auto regretItUnderFocus = _regretValueList.begin();

    std::vector<std::pair<int, int>> paretoFrontGenomeList;

    // build the Pareto front

    for (; fitnessItUnderFocus != _fitnessValueList.end(); ++fitnessItUnderFocus, ++regretItUnderFocus)
    {
        auto fitnessItChallenger = _fitnessValueList.begin();
        auto regretItChallenger = _regretValueList.begin();

        bool candidate = true;

        for (; fitnessItChallenger != _fitnessValueList.end(); ++fitnessItChallenger, ++regretItChallenger)
        {
            if ((*fitnessItUnderFocus).second<(*fitnessItChallenger).second and (*regretItUnderFocus).second>(
                    *regretItChallenger).second) // remember: regret is positive and larger value is worse.
            {
                candidate = false;
                break;
            }
        }
        if (candidate)
            paretoFrontGenomeList.push_back((*fitnessItUnderFocus).first);
    }
    
    // select a random genome from the Pareto front
    
    int randomIndex = randint()%paretoFrontGenomeList.size();
    std::pair<int,int> selectId = paretoFrontGenomeList.at(randomIndex);
    
    // update current genome with selected parent (mutation will be done elsewhere)
    
    _birthdate = gWorld->getIterations();
    
    _currentGenome = _genomesList[selectId];
    _currentSigma = _sigmaList[selectId];
    
    setNewGenomeStatus(true);
}

void ForagingRegionsController::performVariation()
{
    TemplateEEController::performVariation();
}

void ForagingRegionsController::broadcastGenome()
{
    TemplateEEController::broadcastGenome();
}

double ForagingRegionsController::getFitness()
{
    switch ( ForagingRegionsSharedData::fitnessFunction )
    {
        case 0:
            return 0.0; // no fitness (ie. medea) [CTL]
            break;
        case 1: // foraging-only
        case 4: // naive MO (using foraging and regret as seperate objectives)
            return std::abs(_wm->_fitnessValue); // foraging-only (or naive MO, which uses fitness as foraging)
            break;
        case 2:
            return std::max( 0.0, ( std::abs(_wm->_fitnessValue) - this->regret ) ); // foraging and regret (aggregated)
            break;
        case 3:
            return -(double)this->regret; // regret-only [CTL]
            break;
        default:
            std::cerr << "[ERROR] Fitness function unkown (check fitnessFunction value). Exiting.\n";
            exit (-1);
    }
}


void ForagingRegionsController::resetFitness()
{
    TemplateEEController::resetFitness();
    
    nbForagedItemType0 = 0;
    nbForagedItemType1 = 0;
    
    this->regret = 0;
}


void ForagingRegionsController::updateFitness()
{
    // nothing to do -- updating is performed in ForagingRegionAgentObserver (automatic event when energy item are captured)
}


void ForagingRegionsController::logCurrentState()
{
    TemplateEEController::logCurrentState();
}


bool ForagingRegionsController::sendGenome(std::shared_ptr<TemplateEEController> _targetRobotController)
{
    auto *p = new ForagingRegionsPacket();
    p->senderId = std::make_pair(_wm->getId(), _birthdate);
    p->fitness = getFitness();
    p->genome = _currentGenome;
    p->sigma = _currentSigma;
    p->regret = this->regret;

    bool retValue = std::dynamic_pointer_cast<ForagingRegionsController>(_targetRobotController)->receiveGenome(p);

    return retValue;
}


bool ForagingRegionsController::receiveGenome( Packet* p )
{
    ForagingRegionsPacket* p2 = static_cast<ForagingRegionsPacket*>(p);
    
    std::map<std::pair<int,int>, std::vector<double> >::const_iterator it = _genomesList.find(p2->senderId);
    
    _fitnessValueList[p2->senderId] = p2->fitness;
    _regretValueList[p2->senderId] = p2->regret;
    
    if ( it == _genomesList.end() ) // this exact agent's genome is already stored. Exact means: same robot, same generation. Then: update fitness value (the rest in unchanged)
    {
        _genomesList[p2->senderId] = p2->genome;
        _sigmaList[p2->senderId] = p2->sigma;
        return true;
    }
    else
    {
        return false;
    }
}
