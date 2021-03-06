
// <<<<<<<<<< includes >>>>>>>>>> //
#include "Particle.h"
#include "UniformGrid.h"
#include "World.h"
#include "Cuda.h"
#include "DemoApp\Demo.h"

//link fix try 4
extern World* world;
extern Cuda* cuda;
extern Demo* demo;

int Particle::indexCount = 0;

Particle::Particle(glm::vec3 posIN, float massIN){

	position = posIN;
	
	mass = massIN;
	velocity = glm::vec3(0,0,0);
	force = glm::vec3(0,0,0);

	gridIndex = glm::ivec3(0,0,0);
	partIndex = 0;

	partNode = new CVK::Node();
	partNode->setGeometry(demo->partGeometry);
	partNode->setMaterial((demo->partMaterial));
	partNode->setModelMatrix(glm::translate(glm::mat4(1.0f), position));
	demo->partRoot->addChild(partNode);
	//demo->sceneRoot->addChild(partNode);	//!for transparency!
}

Particle::~Particle(){

	delete partNode;
}

glm::vec3 Particle::calculateForces(bool wgIN){

	force = glm::vec3(0,0,0);

	float partR = world->getPartRadius();
	float worldS = world->getWorldSize();
	float springC = world->getSpringCoeff();
	float dampC = world->getDampCoeff();

	//-----part1-----
	//calculateCollisionForces();		//ohne gitter
	if (wgIN == false){

		int numP = world->getAllPartNum();
		for (int j = 0; j<numP; j++) {
			if (j != this->partIndex) {

				glm::vec3 jPos = world->allParticles[j]->getPosition();

				glm::vec3 distance;
				distance.x = jPos.x - position.x;
				distance.y = jPos.y - position.y;
				distance.z = jPos.z - position.z;

				float absDistance = sqrt(distance[0] * distance[0] +
										 distance[1] * distance[1] +
										 distance[2] * distance[2]);

				if (absDistance + 0.00001f < (2.0f * partR)) {
					force.x = force.x - springC*(2.0f*partR - absDistance)*(distance.x / absDistance);
					force.y = force.y - springC*(2.0f*partR - absDistance)*(distance.y / absDistance);
					force.z = force.z - springC*(2.0f*partR - absDistance)*(distance.z / absDistance);

					glm::vec3 jVel = world->allParticles[j]->getVelocity();
					glm::vec3 relativeVelocity;
					relativeVelocity.x = jVel.x - velocity.x;
					relativeVelocity.y = jVel.y - velocity.y;
					relativeVelocity.z = jVel.z - velocity.z;

					force.x = force.x + dampC*relativeVelocity.x;
					force.y = force.y + dampC*relativeVelocity.y;
					force.z = force.z + dampC*relativeVelocity.z;
				}
			}
		}
	}
	
	//-----part2-----
	//calculateCollisionForcesWithGrid();	//mit gitter
	if (wgIN == true){

		if (UniformGrid::getInstance()->isValidGridIndex(gridIndex) == true) {
			
			int* neighborParticles = UniformGrid::getInstance()->getNeighborPartIndices(gridIndex);
			int ppv = UniformGrid::getInstance()->getPartPerVoxel();
		
			int target = ppv * 27;
			for (int i=0; i<target; i++) {
				int neighborIndex = neighborParticles[i];
				if (neighborIndex != -1 && neighborIndex != this->partIndex) {
	
					Particle* neighbor = world->allParticles[neighborIndex];
					glm::vec3 jPos = neighbor->getPosition();

					glm::vec3 distance;
					distance.x = jPos.x - position.x;
					distance.y = jPos.y - position.y;
					distance.z = jPos.z - position.z;
	
					float absDistance = sqrt(distance.x*distance.x + distance.y*distance.y + distance.z*distance.z);

					if (absDistance + 0.00001f < (2.0f * partR)) {
						force.x = force.x - springC*(2.0f*partR - absDistance)*(distance.x/absDistance);
						force.y = force.y - springC*(2.0f*partR - absDistance)*(distance.y/absDistance);
						force.z = force.z - springC*(2.0f*partR - absDistance)*(distance.z/absDistance);

						glm::vec3 jVel = neighbor->getVelocity();

						glm::vec3 relativeVelocity;
						relativeVelocity.x = jVel.x - velocity.x;
						relativeVelocity.y = jVel.y - velocity.y;
						relativeVelocity.z = jVel.z - velocity.z;

						force.x = force.x + dampC*relativeVelocity.x;
						force.y = force.y + dampC*relativeVelocity.y;
						force.z = force.z + dampC*relativeVelocity.z;
					}
				}
			}
			delete neighborParticles;
		}
	}

	//-----part3-----
	//calculateBoundaryForces();
	bool collisionOccured = false;

	// Ground collision
	if (position.y-partR < 0.0f) {

		collisionOccured = true;
		force.y = force.y + springC*(partR - position.y);
	}

	// X-axis Wall Collision
	if (position.x-partR < -worldS) {
		
		collisionOccured = true;
		force.x = force.x + springC*(-worldS - position.x + partR);
	} else if (position.x+partR > worldS) {
		
		collisionOccured = true;
		force.x = force.x + springC*(worldS - position.x - partR);
	}

	// Z-axis Wall Collision
	if (position.z-partR < -worldS) {
		
		collisionOccured = true;
		force.z = force.z + springC*(-worldS - position.z + partR);
	} else if (position.z+partR > worldS) {
		
		collisionOccured = true;
		force.z = force.z + springC*(worldS - position.z - partR);
	}

	// Damping
	if (collisionOccured) {
		force.x = force.x - dampC*velocity.x;
		force.y = force.y - dampC*velocity.y;
		force.z = force.z - dampC*velocity.z;
	}
	return force;
}

void Particle::updateVeloc(glm::vec3 bodyPosition, glm::vec3 bodyVelocity, glm::vec3 bodyAngularVelocity){

	velocity = glm::vec3(0,0,0);

	float scalar = sqrt(bodyAngularVelocity.x*bodyAngularVelocity.x +
						bodyAngularVelocity.y*bodyAngularVelocity.y +
						bodyAngularVelocity.z*bodyAngularVelocity.z);

	if (scalar > 0.0f) {
		scalar = scalar * scalar;
		glm::vec3 relativePosition = glm::vec3( position.x - bodyPosition.x,
												position.y - bodyPosition.y,
												position.z - bodyPosition.z );

		scalar = (bodyAngularVelocity.x*relativePosition.x +
				  bodyAngularVelocity.y*relativePosition.y +
				  bodyAngularVelocity.z*relativePosition.z) / scalar;

		glm::vec3 term;
		term.x = relativePosition.x - bodyAngularVelocity.x * scalar;
		term.y = relativePosition.y - bodyAngularVelocity.y * scalar;
		term.z = relativePosition.z - bodyAngularVelocity.z*scalar;

		velocity.x = (bodyAngularVelocity.y * term.z - bodyAngularVelocity.z * term.y);
		velocity.y = (bodyAngularVelocity.z * term.x - bodyAngularVelocity.x * term.z);
		velocity.z = (bodyAngularVelocity.x * term.y - bodyAngularVelocity.y * term.x);
	}
	velocity.x = velocity.x + bodyVelocity.x;
	velocity.y = velocity.y + bodyVelocity.y;
	velocity.z = velocity.z + bodyVelocity.z;

}

void Particle::applyRot(glm::mat3 rotatMatrix, glm::vec3 relatPos, glm::vec3 bodyPos){

	position.x = relatPos.x*rotatMatrix[0].x +
			relatPos.y*rotatMatrix[0].y +
			relatPos.z*rotatMatrix[0].z;

	position.y = relatPos.x*rotatMatrix[1].x +
			relatPos.y*rotatMatrix[1].y +
			relatPos.z*rotatMatrix[1].z;

	position.z = relatPos.x*rotatMatrix[2].x +
			relatPos.y*rotatMatrix[2].y +
			relatPos.z*rotatMatrix[2].z;

	position.x = position.x + bodyPos.x;
	position.y = position.y + bodyPos.y;
	position.z = position.z + bodyPos.z;

	//modelmatrix von node aktualisieren
	glm::mat4 modMat = glm::mat4(glm::translate(glm::mat4(1.0f), position));
	partNode->setModelMatrix(modMat);
}

void Particle::populateArray(){

	partIndex = indexCount;

	world->allParticles[indexCount] = this;
	indexCount++;
}

/*void Particle::reset(float* oldBodyPos, float* newBodyPos){

	//...
}*/

void Particle::updateGridIndex(){

	float gmp = UniformGrid::getInstance()->getGridMinPos();
	float vS = UniformGrid::getInstance()->getVoxelSize();

	gridIndex.x = (int)((position.x - gmp)/vS);
	gridIndex.y = (int)((position.y - vS) / vS);
	//gridIndex.y = ((position.y - gmp) / vS);
	gridIndex.z = (int)((position.z - gmp) / vS);
}

void Particle::updateCUDArray(int particleIndex){

	int i = particleIndex;
	cuda->h_pMass[i] = mass;
	cuda->h_pPos[i] = position;
	cuda->h_pVeloc[i] = velocity;
	cuda->h_pForce[i] = force;
}

/*void Particle::updateNode(int i){
	glm::vec3 temP = cuda->h_uVOpPos[i];
	glm::mat4 modMat = glm::mat4(glm::translate(glm::mat4(1.0f), temP));
	partNode->setModelMatrix(modMat);
}*/