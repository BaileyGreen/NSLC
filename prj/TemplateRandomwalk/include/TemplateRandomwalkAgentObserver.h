/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 */
 
#ifndef TEMPLATERANDOMWALKAGENTOBSERVER_H
#define TEMPLATERANDOMWALKAGENTOBSERVER_H 

#include "Observers/AgentObserver.h"

class RobotWorldModel;

class TemplateRandomwalkAgentObserver : public AgentObserver
{
public:
    using AgentObserver::AgentObserver;

    ~TemplateRandomwalkAgentObserver();

    void reset() override;

    void stepPre() override;

};


#endif

