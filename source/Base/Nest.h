#ifndef NEST_H_
#define NEST_H_

#include <argos3/core/utility/math/vector2.h>
//#include <argos3/core/utility/math/ray3.h>
#include "Pheromone.h"
using namespace argos;
using namespace std;

/*****
 * Implementation of the iAnt nest object used by the iAnt MPFA. iAnts
 * build and maintain a list of these nest objects.
 *****/
class Nest {

	public:
		Nest();
		Nest(CVector2 location);
		       
		vector<Pheromone> PheromoneList;
        /* constructor function */
		
		/* public helper functions */
        CVector2		GetLocation();
        void		SetLocation();
        
	private:

        CVector2        nestLocation;
        
};

#endif /* IANT_NEST_H_ */
