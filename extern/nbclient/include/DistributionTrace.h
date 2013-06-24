/***************************************************************************
 *  Copyright:http://www.naradabrokering.org/index.html
 *  Author: Jaliya Ekanayake
 *  Email : jaliyae@gmail.com
 ****************************************************************************/
#ifndef DISTRIBUTIONTRACE_H_
#define DISTRIBUTIONTRACE_H_

/**
 * This is used as a place holder for DistributionTrace object that
 * needs to be sent by every NBEvent. Currently C++ client does no
 * support processing these traces.
 */
class DistributionTrace{

static const int DIST_TRAVERSDSOFAR_LEN=16;
static const int DIST_TOREACH_LEN=16;

public:
DistributionTrace();
~DistributionTrace();

void fromBytes(char *stringProfileBytes) ;
void getBytes(char *buff,int * length); 

private:
	char *traversedSoFar;
    char *toReach;

};


#endif /*DISTRIBUTIONTRACE_H_*/
