
#ifndef COLLISIONSHAPE_H_
#define COLLISIONSHAPE_H_

// <<<<<<<<<< includes >>>>>>>>>> //
#include <vector>

#include "Particle.h"

/** \brief Collision Shape
 *
 * represents the shape of the RigidBody
 */
class CollisionShape {

protected:

	glm::vec3 origin;			/**< origin of the shape = position of RigidBody */
	float mass;					/**< mass of the shape = mass of RigidBody */

	//std::vector<Particle*> bodyParticles;	//besser ist array
	//**
	Particle** bodyParticles;	/**< array with body representing particles */
	int numOfPart;				/**< number of body representing particles */

public:
	/** \brief constructor
	 *
	 * ...
	 */
	CollisionShape(glm::vec3 origIN, float massIN, Particle** bPartIN, int numPartIN);	//bodyPart noch dazu?!

	/** \brief destructor
	 *
	 * ...
	 */
	virtual ~CollisionShape();

	/** \brief create particles
	 *
	 * ...
	 */
	virtual void calcParticles() = 0;		//createParticles

	/** \brief apply rotation to body particles
	 *
	 * ...
	 */
	virtual void applyRotToPart(glm::mat3 rotMatrix) = 0;	//von wo aufgerufen vorher updateRotationMatrix();

	/** \brief fill particle array
	 *
	 * ...
	 */
	virtual void populatePartArray() = 0;	//virtual?!

	// <<<<<<<<<< getter + setter >>>>>>>>>> //
	int getNumOfPart() const {
		return numOfPart;
	}

	void setNumOfPart(int numOfPart) {
		this->numOfPart = numOfPart;
	}

	 Particle** getBodyParticles() const {					//falsch generiert?!
		return bodyParticles;
	}

	void setBodyParticles( Particle**& bodyParticles) {		//falsch generiert?!
		this->bodyParticles = bodyParticles;
	}
};

#endif /* COLLISIONSHAPE_H_ */
