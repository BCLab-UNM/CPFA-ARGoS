#include "BaseController.h"
#include <random>

using namespace std;

/**
 * Constructor for the BaseController. Several important variables are defined here.
 * <p>
 * TODO: update xml configuration file to allow these to be adjusted from configuration without recompiling.
 */
BaseController::BaseController() :
	LF(argos::CSimulator::GetInstance().GetLoopFunctions()),
	WaitTime(0),
	NestDistanceTolerance(0.01),
	NestAngleTolerance(0.05),
	TargetDistanceTolerance(0.01),
	TargetAngleTolerance(0.04),
	SearchStepSize(0.16),
	RobotForwardSpeed(16.0),
	RobotRotationSpeed(4.0),
	TicksToWaitWhileMoving(0.0),
	CurrentMovementState(STOP),
	heading_to_nest(false),
	DestinationNoiseStdev(0),
	PositionNoiseStdev(0),
	RNG(argos::CRandom::CreateRNG("argos"))
{
	// calculate the forage range and compensate for the robot's radius of 0.085m
	argos::CVector3 ArenaSize = LF.GetSpace().GetArenaSize();
	argos::Real rangeX = (ArenaSize.GetX() / 2.0) - 0.085;
	argos::Real rangeY = (ArenaSize.GetY() / 2.0) - 0.085;
	ForageRangeX.Set(-rangeX, rangeX);
	ForageRangeY.Set(-rangeY, rangeY);
	GoStraightAngleRangeInDegrees.Set(-37.5, 37.5);
}

argos::CRadians BaseController::GetHeading() {
	/* in ARGoS, the robot's orientation is represented by a quaternion */
	const argos::CCI_PositioningSensor::SReading& readings = compassSensor->GetReading();
	argos::CQuaternion orientation = readings.Orientation;

	/* convert the quaternion to euler angles */
	argos::CRadians z_angle, y_angle, x_angle;
	orientation.ToEulerAngles(z_angle, y_angle, x_angle);

	/* the angle to the z-axis represents the compass heading */

	return z_angle;
}

argos::CVector2 BaseController::GetPosition() {
	/* the robot's compass sensor gives us a 3D position */
	argos::CVector3 position3D = compassSensor->GetReading().Position;
	/* return the 2D position components of the compass sensor reading */

	float x = position3D.GetX();
	float y = position3D.GetY();

	// Add noise to the current position unless travelling to the nest
	// Make the noise proportional to the distance to the target
	/*
	if (!heading_to_nest)
	{
		argos::Real noise_x = RNG->Gaussian(PositionNoiseStdev);
		argos::Real noise_y = RNG->Gaussian(PositionNoiseStdev);

		x += noise_x;
		y += noise_y;
	}
	*/

	return argos::CVector2(x, y);
}

argos::CVector2 BaseController::GetTarget() {
	return TargetPosition;
}

void BaseController::SetTarget(argos::CVector2 t) {

	argos::Real x(t.GetX()), y(t.GetY());

	//if(x > ForageRangeX.GetMax()) x = ForageRangeX.GetMax();
	//else if(x < ForageRangeX.GetMin()) x = ForageRangeX.GetMin();
	
	//if(y > ForageRangeY.GetMax()) y = ForageRangeY.GetMax();
	//else if(y < ForageRangeY.GetMin()) y = ForageRangeY.GetMin();
	
	//argos::LOG << "<<Updating Target Position>>" << std::endl;

	//argos::LOG << "x: "<< x << std::endl;
	//argos::LOG << "y:" << y << std::endl;

	if (!heading_to_nest) {
		argos::Real distanceToTarget = (TargetPosition - GetPosition()).Length();
		argos::Real noise_x = RNG->Gaussian(DestinationNoiseStdev*distanceToTarget);
		argos::Real noise_y = RNG->Gaussian(DestinationNoiseStdev*distanceToTarget);
		
		x += noise_x;
		y += noise_y;

		//argos::LOG << "Not Heading to Nest " << std::endl;
		//argos::LOG << "Noise x: "<< noise_x << std::endl;
		//argos::LOG << "Noise y:" << noise_y << std::endl;
	} else {
		//argos::LOG << "Heading to Nest " << std::endl;
	}

	if(y > ForageRangeY.GetMax() || y < ForageRangeY.GetMin() ||
			x > ForageRangeX.GetMax() || x < ForageRangeX.GetMin()) {
		x = GetPosition().GetX();
		y = GetPosition().GetY();
		SetRightTurn(37.5);
	}

	//argos::LOG << "New Target x: "<< x << std::endl;
	//argos::LOG << "New Target y:" << y << std::endl;

	TargetPosition = argos::CVector2(x, y);
	argos::Real distanceToTarget = (TargetPosition - GetPosition()).Length();
	
	//argos::LOG << "Distance: " << distanceToTarget << std::endl;
	//argos::LOG << "<<New Target Set>>" << std::endl;
}

void BaseController::SetStartPosition(argos::CVector3 sp) {
	StartPosition = sp;
}

argos::CVector3 BaseController::GetStartPosition() {
	return StartPosition;
}

size_t BaseController::GetMovementState() {
	return CurrentMovementState;
}


void BaseController::SetIsHeadingToNest(bool n) {
	heading_to_nest = n;
}

void BaseController::SetNextMovement() {
	argos::CRadians AngleTol;
	float DistTol;
	
	// Allow the searcher to treat movement to the nest
	// differently than other movement
	if (heading_to_nest) {
		DistTol = NestDistanceTolerance;
		AngleTol = NestAngleTolerance;
	}
	else {
		DistTol = TargetDistanceTolerance;
		AngleTol = TargetAngleTolerance;
	}
 
	if(MovementStack.size() == 0 && CurrentMovementState == STOP) {
		argos::Real distanceToTarget = (TargetPosition - GetPosition()).Length();
		argos::CRadians headingToTarget = (TargetPosition - GetPosition()).Angle();
		argos::CRadians headingToTargetError = (GetHeading() - headingToTarget).SignedNormalize();

		//argos::LOG << "Heading to target error: " << headingToTarget <<", Tol:" << AngleTol <<", Angle Tol:" << TargetAngleTolerance << "\n";

		//cout << "headingToTarget: " << headingToTarget << endl;
		//cout << "headingToTargetError: " << headingToTargetError << endl;

		//cout << "TargetAngleTolerance: " << TargetAngleTolerance << endl;
		//cout << "TargetDistanceTolerance: " << TargetDistanceTolerance << endl;


		if(!IsAtTarget()) {
			if(headingToTargetError > AngleTol) {
				//cout << "Turn Left " << endl;
				PushMovement(LEFT, -ToDegrees(headingToTargetError).GetValue());
			} else if(headingToTargetError < -AngleTol) {
				//cout << "Turn Right " << endl;
				PushMovement(RIGHT, ToDegrees(headingToTargetError).GetValue());
			} else {
				//cout << "Move Forward " << endl;
				PushMovement(FORWARD, distanceToTarget);
			}
		} else {
			PushMovement(STOP, 0.0);
		}
	} else {
		PopMovement();
	}
}

void BaseController::SetTargetAngleDistance(argos::Real newAngleToTurnInDegrees) {
	// s = arc_length = robot_radius * turning_angle
	// NOTE: the footbot robot has a radius of 0.085 m... or 8.5 cm...
	// adjusting with + 0.02 m, or + 2 cm, increases accuracy...
	
	argos::Real s = 0.105 * newAngleToTurnInDegrees;
	TicksToWaitWhileMoving = std::ceil((SimulationTicksPerSecond() * s) / RobotRotationSpeed);
}

void BaseController::SetTargetTravelDistance(argos::Real newTargetDistance) {
	// convert meters into cm
	argos::Real d = newTargetDistance * 100.0;
	TicksToWaitWhileMoving = std::ceil((SimulationTicksPerSecond() * d) / RobotForwardSpeed);
}

void BaseController::SetLeftTurn(argos::Real newAngleToTurnInDegrees) {
	if(newAngleToTurnInDegrees > 0.0) {
		SetTargetAngleDistance(newAngleToTurnInDegrees);
		CurrentMovementState = LEFT;
	} else if(newAngleToTurnInDegrees < 0.0) {
		SetTargetAngleDistance(-newAngleToTurnInDegrees);
		CurrentMovementState = RIGHT;  
	} else {
		Stop();
	}
}

void BaseController::SetRightTurn(argos::Real newAngleToTurnInDegrees) {
	if(newAngleToTurnInDegrees > 0.0) {
		SetTargetAngleDistance(newAngleToTurnInDegrees);
		CurrentMovementState = RIGHT;
	} else if(newAngleToTurnInDegrees < 0.0) {
		SetTargetAngleDistance(-newAngleToTurnInDegrees);
		CurrentMovementState = LEFT;
	} else {
		Stop();
	}
}

void BaseController::SetMoveForward(argos::Real newTargetDistance) {
	if(newTargetDistance > 0.0) {
		SetTargetTravelDistance(newTargetDistance);
		CurrentMovementState = FORWARD;
	} else if(newTargetDistance < 0.0) {
		SetTargetTravelDistance(newTargetDistance);
		CurrentMovementState = BACK;
	} else {
		Stop();
	}
}

void BaseController::SetMoveBack(argos::Real newTargetDistance) {
	if(newTargetDistance > 0.0) {
		SetTargetTravelDistance(newTargetDistance);
		CurrentMovementState = BACK;
	} else if(newTargetDistance < 0.0) {
		SetTargetTravelDistance(newTargetDistance);
		CurrentMovementState = FORWARD;
	} else {
		Stop();
	}
}

void BaseController::PushMovement(size_t moveType, argos::Real moveSize) {
	Movement newMove = { moveType, moveSize };
	MovementStack.push(newMove);
}

void BaseController::PopMovement() {
	Movement nextMove = MovementStack.top();

	previous_movement = nextMove;

	MovementStack.pop();

	switch(nextMove.type) {

		case STOP: {
			Stop();
			break;
		}

		case LEFT: {
			SetLeftTurn(nextMove.magnitude);
			break;
		}

		case RIGHT: {
			SetRightTurn(nextMove.magnitude);
			break;
		}

		case FORWARD: {
			SetMoveForward(nextMove.magnitude);
			break;
		}

		case BACK: {
			SetMoveBack(nextMove.magnitude);
			break;
		}

	}

}

bool BaseController::CollisionDetection() {

	argos::CVector2 collisionVector = GetCollisionVector();
	argos::Real collisionAngle = ToDegrees(collisionVector.Angle()).GetValue();
	bool isCollisionDetected = false;

	if(GoStraightAngleRangeInDegrees.WithinMinBoundIncludedMaxBoundIncluded(collisionAngle)
		 && collisionVector.Length() > 0.0) {
	 
		Stop();
		 isCollisionDetected = true;
		 collision_counter++;
		while(MovementStack.size() > 0) MovementStack.pop();

		PushMovement(FORWARD, SearchStepSize);

		if(collisionAngle <= 0.0)  {
			//argos::LOG << collisionAngle << std::endl << collisionVector << std::endl << std::endl;
			SetLeftTurn(37.5 - collisionAngle);
		} else {
			//argos::LOG << collisionAngle << std::endl << collisionVector << std::endl << std::endl;
			SetRightTurn(37.5 + collisionAngle);
		}
	}

	return isCollisionDetected;
}

argos::CVector2 BaseController::GetCollisionVector() {
	/* Get readings from proximity sensor */
	const argos::CCI_FootBotProximitySensor::TReadings& proximityReadings = proximitySensor->GetReadings();

	/* Sum them together */
	argos::CVector2 collisionVector;

	for(size_t i = 0; i < proximityReadings.size(); ++i) {
		collisionVector += argos::CVector2(proximityReadings[i].Value, proximityReadings[i].Angle);
	}

	collisionVector /= proximityReadings.size();

	return collisionVector;
}

void BaseController::Stop() {
	SetTargetTravelDistance(0.0);
	SetTargetAngleDistance(0.0);
	TicksToWaitWhileMoving = 0.0;
	CurrentMovementState = STOP;
}

void BaseController::Move() {

	if(Wait() == true) return;

	CollisionDetection();

	/* move based on the movement state flag */
	switch(CurrentMovementState) {

		/* stop movement */
		case STOP: {
			//argos::LOG << "STOP\n";
			wheelActuator->SetLinearVelocity(0.0, 0.0);
			SetNextMovement();
			break;
		}

		/* turn left until the robot is facing an angle inside of the TargetAngleTolerance */
		case LEFT: {
			if((TicksToWaitWhileMoving--) <= 0.0) {
				Stop();
			} else {
				//argos::LOG << "LEFT\n";
				wheelActuator->SetLinearVelocity(-RobotRotationSpeed, RobotRotationSpeed);
			}
			break;
		}

		/* turn right until the robot is facing an angle inside of the TargetAngleTolerance */
		case RIGHT: {
			if((TicksToWaitWhileMoving--) <= 0.0) {
				Stop();
			} else {
				//argos::LOG << "RIGHT\n";
				wheelActuator->SetLinearVelocity(RobotRotationSpeed, -RobotRotationSpeed);
			}
			break;
		}

		/* move forward until the robot has traveled the specified distance */
		case FORWARD: {
			if((TicksToWaitWhileMoving--) <= 0.0) {
				Stop();
			} else {
			//argos::LOG << "FORWARD\n";
				wheelActuator->SetLinearVelocity(RobotForwardSpeed, RobotForwardSpeed);             
			}
			break;
		}

		/* move backward until the robot has traveled the specified distance */
		case BACK: {
			if((TicksToWaitWhileMoving--) <= 0.0) {
				Stop();
			} else {
			//argos::LOG << "BACK\n";
				wheelActuator->SetLinearVelocity(-RobotForwardSpeed, -RobotForwardSpeed);
			}
			break;
		}
	}
}

bool BaseController::Wait() {

	bool wait = false;

	if(WaitTime > 0) {
		WaitTime--;
		wait = true;
	}

	return wait;
}

void BaseController::Wait(size_t wait_time_in_seconds) {

	WaitTime += (wait_time_in_seconds * SimulationTicksPerSecond());

}

size_t BaseController::SimulationTick() {
	return LF.GetSpace().GetSimulationClock();
}

size_t BaseController::SimulationTicksPerSecond() {
	return LF.GetSimulator().GetPhysicsEngine("default").GetInverseSimulationClockTick();
}

argos::Real BaseController::SimulationSecondsPerTick() {
	return LF.GetSimulator().GetPhysicsEngine("default").GetSimulationClockTick();
}

argos::Real BaseController::SimulationTimeInSeconds() {
	return (argos::Real)(SimulationTick()) * SimulationSecondsPerTick();
}

bool BaseController::IsAtTarget() 
{
	argos::CRadians AngleTol;
	float DistTol;
	
	// Allow the searcher to treat movement to the nest
	// differently than other movement
	if (heading_to_nest)
	{
		DistTol = NestDistanceTolerance;
		AngleTol = NestAngleTolerance;
	}
	else
	{
		DistTol = TargetDistanceTolerance;
		AngleTol = TargetAngleTolerance;
	}

	argos::Real distanceToTarget = (TargetPosition - GetPosition()).Length();

	//argos::LOG << "IsAtTarget: Distance to Target: " << distanceToTarget << endl;
	//argos::LOG << "IsAtTarget: TargetDistanceTolerance: " << DistTol << endl;

	return (distanceToTarget < DistTol) ? (true) : (false);
}

//REGISTER_CONTROLLER(BaseController, "BaseController")