
// <<<<<<<<<< includes >>>>>>>>>> //
#include "World.h"
#include "UniformGrid.h"
#include "RigidBody.h"

World::World(float wsIN, float prIN, float scIN, float dcIN){

	cout << "world: world constr called!" << endl; //zum test

	worldSize = wsIN;
	partRadius = prIN;
	springCoeff = scIN;
	dampCoeff = dcIN;
	gravity = 9.81f;			//fester wert
	allBodyNum = 0;
	allParticles = 0;
	allPartNum = 0;
	grid = new UniformGrid();

}

World::~World(){

	//...
}

void World::stepPhysics(float duration, bool isGPU){

	cout << "world: step physics!" << endl; //zum test

	//unterteilen in cpu und gpu

	//ausf�hrung auf gpu
	if(isGPU == true){
		Cuda::getInstance()->stepCUDA();
	}
	//ausf�hrung auf cpu
	else{

		//update part. values
		//for(std::vector<RigidBody*>::iterator it = allBodies.begin(); it != allBodies.end(); ++it){
		for (int i = 0; i < allBodyNum; i++){
			//(*it)->updatePartValues();			//in array ge�ndert
			allBodies[i]->updatePartValues();
		}	

		//update grid
		UniformGrid::getInstance()->updateGrid();

		//update momenta
		//for(std::vector<RigidBody*>::iterator it = allBodies.begin(); it != allBodies.end(); ++it){
		for (int i = 0; i < allBodyNum; i++){
			//(*it)->updateMomenta(duration);		//in array ge�ndert
			allBodies[i]->updateMomenta(duration);
		}

		//iterate
		//for(std::vector<RigidBody*>::iterator it = allBodies.begin(); it != allBodies.end(); ++it){
		for (int i = 0; i < allBodyNum; i++){
			//(*it)->iterate(duration);				//in array ge�ndert
			allBodies[i]->iterate(duration);
		}
	}
}
