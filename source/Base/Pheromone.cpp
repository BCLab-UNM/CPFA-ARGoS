#include "Pheromone.h"

/*****
 * The  pheromone needs to keep track of four things:
 *
 * [1] location of the waypoint
 * [2] a trail to the nest
 * [3] simulation time at creation
 * [4] pheromone rate of decay
 *
 * The remaining variables always start with default values.
 *****/
Pheromone::Pheromone(argos::CVector2              newLocation,
                             std::vector<argos::CVector2> newTrail,
                             argos::Real                  newTime,
                             argos::Real                  newDecayRate,
                             size_t                       density)
{
    /* required initializations */
	location    = newLocation;
    trail       = newTrail;
	lastUpdated = newTime;
	decayRate   = newDecayRate;
    ResourceDensity = density;
    /* standardized initializations */
	weight      = 1.0;
	threshold   = 0.001;
}

/*****
 * The pheromones slowly decay and eventually become inactive. This simulates
 * the effect of a chemical pheromone trail that dissipates over time.
 *****/
void Pheromone::Update(argos::Real time) {
    /* pheromones experience exponential decay with time */
    weight *= exp(-decayRate * (time - lastUpdated));
    lastUpdated = time;
}

void Pheromone::UpdateLocation(argos::CVector2  location){ //qilu 09/12/2016


}
/*****
 * Turns off a pheromone and makes it inactive.
 *****/
void Pheromone::Deactivate() {
    weight = 0.0;
}

/*****
 * Return the pheromone's location.
 *****/
argos::CVector2 Pheromone::GetLocation() {
    return location;
}

/*****
 * Return the trail between the pheromone and the nest.
 *****/
std::vector<argos::CVector2> Pheromone::GetTrail() {
    return trail;
}

/*****
 * Return the weight, or strength, of this pheromone.
 *****/
argos::Real Pheromone::GetWeight() {
	return weight;
}
size_t  Pheromone::GetResourceDensity(){
    return ResourceDensity;
}

/*****
 * Is the pheromone active and usable?
 * TRUE:  weight >  threshold : the pheromone is active
 * FALSE: weight <= threshold : the pheromone is not active
 *****/
bool Pheromone::IsActive() {
	return (weight > threshold);
}
