#ifndef CPFA_CONTROLLER_H
#define CPFA_CONTROLLER_H

#include <source/Base/BaseController.h>
#include <source/Base/Pheromone.h>
#include <source/CPFA/CPFA_loop_functions.h>

using namespace std;
using namespace argos;

static unsigned int num_targets_collected = 0;

class CPFA_loop_functions;

class CPFA_controller : public BaseController {

	public:

		CPFA_controller();

		// CCI_Controller inheritence functions
		void Init(argos::TConfigurationNode &node);
		void ControlStep();
		void Reset();

		bool IsHoldingFood();
		bool IsUsingSiteFidelity();
		bool IsInTheNest();

		Real FoodDistanceTolerance;

		void SetLoopFunctions(CPFA_loop_functions* lf);

	private:
        string controllerID;
		CPFA_loop_functions* LoopFunctions;
		argos::CRandom::CRNG* RNG;

		/* pheromone trail variables */
		std::vector<argos::CVector2> TrailToShare;
		std::vector<argos::CVector2> TrailToFollow;
		std::vector<argos::CRay3>    MyTrail;

		/* robot position variables */
		argos::CVector2 SiteFidelityPosition;
        bool	updateFidelity;
		vector<CRay3> myTrail;
		CColor        TrailColor;

		bool isInformed;
		bool isHoldingFood;
		bool isUsingSiteFidelity;
		bool isGivingUpSearch;

		size_t ResourceDensity;
		size_t MaxTrailSize;
		size_t SearchTime;

		/* iAnt CPFA state variable */
		enum CPFA_state {
			DEPARTING = 0,
			SEARCHING = 1,
			RETURNING = 2,
			SURVEYING = 3
		} CPFA_state;

		/* iAnt CPFA state functions */
		void CPFA();
		void Departing();
		void Searching();
		void Returning();
		void Surveying();

		/* CPFA helper functions */
		void SetRandomSearchLocation();
		void SetHoldingFood();
		void SetLocalResourceDensity();
		void SetFidelityList(argos::CVector2 newFidelity);
		void SetFidelityList();
		bool SetTargetPheromone();

		argos::Real GetExponentialDecay(argos::Real value, argos::Real time, argos::Real lambda);
		argos::Real GetBound(argos::Real value, argos::Real min, argos::Real max);
		argos::Real GetPoissonCDF(argos::Real k, argos::Real lambda);

		void UpdateTargetRayList();

		CVector2 previous_position;

		string results_path;
		string results_full_path;
		bool isUsingPheromone;

		unsigned int survey_count;
};

#endif /* CPFA_CONTROLLER_H */