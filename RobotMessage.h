/** \file
 *  Messages used for intertask communications
 */

/** Defines the messages we pass from task to task.
 *
 * The RobotMessage struct is a data structure used to pass information to the
 * robot's components. It is composed of a command that indicates the action to
 * be carried out and a union of params that contain additional data.
 */

#ifndef ROBOT_MESSAGE_H
#define ROBOT_MESSAGE_H

/**
 \msc
 arcgradient = 8;
 robot [label="Main\nRobot"],
 auto [label="Autonomous"],
 check [label="Check\nList"],
 drive [label="Drive\nTrain"],
 test [label="Component\nExample"];
 robot=>* [label="SYSTEM_MSGTIMEOUT"];
 robot=>* [label="SYSTEM_OK"];
 robot=>* [label="SYSTEM_ERROR"];
 robot=>* [label="STATE_DISABLED"];
 robot=>* [label="STATE_AUTONOMOUS"];
 robot=>* [label="STATE_TELEOPERATED"];
 robot=>* [label="STATE_TEST"];
 robot=>* [label="STATE_UNKNOWN"];
 robot=>auto [label="RUN"];
 robot=>check [label="RUN"];
 robot=>drive [label="STOP"];
 robot=>drive [label="DRIVE_TANK"];
 robot=>drive [label="DRIVE_ARCADE"];
 auto=>drive [label="DRIVE_STRAIGHT"];
 auto=>drive [label="TURN"];
 drive=>auto [label="AUTONOMOUS_RESPONSE_OK"]
 drive=>auto [label="AUTONOMOUS_RESPONSE_ERROR"]

 robot=>test[label="TEST"];
 \endmsc

 */

enum MessageCommand {
	COMMAND_UNKNOWN,					//!< COMMAND_UNKNOWN
	COMMAND_SYSTEM_MSGTIMEOUT,			//!< COMMAND_SYSTEM_MSGTIMEOUT
	COMMAND_SYSTEM_OK,					//!< COMMAND_SYSTEM_OK
	COMMAND_SYSTEM_ERROR,				//!< COMMAND_SYSTEM_ERROR

	COMMAND_ROBOT_STATE_DISABLED,		//!< Tells all components that the robot is disabled
	COMMAND_ROBOT_STATE_AUTONOMOUS,		//!< Tells all components that the robot is in auto
	COMMAND_ROBOT_STATE_TELEOPERATED,	//!< Tells all components that the robot is in teleop
	COMMAND_ROBOT_STATE_TEST,			//!< Tells all components that the robot is in test
	COMMAND_ROBOT_STATE_UNKNOWN,		//!< Tells all components that the robot's state is unknown

	COMMAND_AUTONOMOUS_RUN,				//!< Tells Autonomous to run
	COMMAND_AUTONOMOUS_COMPLETE,		//!< Tells all components that Autonomous is done running the script
	COMMAND_AUTONOMOUS_RESPONSE_OK,		//!< Tells Autonomous that a command finished running successfully
	COMMAND_AUTONOMOUS_RESPONSE_ERROR,	//!< Tells Autonomous that a command had a error while running
	COMMAND_CHECKLIST_RUN,				//!< Tells CheckList to run

	COMMAND_DRIVETRAIN_STOP,			//!< Tells Drivetrain to stop moving
	COMMAND_DRIVETRAIN_DRIVE_TANK,		//!< Tells Drivetrain to use tank drive
	COMMAND_DRIVETRAIN_DRIVE_ARCADE,	//!< Tells Drivetrain to use arcade drive
	COMMAND_DRIVETRAIN_AUTO_MOVE,		//!< Tells Drivetrain to move motors, used by Autonomous
	COMMAND_DRIVETRAIN_DRIVE_STRAIGHT,	//!< Tells Drivetrain to drive straight, used by Autonomous
	COMMAND_DRIVETRAIN_TURN,			//!< Tells Drivetrain to turn, used by Autonomous
	COMMAND_DRIVETRAIN_START_DRIVE_FWD,	//!< Tells Drivetrain to front load the next tote, used by Autonomous
	COMMAND_DRIVETRAIN_START_DRIVE_BCK,	//!< Tells Drivetrain to back load the next tote, used by Autonomous
	COMMAND_DRIVETRAIN_START_KEEPALIGN,	//!< Tells Drivetrain to start keeping itself at constant alignment, used by Autonomous
	COMMAND_DRIVETRAIN_STOP_KEEPALIGN,	//!< Tells Drivetrain to stop keeping itself at constant alignment, used by Autonomous

	COMMAND_COMPONENT_TEST,				//!< COMMAND_COMPONENT_TEST

	COMMAND_LAST                      //!< COMMAND_LAST 
};
///Used to deliver joystick readings to Drivetrain
struct TankDriveParams {
	float left;
	float right;
};

///Used to deliver joystick readings to Drivetrain
struct ArcadeDriveParams {
	float x;
	float y;
};

///Used to deliver autonomous values to Drivetrain
struct AutonomousParams {
	unsigned uMode;
	unsigned uDelay;
	///how long a function can run, maximum
	float timeout;
	///how long until a function performs
	float timein;

	///used by drivetrain for straight driving
	float driveSpeed;
	float driveDistance;
	float turnAngle;
	float driveTime;
};

///Contains all the parameter structures contained in a message
union MessageParams {
	TankDriveParams tankDrive;
	ArcadeDriveParams arcadeDrive;
	AutonomousParams autonomous;
};

///A structure containing a command, a set of parameters, and a reply id, sent between components
struct RobotMessage {
	MessageCommand command;
	const char* replyQ;
	MessageParams params;
};

#endif //ROBOT_MESSAGE_H
