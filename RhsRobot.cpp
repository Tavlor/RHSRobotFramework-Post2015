/** \file
 * Main robot class.
 *
 * The RhsRobot class is the main robot class. It inherits from RhsRobotBase and MUST define the Init() function, the Run() function, and
 * the OnStateChange() function.  Messages from the DS are processed and commands sent to the subsystems
 * that implement behaviors for each part for the robot.
 */

#include "RhsRobot.h"
#include "WPILib.h"

//Robot
#include "ComponentBase.h"
#include "RobotParams.h"

RhsRobot::RhsRobot() {
	Controller_1 = NULL;
	Controller_2 = NULL;
	ControllerListen_1 = NULL;
	ControllerListen_2 = NULL;
	drivetrain = NULL;
	autonomous = NULL;
	conveyor = NULL;
	cube = NULL;
	canlifter = NULL;
	claw = NULL;
	//canarm = NULL;
	noodlefan = NULL;

	bLastConveyorButtonDown = false;
	bCanlifterNearBottom = false;

	iLoop = 0;
}

RhsRobot::~RhsRobot() {
	std::vector<ComponentBase *>::iterator nextComponent = ComponentSet.begin();

	for(; nextComponent != ComponentSet.end(); ++nextComponent)
	{
		delete (*nextComponent);
	}

	delete Controller_1;
	delete Controller_2;
	delete ControllerListen_1;
	delete ControllerListen_2;
}

void RhsRobot::Init() {
	/* 
	 * Set all pointers to null and then allocate memory and construct objects
	 * EXAMPLE:	drivetrain = NULL; (in constructor)
	 * 			drivetrain = new Drivetrain(); (in RhsRobot::Init())
	 */
	Controller_1 = new Joystick(0);
	Controller_2 = new Joystick(1);
	ControllerListen_1 = new JoystickListener(Controller_1);
	ControllerListen_2 = new JoystickListener(Controller_2);
	ControllerListen_1->SetAxisTolerance(.05);
	ControllerListen_2->SetAxisTolerance(.05);
	drivetrain = new Drivetrain();
	conveyor = new Conveyor();
	canlifter = new CanLifter();
	claw = new Claw();
	cube = new Cube();
	//canarm = new CanArm();
	noodlefan = new NoodleFan();
	autonomous = new Autonomous();

	std::vector<ComponentBase *>::iterator nextComponent = ComponentSet.begin();

	if(drivetrain)
	{
		nextComponent = ComponentSet.insert(nextComponent, drivetrain);
	}

	if(conveyor)
	{
		nextComponent = ComponentSet.insert(nextComponent, conveyor);
	}

	if(cube)
	{
		nextComponent = ComponentSet.insert(nextComponent, cube);
	}

	if(canlifter)
	{
		nextComponent = ComponentSet.insert(nextComponent, canlifter);
	}

	if(claw)
	{
		nextComponent = ComponentSet.insert(nextComponent, claw);
	}

	/*if(canarm)
	{
		nextComponent = ComponentSet.insert(nextComponent, canarm);
	}*/

	if(noodlefan)
	{
		nextComponent = ComponentSet.insert(nextComponent, noodlefan);
	}

	if(autonomous)
	{
		nextComponent = ComponentSet.insert(nextComponent, autonomous);
	}
}

void RhsRobot::OnStateChange() {
	std::vector<ComponentBase *>::iterator nextComponent;

	for(nextComponent = ComponentSet.begin();
			nextComponent != ComponentSet.end(); ++nextComponent)
	{
		(*nextComponent)->SendMessage(&robotMessage);
	}
}

void RhsRobot::Run() {
	//SmartDashboard::PutString("ROBOT STATUS", "Running");
	/* Poll for control data and send messages to each subsystem. Surround blocks with if(component) so entire components can be disabled
	 * by commenting out their construction.
	 * EXAMPLE: if(drivetrain) 
	 * 			{ 
	 * 				//Check joysticks and send messages 
	 * 			}
	 */

	if(autonomous)
	{
		if(GetCurrentRobotState() == ROBOT_STATE_AUTONOMOUS)
		{
			// all messages to components will come from the autonomous task
			return;
		}
	}

	if (drivetrain)
	{
		//for keepalign tests: comment out everything to the sendMessage
		//also comment out the if (ISAUTO) at the bottom of Drivetrain::Run()
		//robotMessage.command = COMMAND_DRIVETRAIN_START_KEEPALIGN;

#if 1
		robotMessage.command = COMMAND_DRIVETRAIN_DRIVE_TANK;
			robotMessage.params.tankDrive.left = TANK_DRIVE_LEFT * fDriveMax;
			robotMessage.params.tankDrive.right = TANK_DRIVE_RIGHT * fDriveMax;

#else
		robotMessage.command = COMMAND_DRIVETRAIN_DRIVE_ARCADE;
		robotMessage.params.arcadeDrive.x = ARCADE_DRIVE_X;
		robotMessage.params.arcadeDrive.y = ARCADE_DRIVE_Y;
#endif
		drivetrain->SendMessage(&robotMessage);
	}

	if(conveyor)
	{
		if(CONVEYOR_FWD)
		{ //only used for autonomous and depositing cans
			//SmartDashboard::PutString("Conveyor Mode", "Output Front");
			robotMessage.command = COMMAND_CONVEYOR_RUN_FWD;
			//if(!bLastConveyorButtonDown)
			//{
			//	robotMessage.params.conveyorParams.bButtonWentDownEvent = true;
			//	bLastConveyorButtonDown = true;
			//}
			//else
			//{
			//	robotMessage.params.conveyorParams.bButtonWentDownEvent = false;
			//}
		}
		else if(CONVEYOR_BCK)
		{ //used to intake and deposit totes
			//SmartDashboard::PutString("Conveyor Mode", "Output Back");
			robotMessage.command = COMMAND_CONVEYOR_RUN_BCK;

			//if(!bLastConveyorButtonDown)
			//{
			//	robotMessage.params.conveyorParams.bButtonWentDownEvent = true;
			//	bLastConveyorButtonDown = true;
			//}
			//else
			//{
			//	robotMessage.params.conveyorParams.bButtonWentDownEvent = false;
			//}
		}
		else
		{
			//SmartDashboard::PutString("Conveyor Mode", "Stopped");
			robotMessage.command = COMMAND_CONVEYOR_STOP;
			//robotMessage.params.conveyorParams.bButtonWentDownEvent = false;
			//bLastConveyorButtonDown = false;
		}

		conveyor->SendMessage(&robotMessage);
	}

	if(cube)
	{
		if(ControllerListen_1->ButtonPressed(CUBEAUTO_START_ID) ||
				ControllerListen_2->ButtonPressed(CUBEAUTO_START_ID))
		{
			//SmartDashboard::PutString("Robot->Cube", "Auto Start");
			robotMessage.command = COMMAND_CUBEAUTOCYCLE_START;
			cube->SendMessage(&robotMessage);
		}
		else if(ControllerListen_1->ButtonPressed(CUBEAUTO_STOP_ID) ||
				ControllerListen_2->ButtonPressed(CUBEAUTO_STOP_ID))
		{
			//SmartDashboard::PutString("Robot->Cube", "Auto Stop");
			robotMessage.command = COMMAND_CUBEAUTOCYCLE_STOP;
			cube->SendMessage(&robotMessage);
		}
		else if(ControllerListen_1->ButtonPressed(CUBEAUTO_HOLD_ID) ||
				ControllerListen_2->ButtonPressed(CUBEAUTO_HOLD_ID))
		{
			//SmartDashboard::PutString("Robot->Cube", "Hold");
			robotMessage.command = COMMAND_CUBEAUTOCYCLE_HOLD;
			cube->SendMessage(&robotMessage);
		}
		else if(ControllerListen_1->ButtonPressed(CUBEAUTO_RELEASE_ID) ||
				ControllerListen_2->ButtonPressed(CUBEAUTO_RELEASE_ID))
		{
			//SmartDashboard::PutString("Robot->Cube", "Release");
			robotMessage.command = COMMAND_CUBEAUTOCYCLE_RELEASE;
			cube->SendMessage(&robotMessage);
		}

		if(CUBECLICKER_RAISE)
		{
			robotMessage.command = COMMAND_CUBECLICKER_RAISE;
		}
		else if(CUBECLICKER_LOWER)
		{
			robotMessage.command = COMMAND_CUBECLICKER_LOWER;
		}
		else
		{
			robotMessage.command = COMMAND_CUBECLICKER_STOP;
		}

		cube->SendMessage(&robotMessage);

		//Intake should always run
		/*if(CUBEINTAKE_RUN)
		{
			robotMessage.command = COMMAND_CUBEINTAKE_RUN;
		}
		else
		{
			robotMessage.command = COMMAND_CUBEINTAKE_STOP;
		}

		cube->SendMessage(&robotMessage);*/
	}

	if(canlifter)
	{
		/*if (canlifter->GetHallEffectBottom()
				|| (canlifter->GetHallEffectMiddle() && CANLIFTER_LOWER > .1))
		{
			//if lift is at bottom or lowering past the middle hall effect
			bCanlifterNearBottom = true;
		}
		if (canlifter->GetHallEffectTop()
				|| (canlifter->GetHallEffectMiddle() && CANLIFTER_RAISE > .1))
		{
			//if lift is at top or raising past the middle hall effect
			bCanlifterNearBottom = false;
		}

		SmartDashboard::PutBoolean("Can Lift near bottom", bCanlifterNearBottom);*/

		if(CANLIFTER_RAISE > .1)
		{
			robotMessage.command = COMMAND_CANLIFTER_RAISE;
			robotMessage.params.canLifterParams.lifterSpeed = CANLIFTER_RAISE;
		}
		else if(CANLIFTER_LOWER > .1)
		{
			robotMessage.command = COMMAND_CANLIFTER_LOWER;
			robotMessage.params.canLifterParams.lifterSpeed = CANLIFTER_LOWER;
		}
		/*else if(ControllerListen_1->ButtonPressed(CANLIFTER_HOVER_ID))
		{
			robotMessage.command = COMMAND_CANLIFTER_HOVER;
		}*/
		else
		{
			robotMessage.command = COMMAND_CANLIFTER_STOP;
		}

		canlifter->SendMessage(&robotMessage);
	}

	if (claw)
	{
		claw->SendMessage(&robotMessage);

		if (ControllerListen_1->ButtonPressed(CLAW_OPEN_ID))
		{
			robotMessage.command = COMMAND_CLAW_OPEN;
		}
		else if (ControllerListen_1->ButtonPressed(CLAW_CLOSE_ID))
		{
			robotMessage.command = COMMAND_CLAW_CLOSE;
		}

		claw->SendMessage(&robotMessage);
	}

	/*if (canarm)
	{
		if(CANARM_OPEN)
		{
			robotMessage.command = COMMAND_CANARM_OPEN;
		}
		else if(CANARM_CLOSE)
		{
			robotMessage.command = COMMAND_CANARM_CLOSE;
		}
		else
		{
			robotMessage.command = COMMAND_CANARM_STOP;
		}

		canarm->SendMessage(&robotMessage);

	}*/

	if(noodlefan)
	{
		if(ControllerListen_1->ButtonPressed(NOODLEFAN_TOGGLE_ID))
		{
			robotMessage.command = COMMAND_NOODLEFAN_TOGGLE;
		}
		noodlefan->SendMessage(&robotMessage);
	}

	ControllerListen_1->FinalUpdate();
	ControllerListen_2->FinalUpdate();
	iLoop++;
}

START_ROBOT_CLASS(RhsRobot)
