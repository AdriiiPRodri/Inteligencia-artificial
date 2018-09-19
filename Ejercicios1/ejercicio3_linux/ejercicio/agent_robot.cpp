#include "agent_robot.h"
#include "environment.h"
#include <iostream>
#include <cstdlib>
#include <vector>
#include <utility>

using namespace std;

// -----------------------------------------------------------
Agent::ActionType Agent::Think()
{
	int accion = 0;
	
	if(CNY70_){
		giro = true;
		accion = 2;
		choque = true;
	}
	else if(giro){
		accion = 2;
		giro = false;
	}
	
	if(choque){
		if(CNY70_)
			acabado = true;	
		if(accion == 0)	
			contador++;
	}

	if(acabado)
		cout << "La dimensión es: " << contador << endl;

	return static_cast<ActionType> (accion);

}
// -----------------------------------------------------------
void Agent::Perceive(const Environment &env)
{
	CNY70_ = env.isFrontier();
	BUMPER_ = env.isBump();
}
// -----------------------------------------------------------
string ActionStr(Agent::ActionType accion)
{
	switch (accion)
	{
		case Agent::actFORWARD: return "FORWARD";
		case Agent::actTURN_L: return "TURN LEFT";
		case Agent::actTURN_R: return "TURN RIGHT";
		case Agent::actBACKWARD: return "BACKWARD";
		case Agent::actPUSH: return "PUSH";
		case Agent::actIDLE: return "IDLE";
		default: return "???";
	}
}
