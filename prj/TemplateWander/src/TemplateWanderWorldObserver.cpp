/**
 * @author Nicolas Bredeche <nicolas.bredeche@upmc.fr>
 */


#include "TemplateWander/include/TemplateWanderWorldObserver.h"
#include "World/World.h"


TemplateWanderWorldObserver::TemplateWanderWorldObserver( World *__world ) : WorldObserver( __world )
{
	_world = __world;
}

TemplateWanderWorldObserver::~TemplateWanderWorldObserver()
{
	// nothing to do.
}


void TemplateWanderWorldObserver::initPre()
{
    // nothing to do.
}

void TemplateWanderWorldObserver::initPost()
{
    // nothing to do.
}

void TemplateWanderWorldObserver::stepPre()
{
	// nothing to do.
}

void TemplateWanderWorldObserver::stepPost()
{
    // nothing to do.
}
