#include "Nest.h"

/*****
 * The iAnt nest needs to keep track of four things:
 *
 * [1] location
 * [2] nest id 
 * [3] site fidelity
 * [4] pheromone trails
 *
 *****/
	Nest::Nest(){}
	Nest::Nest(CVector2   location)
{
    /* required initializations */
	   nestLocation    = location;
    PheromoneList.clear();
    FidelityList.clear();
    DensityOnFidelity.clear(); //qilu 09/11/2016
    FoodList.clear(); //qilu 09/07/2016
}

/*****
 *****/

/*****
 * Return the nest's location.
 *****/
CVector2 Nest::GetLocation() {
    return nestLocation;
}

void Nest::SetLocation() {
    nestLocation=CVector2(0.0, 0.0);
}

void Nest::SetLocation(CVector2 newLocation) {
    nestLocation = newLocation;
}

void Nest::UpdateNestLocation(){ //qilu 09/10/2016
    CVector2 Sum_locations = CVector2(0.0, 0.0);
    size_t Num_points = 0;


    for(size_t i =0; i<PheromoneList.size(); i++){
//        Sum_locations += PheromoneList[i].GetLocation() * PheromoneList[i].GetResourceDensity();
//        Num_points += PheromoneList[i].GetResourceDensity();
        Sum_locations += PheromoneList[i].location * PheromoneList[i].ResourceDensity;
        Num_points += PheromoneList[i].ResourceDensity;
        }

    for(map<string, size_t>::iterator it= DensityOnFidelity.begin(); it!=DensityOnFidelity.end(); ++it){
        Sum_locations += FidelityList[it->first] * it->second;
        Num_points += it->second;
    }

    //if (FoodList.size() !=0)
    for (size_t i=0; i<FoodList.size(); i++) {
        Sum_locations += FoodList[i];
        Num_points ++;
    }
    if (Num_points !=0) SetLocation(Sum_locations / Num_points);
}
        
