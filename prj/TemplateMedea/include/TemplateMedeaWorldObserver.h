/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 *
 */

#ifndef TEMPLATEMEDEAWORLDOBSERVER_H
#define TEMPLATEMEDEAWORLDOBSERVER_H

#include "TemplateEE/include/TemplateEEWorldObserver.h"

class World;

class TemplateMedeaWorldObserver : public TemplateEEWorldObserver
{
public:
    TemplateMedeaWorldObserver(World *world);
    ~TemplateMedeaWorldObserver();
    
    void initPre();
    void initPost();

    void stepPre();
    void stepPost();
    
    void monitorPopulation( bool localVerbose );
};

#endif
