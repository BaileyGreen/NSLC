/*
 *  Controller.cpp
 *  roborobo-online
 *
 *  Created by Nicolas on 19/03/09.
 *
 */

#ifdef PYROBOROBO
#include <pyroborobo/pyroborobo.h>

#include <utility>
#endif

#include "Controllers/Controller.h"
#include "WorldModels/RobotWorldModel.h"
#include "RoboroboMain/roborobo.h"
#include "World/World.h"


Controller::Controller(  ): lastRefreshIteration(-1)
{
}

Controller::Controller(std::shared_ptr<RobotWorldModel> __wm) :
    lastRefreshIteration(-1), _wm(__wm)
{
    distanceSensors.resize(_wm->_cameraSensorsNb);
    angleSensors.resize(_wm->_cameraSensorsNb);
    objectDetectors.resize(_wm->_cameraSensorsNb);
    objectTypeDetectors.resize(_wm->_cameraSensorsNb);
    objectInstanceDetectors.resize(_wm->_cameraSensorsNb);
    robotDetectors.resize(_wm->_cameraSensorsNb);
    robotGroupDetector.resize(_wm->_cameraSensorsNb);
    robotRelativeOrientationDetectors.resize(_wm->_cameraSensorsNb);
    wallDetectors.resize(_wm->_cameraSensorsNb);
    robotControllerDetectors.resize(_wm->_cameraSensorsNb);
    robotInstanceDetectors.resize(_wm->_cameraSensorsNb);
#ifdef PYROBOROBO
    pyRobotControllerDetectors.resize(_wm->_cameraSensorsNb);
    pyObjectInstanceDetectors.resize(_wm->_cameraSensorsNb);
#endif

    for (size_t i = 0; i < angleSensors.size(); i++)
    {
        angleSensors[i] = _wm->getCameraSensorValue(i, SENSOR_TARGETANGLE);
    }

}

Controller::~Controller() = default;

std::string Controller::inspect( std::string prefix )
{
    return std::string(prefix + "Controller::inspect() not implemented.");
}

/*
  The refreshInputs() method refreshes sensor information, for user by the wrapper access functions.
       In general, calling this method is not called if you access directly sensor information (as in most case)
       This method is useful only if you use wrapper function as defined in Controller.hpp
       Not that this method is automatically called when one of the wrapper function is used, if necessary.
       It can be quite useful to present sensory information in a clear, easy-to-use, format.
  Wrapper access function that use this method are (with "i", the index of the distance sensor):
       int getObjectAt( int i )
       int getObjectTypeAt( int i )
       int getRobotIdAt( int i )
       int getRobotGroupIdAt( int i )
       int getWallAt( int i )
       double getRobotOrientationAt( int i )
       double getRedGroundDetector( )
       double getGreenGroundDetector( )
       double getBlueGroundDetector( )
       double getClosestLandmarkDistance()
       double getClosestLandmarkOrientation()
 */
void Controller::refreshInputs(){
    // sensory inputs for each sensor are ordered (and filled in) as follow:
    // - N range sensors
    //      - distance to obstacle (max if nothing) -- in [0,1]
    //      - [0...N_physicalobjecttypes] Is it an object of type i? (0: no, 1: yes) -- type: 0 (round), 1 (energy item), 2 (gate), 3 (switch), ...? (cf. PhysicalObjectFactory)
    //      - is it a robot? (1 or 0)
    //      - is it from the same group? (1 or 0)
    //      - relative orientation wrt. current robot (relative orientation or 0 if not a robot)
    //      - is it a wall? (ie. a non-distinguishible something) (1 or 0)
    // - floor sensor
    //      - red value
    //      - green value
    //      - blue value
    // - relative distance and/or orientation of either the closest landmark (if any, zero if none)
#ifdef PYROBOROBO
    auto rob = Pyroborobo::get();
#endif

    lastRefreshIteration = gWorld->getIterations();
    redGroundDetectors = -1;
    greenGroundDetectors = -1;
    blueGroundDetectors = -1;

    landmark_closest_DirectionDetector = -1;
    landmark_closest_DistanceDetector = -1;
    
    // camera sensors

    for(int i  = 0; i < _wm->_cameraSensorsNb; i++)
    {
        distanceSensors[i] = (_wm->getDistanceValueFromCameraSensor(i) / _wm->getCameraSensorMaximumDistanceValue(i));
        
        int objectId = _wm->getObjectIdFromCameraSensor(i);
        if ( gSensoryInputs_physicalObjectType )
        {
            // input: physical object? which type?
            if ( PhysicalObject::isInstanceOf(objectId) )
            {
                objectDetectors[i] = objectId; // match
                objectInstanceDetectors[i] = gPhysicalObjects[objectId - gPhysicalObjectIndexStartOffset];
                objectTypeDetectors[i] = (objectInstanceDetectors[i]->getType() );
#ifdef PYROBOROBO
                pyObjectInstanceDetectors[i] = rob->getObjects()[objectId - gPhysicalObjectIndexStartOffset];
#endif
            }
            else
            {
                    objectDetectors[i] = (-1);
                    objectInstanceDetectors[i] = nullptr;
                    objectTypeDetectors[i] = (-1);
#ifdef PYROBOROBO
                pyObjectInstanceDetectors[i] = py::none();
#endif
            }
        }
        
        if ( gSensoryInputs_isOtherAgent )
        {
            // input: another agent? If yes: same group?
            if ( Agent::isInstanceOf(objectId) )
            {
                // this is an agent
                robotInstanceDetectors[i] = gWorld->getRobot(objectId-gRobotIndexStartOffset);
                int targetRobotId = robotInstanceDetectors[i]->getWorldModel()->getId();
                robotDetectors[i] = ( targetRobotId );
                robotControllerDetectors[i] = robotInstanceDetectors[i]->getController();
#ifdef PYROBOROBO
                pyRobotControllerDetectors[i] = rob->getControllers()[targetRobotId];
#endif

                if ( gSensoryInputs_otherAgentGroup )
                {
                    int targetRobotGroup = gWorld->getRobot(objectId-gRobotIndexStartOffset)->getWorldModel()->getGroupId();
                    robotGroupDetector[i] = ( targetRobotGroup );
                }
                
                if ( gSensoryInputs_otherAgentOrientation )
                {
                    // relative orientation? (ie. angle difference wrt. current agent)
                    double srcOrientation = _wm->_agentAbsoluteOrientation;
                    double tgtOrientation = gWorld->getRobot(objectId-gRobotIndexStartOffset)->getWorldModel()->_agentAbsoluteOrientation;
                    double delta_orientation = - ( srcOrientation - tgtOrientation );
                    if ( delta_orientation >= 180.0 )
                    {
                        delta_orientation = - ( 360.0 - delta_orientation );
                    }
                    else
                    {
                        if ( delta_orientation <= -180.0 )
                        {
                            delta_orientation = - ( - 360.0 - delta_orientation );
                        }
                    }
                    robotRelativeOrientationDetectors[i] = ( delta_orientation/180.0 );
                }
            }
            else
            {
                robotDetectors[i] = ( -1 ); // not an agent...
                robotInstanceDetectors[i] = nullptr;
                robotControllerDetectors[i] = nullptr;
#ifdef PYROBOROBO
                pyRobotControllerDetectors[i] = py::none();
#endif
                if ( gSensoryInputs_otherAgentGroup )
                {
                    robotGroupDetector[i] = ( 0 ); // ...therefore no match wrt. group.
                }
                if ( gSensoryInputs_otherAgentOrientation )
                {
                    robotRelativeOrientationDetectors[i] = ( 0 ); // ...and no orientation.
                }
            }
        }
        
        if ( gSensoryInputs_isWall )
        {
            // input: wall or empty?
            if ( objectId >= 0 && objectId < gPhysicalObjectIndexStartOffset ) // not empty, but cannot be identified: this is a wall.
            {
                wallDetectors[i] = ( 1 );
            }
            else
            {
                wallDetectors[i] = ( 0 ); // nothing. (objectId=-1)
            }
        }
        
    }
    
    // floor sensor
    
    if ( gSensoryInputs_groundSensors )
    {
        redGroundDetectors = (double)_wm->getGroundSensor_redValue()/255.0;
        greenGroundDetectors = (double)_wm->getGroundSensor_greenValue()/255.0;
        blueGroundDetectors = (double)_wm->getGroundSensor_blueValue()/255.0;
    }
    
    // closest landmark
    
    if ( gNbOfLandmarks > 0 )  // if ( gSensoryInputs_landmarkTrackerMode == 1 ) // register closest landmark
    {
        _wm->updateLandmarkSensor(); // update with closest landmark
        landmark_closest_DistanceDetector = _wm->getLandmarkDistanceValue();
        landmark_closest_DirectionDetector = _wm->getLandmarkDirectionAngleValue();
    }
    else
    {
        landmark_closest_DistanceDetector = -1;
        landmark_closest_DirectionDetector = -1;
    }

}

bool Controller::checkRefresh() const
{
    if ( lastRefreshIteration != gWorld->getIterations() )
        return false;
    else
        return true;
}

// setTranslation(value) takes a value in [-1,+1]
void Controller::setTranslation( double value )
{
    if ( value > 1.0 || value < -1.0 )
    {
        std::cerr << "[WARNING] setTranslation(value) with value out of bounds (" << value << "). Bounding in [-1.0,+1.0]\n";
        if ( value > 1.0 )
        {
            value = 1.0;
        }
        else
        {
            value = -1.0;
        }
    }
    _wm->_desiredTranslationalValue = value * _wm->_maxTranslationalDeltaValue;
}

// setRotation(value) takes a value in [-1,+1]
void Controller::setRotation( double value )
{
    if ( value > 1.0 || value < -1.0 )
    {
        std::cerr << "[WARNING] setRotation(value) with value out of bounds (" << value << "). Bounding in [-1.0,+1.0]\n";
        if ( value > 1.0 )
        {
            value = 1.0;
        }
        else
        {
            value = -1.0;
        }
    }
    _wm->_desiredRotationalVelocity = value * _wm->_maxRotationalDeltaValue;
}

int Controller::getId()
{
    return _wm->getId();
}

double Controller::getActualTranslation()
{
    return _wm->_actualTranslationalValue/_wm->_maxTranslationalDeltaValue;
}

double Controller::getActualRotation()
{
    return _wm->_actualRotationalVelocity/_wm->_maxRotationalDeltaValue;
}

double Controller::getCompass()
{
    return _wm->_agentAbsoluteOrientation / 180.0;
}

double Controller::getOrientation()
{
    return _wm->_agentAbsoluteOrientation / 180.0;
}

double Controller::getLinearSpeed()
{
    return _wm->_agentAbsoluteOrientation;
}

Point2d Controller::getPosition()
{
    Point2d posRobot(_wm->_xReal,_wm->_yReal);
    return posRobot;
}

// returns -1 (no object) or objectId (object)
int Controller::getObjectAt( int sensorId )
{
    if ( gSensoryInputs_physicalObjectType == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getObjectAt(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return objectDetectors[sensorId];
}

std::vector<int>& Controller::getAllObjects( )
{
    if ( gSensoryInputs_physicalObjectType == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getAllObjects(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return objectDetectors;
}

std::shared_ptr<PhysicalObject> Controller::getObjectInstanceAt( int sensorId )
{
    if ( gSensoryInputs_physicalObjectType == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getObjectInstanceAt(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return objectInstanceDetectors[sensorId];
}

std::vector<std::shared_ptr<PhysicalObject>>& Controller::getAllObjectInstances()
{
    if ( gSensoryInputs_physicalObjectType == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getAllObjectInstances(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }

    return objectInstanceDetectors;
}

// returns target object's type (relevant if target object exists)
// returns an integer. Check PhysicalObjectFactory class for existing types.
// most common types are: 0 (round), 1 (energy item), 2 (gate), 3 (switch), 4 (movable), ...?
int Controller::getObjectTypeAt( int sensorId )
{
    if ( gSensoryInputs_physicalObjectType == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getObjectTypeAt(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return objectTypeDetectors[sensorId];
}

std::vector<int>& Controller::getAllObjectTypes()
{
    if ( gSensoryInputs_physicalObjectType == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getAllObjectTypes(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return objectTypeDetectors;
}

// returns robot's id, or -1 (not a robot)
int Controller::getRobotIdAt( int sensorId )
{
    if ( gSensoryInputs_isOtherAgent == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getRobotIdAt(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return robotDetectors[sensorId];
}

std::vector<int>& Controller::getAllRobotIds()
{
    if ( gSensoryInputs_isOtherAgent == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getAllRobotIds(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return robotDetectors;
}

// returns target robot's group, or -1
int Controller::getRobotGroupIdAt( int sensorId )
{
    if ( gSensoryInputs_isOtherAgent == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getRobotIdAt(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return robotGroupDetector[sensorId];
}

std::vector<int>& Controller::getAllRobotGroupIds()
{
    if ( gSensoryInputs_isOtherAgent == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getAllRobotGroupIds(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return robotGroupDetector;
}

// returns 0 (not a wall) or 1 (wall)
int Controller::getWallAt( int sensorId )
{
    if ( gSensoryInputs_isWall == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getWallAt(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return wallDetectors[sensorId];
}

std::vector<int>& Controller::getAllWalls()
{
    if ( gSensoryInputs_isWall == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getWallAt(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return wallDetectors;
}

double Controller::getRobotRelativeOrientationAt( int sensorId )
{
    if ( gSensoryInputs_otherAgentOrientation == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getRobotRelativeOrientationAt(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return robotRelativeOrientationDetectors[sensorId];
}

std::vector<double>& Controller::getAllRobotRelativeOrientations( )
{
    if ( gSensoryInputs_otherAgentOrientation == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getRobotRelativeOrientationAt(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return robotRelativeOrientationDetectors;
}


// return a value in [0,1] (red component)
double Controller::getRedGroundDetector( )
{
    if ( gSensoryInputs_groundSensors == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getRedGroundDetector(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return redGroundDetectors;
}

// return a value in [0,1] (green component)
double Controller::getGreenGroundDetector( )
{
    if ( gSensoryInputs_groundSensors == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getGreenGroundDetector(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return greenGroundDetectors;
}

// return a value in [0,1] (blue component)
double Controller::getBlueGroundDetector( )
{
    if ( gSensoryInputs_groundSensors == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getBlueGroundDetector(.)\n";
        exit(-1);
    }
    if ( checkRefresh() == false ) { refreshInputs(); }
    return blueGroundDetectors;
}

std::shared_ptr<Controller> Controller::getRobotControllerAt(int sensorId)
{
    if (gSensoryInputs_isOtherAgent == false)
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getRobotControllerAt(.)\n";
        exit(-1);
    }
    if (checkRefresh() == false)
    {
        refreshInputs();
    }
    return robotControllerDetectors[sensorId];
}

std::vector<std::shared_ptr<Controller>>& Controller::getAllRobotControllers()
{
    if (gSensoryInputs_isOtherAgent == false)
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getAllRobotControllers(.)\n";
        exit(-1);
    }
    if (checkRefresh() == false)
    {
        refreshInputs();
    }
    return robotControllerDetectors;
}

std::vector<std::shared_ptr<Robot>> &Controller::getAllRobotInstances()
{
    if (gSensoryInputs_isOtherAgent == false)
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getAllRobotInstances(.)\n";
        exit(-1);
    }
    if (checkRefresh() == false)
    {
        refreshInputs();
    }
    return robotInstanceDetectors;
}

std::shared_ptr<Robot> Controller::getRobotInstanceAt(int sensorId)
{
    if (gSensoryInputs_isOtherAgent == false)
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getRobotInstanceAt(.)\n";
        exit(-1);
    }
    if (checkRefresh() == false)
    {
        refreshInputs();
    }
    return robotInstanceDetectors[sensorId];
}

#ifdef PYROBOROBO
std::vector<py::handle> & Controller::getAllPyObjectInstances()
{
    if ( gSensoryInputs_physicalObjectType == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getAllPyObjectInstances(.)\n";
        exit(-1);
    }
    if (checkRefresh() == false)
    {
        refreshInputs();
    }
    return pyObjectInstanceDetectors;
}

std::vector<py::handle> & Controller::getAllPyRobotControllers()
{
    if (gSensoryInputs_isOtherAgent == false)
    {
        std::cout << "[ERROR] Unauthorized call to getAllPyRobotControllers\n";
        exit(-1);
    }
    if (checkRefresh() == false)
    {
        refreshInputs();
    }
    return pyRobotControllerDetectors;
}


py::handle Controller::getPyRobotControllerAt(int sensorId)
{
    if (gSensoryInputs_isOtherAgent == false)
    {
        std::cout << "[ERROR] Unauthorized call to getPyRobotControllerAt\n";
        exit(-1);
    }
    if (checkRefresh() == false)
    {
        refreshInputs();
    }
    return pyRobotControllerDetectors[sensorId];
}

py::handle Controller::getPyObjectInstanceAt(int sensorId)
{
    if ( gSensoryInputs_physicalObjectType == false )
    {
        std::cout << "[ERROR] Unauthorized call to Controller::getPyObjectInstanceAt(.)\n";
        exit(-1);
    }
    if (checkRefresh() == false)
    {
        refreshInputs();
    }
    return pyObjectInstanceDetectors[sensorId];
}

double Controller::getSensorAngleAt(int sensorId) {
    return angleSensors[sensorId];
}

std::vector<double>& Controller::getAllSensorAngles() {
    return angleSensors;
}

#endif