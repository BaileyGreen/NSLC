/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 *
 */

#ifndef FORAGINGREGIONSAGENTOBSERVER_H
#define FORAGINGREGIONSAGENTOBSERVER_H

#include "TemplateEE/include/TemplateEEAgentObserver.h"

class RobotWorldModel;

class ForagingRegionsAgentObserver : public TemplateEEAgentObserver
{
public:
    using TemplateEEAgentObserver::TemplateEEAgentObserver;

    ~ForagingRegionsAgentObserver();

    void stepPre() override;
};

#endif

