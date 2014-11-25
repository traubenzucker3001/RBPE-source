
#ifndef RIGIDBODY_H_
#define RIGIDBODY_H_

// <<<<<<<<<< includes >>>>>>>>>> //
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>

#include "CollisionShape.h"
#include "Box.h"
#include "Sphere.h"

/** \brief RigidBody
 *
 * class for rigid body representation
 */
class RigidBody {

private:

	float mass;								/**< body mass */
	float inverseMass;						/**< body inverse mass */	//ka ob n�tig

	glm::vec3 position;						/**< body position */
	glm::vec3 velocity;						/**< body velocity */

	glm::vec3 lastFrameVeloc;				/**< body last frame velocity */	//ka ob n�tig

	glm::quat rotationQuat;					/**< body rotation quaternion */
	glm::mat3 rotationMat;					/**< body rotation matrix */

	glm::mat3 inertiaTensor;				/**< body inertia tensor */
	glm::vec3 initInverseInertTensDiagon;	/**< initial inverse inertia tensor diagonal */	//float initialInverseInertiaTensorDiagonal[3];//
	glm::mat3 inverseInertiaTensor;			/**< inverse inertia tensor */	//world oder local coords?!

	//rotation
	glm::vec3 angularVelocity;				/**< angular velocity */
	glm::vec3 angularMomentum;				/**< angular momentum */

	glm::vec3 linearMomentum;				/**< linear momentum */
	float terminalMom;						/**< terminat momentum */	//?

	glm::vec3 force;						/**< body force */	//ehem. forcetobeapplied

	//glm::vec3 torgueToBeApplied;	//ka ob n�tig
	//glm::vec3 lastFrameVeloc;	//ka ob n�tig

	bool isStatic;							/**< true if object is static, false if object is dynamic */

	//float linearDamp;		//welt oder jeder body einzeln?!. im mom allgemeiner welt param
	//float angularDamp;

	glm::mat4 transformMatrix;				/**< transformation matrix */

	CollisionShape *shape;					/**< body shape */	//partikel erst in collshape

	static int count;						/**< help variable for all bodies array/vector */

public:

	/** \brief constructor
	 *
	 * creates a rigid body instance with given parameters
	 * @param massIN mass
	 * @param staticIN static object (true) or not (false) (not observed at the moment)
	 * @param shapeIN collision shape, true for sphere, false for box (only box is possible at the moment)
	 * @param posIN position
	 * @param sizeIN box half size or sphere radius
	 */
	RigidBody(float massIN, bool staticIN, bool shapeIN, glm::vec3 posIN, float sizeIN);

	/** \brief standard constructor
	*
	* creates a particle instance
	*/
	RigidBody();

	/** \brief destructor
	 *
	 * destroys a rigid body instance
	 */
	~RigidBody();

	/** \brief iterate
	 *
	 * perform linear and angular step to update velocity, position and rotation
	 * @param duration time length of a simulation step
	 * @return void
	 */
	void iterate(float duration);		//performStep, mehrere schritte zusammenfassen

	/** \brief update rotations matrix
	 *
	 * update rotation matrix with new rotation quaternion
	 * @return void
	 */
	void updateRotMatrix();

	/** \brief update inverse inertia tensor
	 *
	 * update inertia tensor with new rotation matrix
	 * @return void
	 */
	void updateInverseInertiaTensor();

	/** \brief update particle values
	 *
	 * update particle positions
	 * @return void
	 */
	void updatePartValues();	//runter in collshape greifen

	/** \brief update momenta
	 *
	 * apply gravity and update linear and angular momenta
	 * @param duration time length of a simulation step
	 * @return void
	 */
	void updateMomenta(float duration); //runter in collshape greifen und schauen ob gravity klasse noch n�tig

	/** \brief reset body
	 *
	 * reset a rigid body to a given position
	 * -not yet implemented-
	 * @param newPosition new origin of the rigid body
	 * @return void
	 */
	void reset(float newPosition);


	// <<<<<<<<<< gpu parts >>>>>>>>>> //
	/** \brief update cuda array
	 *
	 * ...
	 * @param bodyIndex rigid body uniform grid index
	 * @return void
	 */
	void updateCUDArray(int bodyIndex);


	// <<<<<<<<<< getter + settter >>>>>>>>>> //
	const glm::vec3& getPosition() const {
		return position;
	}

	void setPosition(const glm::vec3& position) {
		this->position = position;
	}

	const glm::mat3& getRotationMat() const {
		return rotationMat;
	}

	void setRotationMat(const glm::mat3& rotationMat) {
		this->rotationMat = rotationMat;
	}

	const glm::quat& getRotationQuat() const {
		return rotationQuat;
	}

	void setRotationQuat(const glm::quat& rotationQuat) {
		this->rotationQuat = rotationQuat;
	}
};

#endif /* RIGIDBODY_H_ */
