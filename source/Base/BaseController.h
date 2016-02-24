#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H

#include <argos3/core/utility/logging/argos_log.h>
#include <argos3/core/control_interface/ci_controller.h>
#include <argos3/plugins/robots/generic/control_interface/ci_positioning_sensor.h>
#include <argos3/plugins/robots/generic/control_interface/ci_differential_steering_actuator.h>
#include <argos3/plugins/robots/foot-bot/control_interface/ci_footbot_proximity_sensor.h>
#include <argos3/core/simulator/loop_functions.h>
#include <cmath>
#include <stack>

/**
 * BaseController
 * @author Antonio Griego
 */
class BaseController : public argos::CCI_Controller {

    public:

        BaseController();

        /*  navigation functions */
        argos::CRadians GetHeading();
        argos::CVector2 GetPosition();
        argos::CVector2 GetTarget();
        void SetTarget(argos::CVector2 t);
        void SetStartPosition(argos::CVector3 sp);
        argos::CVector3 GetStartPosition();
        size_t GetMovementState();

        void Stop();
        void Move();
        bool Wait();
        void Wait(size_t wait_time_in_seconds);

        /*  time calculation functions */
        size_t SimulationTick();
        size_t SimulationTicksPerSecond();
        argos::Real SimulationSecondsPerTick();
        argos::Real SimulationTimeInSeconds();

	void SetIsHeadingToNest(bool n);

        bool IsAtTarget();

    protected:

	argos::CRandom::CRNG* RNG;

	unsigned int collision_counter;
	float DestinationNoiseStdev; // for introducing error in destination positions
	float PositionNoiseStdev; // for introducing error in current position

        size_t WaitTime;

	argos::CRadians TargetAngleTolerance;
	argos::Real NestDistanceTolerance;
	argos::CRadians NestAngleTolerance;
        argos::Real TargetDistanceTolerance;
        argos::Real SearchStepSize;

        argos::CRange<argos::Real> ForageRangeX;
        argos::CRange<argos::Real> ForageRangeY;
        argos::CRange<argos::Real> GoStraightAngleRangeInDegrees;

    	//  base controller movement parameters
    	argos::Real RobotForwardSpeed;
    	argos::Real RobotRotationSpeed;
    	argos::Real TicksToWaitWhileMoving;

        // foot-bot components: sensors and actuators
        argos::CCI_PositioningSensor* compassSensor;
        argos::CCI_DifferentialSteeringActuator* wheelActuator;
        argos::CCI_FootBotProximitySensor* proximitySensor;

        // controller state variables
        enum MovementState {
            STOP    = 0,
            LEFT    = 1,
            RIGHT   = 2,
            FORWARD = 3,
            BACK    = 4
        } CurrentMovementState;

	/* movement definition variables */
        struct Movement {
            size_t type;
            argos::Real magnitude;
        };

	Movement previous_movement;
	argos::CVector2 previous_pattern_position;
	
	std::stack<Movement> MovementStack;

    private:

        argos::CLoopFunctions& LF;

        argos::CVector3 StartPosition;
        argos::CVector2 TargetPosition;

                //MovementState CurrentMovementState;
        

        /* private navigation helper functions */
        void SetNextMovement();
        void SetTargetAngleDistance(argos::Real newAngleToTurnInDegrees);
        void SetTargetTravelDistance(argos::Real newTargetDistance);
        void SetLeftTurn(argos::Real newTargetAngle);
        void SetRightTurn(argos::Real newTargetAngle);
        void SetMoveForward(argos::Real newTargetDistance);
        void SetMoveBack(argos::Real newTargetDistance);
        void PushMovement(size_t moveType, argos::Real moveSize);
        void PopMovement();

        /* collision detection functions */
        bool CollisionDetection();
        argos::CVector2 GetCollisionVector();

	bool heading_to_nest;

};

#endif /* IANTBASECONTROLLER_H */
