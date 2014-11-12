
// <<<<<<<<<< includes >>>>>>>>>> //
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_gl_interop.h>
#include <iostream>

#include "Cuda.h"
#include "World.h"
#include "RigidBody.h"
#include "Particle.h"

#include "UniformGrid_kernels.cu"
#include "RigidBody_kernels.cu"
#include "Particle_kernels.cu"

using namespace std;

Cuda::Cuda(int bnIN, int pnIN){

	cout << "cuda: cuda constr called!" << endl; //zum test

	bodyNum = bnIN;
	partNum = pnIN;

	h_rbMass = 0;
	h_rbForce = 0;
	h_rbPos = 0;
	h_rbVeloc = 0;
	h_rbLinMom = 0;
	h_rgRotQuat = 0;
	h_rbRotMat = 0;
	h_rbAngVeloc = 0;
	h_rbAngMom = 0;
	h_rbInitInversInertTensDiago = 0;	//Initial Inverse Inertia Tensor Diagonal
	h_rbInverseInertTens = 0;

	h_pMass = 0;
	h_pPos = 0;
	h_pVeloc = 0;
	h_pForce = 0;

	h_pGridIndex = 0;

	h_gCountGrid = 0;
	h_gIndexGrid = 0; //int4?!

	d_rbMass = 0;
	d_rbForce = 0;
	d_rbPos = 0;
	d_rbVeloc = 0;
	d_rbLinMom = 0;
	d_rgRotQuat = 0;
	d_rbRotMat = 0;
	d_rbAngVeloc = 0;
	d_rbAngMom = 0;
	d_rbInitInversInertTensDiago = 0;
	d_rbInverseInertTens = 0;

	d_pMass = 0;
	d_pPos = 0;
	d_pVeloc = 0;
	d_pForce = 0;

	d_pGridIndex = 0;

	d_gCountGrid = 0;
	d_gIndexGrid = 0; //int4?!
}

Cuda::~Cuda(){

	delete h_rgRotQuat;
	delete h_rbVeloc;
	delete h_rbRotMat;
	delete h_rbPos;
	delete h_rbMass;
	delete h_rbLinMom;
	delete h_rbInverseInertTens;
	delete h_rbInitInversInertTensDiago;
	delete h_rbForce;
	delete h_rbAngVeloc;
	delete h_rbAngMom;
	delete h_pVeloc;
	delete h_pPos;
	delete h_pMass;
	delete h_pGridIndex;
	delete h_pForce;
	delete h_gIndexGrid;
	delete h_gCountGrid;

	cudaFree(d_rbMass);
	cudaFree(d_rbForce);
	cudaFree(d_rbPos);
	cudaFree(d_rbVeloc);
	cudaFree(d_rbLinMom);
	cudaFree(d_rgRotQuat);
	cudaFree(d_rbRotMat);
	cudaFree(d_rbAngVeloc);
	cudaFree(d_rbAngMom);
	cudaFree(d_rbInitInversInertTensDiago);
	cudaFree(d_rbInverseInertTens);
	cudaFree(d_pMass);
	cudaFree(d_pPos);
	cudaFree(d_pVeloc);
	cudaFree(d_pForce);
	cudaFree(d_pGridIndex);
	cudaFree(d_gCountGrid);
	cudaFree(d_gIndexGrid);

}

void Cuda::initCUDA(){

	cout << "cuda: initCuda called!" << endl; //zum test

	//...


	//initializeHostDataArrays();
	//init host arrays
	//body arrays
	h_rbMass = new float[bodyNum];
	h_rbForce = new glm::vec3[bodyNum];
	h_rbPos = new glm::vec3[bodyNum];
	h_rbVeloc = new glm::vec3[bodyNum];
	h_rbLinMom = new glm::vec3[bodyNum];
	h_rgRotQuat = new glm::quat[bodyNum];
	h_rbRotMat = new glm::mat3[bodyNum];
	h_rbAngVeloc = new glm::vec3[bodyNum];
	h_rbAngMom = new glm::vec3[bodyNum];
	h_rbInitInversInertTensDiago = new glm::vec3[bodyNum];
	h_rbInverseInertTens = new glm::mat3[bodyNum];

	//paticle arrays
	h_pMass = new float[partNum];
	h_pPos = new glm::vec3[partNum];
	h_pVeloc = new glm::vec3[partNum];
	h_pForce = new glm::vec3[partNum];

	//initOpenCLGrid();
	//init gitter
	initCUDAGrid();

	//initializeDeviceBuffers();
	//init device arrays
	//speicher f�r arrays alokieren
	//checkCudaErrors(cudaMalloc(devPtr, size));
	cudaMalloc((void**)&d_rbMass, bodyNum*sizeof(float));
	cudaMalloc((void**)&d_rbForce, bodyNum*sizeof(glm::vec3));
	cudaMalloc((void**)&d_rbPos, bodyNum*sizeof(glm::vec3));
	cudaMalloc((void**)&d_rbVeloc, bodyNum*sizeof(glm::vec3));
	cudaMalloc((void**)&d_rbLinMom, bodyNum*sizeof(glm::vec3));
	cudaMalloc((void**)&d_rgRotQuat, bodyNum*sizeof(glm::quat));
	cudaMalloc((void**)&d_rbRotMat, bodyNum*sizeof(glm::mat3));
	cudaMalloc((void**)&d_rbAngVeloc, bodyNum*sizeof(glm::vec3));
	cudaMalloc((void**)&d_rbAngMom, bodyNum*sizeof(glm::vec3));
	cudaMalloc((void**)&d_rbInitInversInertTensDiago, bodyNum*sizeof(glm::vec3));
	cudaMalloc((void**)&d_rbInverseInertTens, bodyNum*sizeof(glm::mat3));

	//particle parameter
	cudaMalloc((void**)&d_pMass, bodyNum*sizeof(float));
	cudaMalloc((void**)&d_pPos, bodyNum*sizeof(glm::vec3));
	cudaMalloc((void**)&d_pVeloc, bodyNum*sizeof(glm::vec3));
	cudaMalloc((void**)&d_pForce, bodyNum*sizeof(glm::vec3));

	//updateHostDataArrays();
	//f�lle host arrays
	updateHostArrays();

	//writeHostDataToDevice();
	//kopiere auf gpu
	hostToDevice();
}

void Cuda::updateHostArrays(){

	cout << "cuda: updateHostArr called!" << endl; //zum test

	//entscheiden ob vector o. arrays, dann entspr. anpassungen vornehmen. ?!auch nicht sicher ob direkter zugriff �ber public nicht besser, sollte hier aber eig so passen
	RigidBody** allB = World::getInstance()->getAllBodies();
	Particle** allP = World::getInstance()->getAllParticles();
	for (int i=0; i<bodyNum; i++) {
		allB[i]->updateCUDArray(i);
	}
	for (int i=0; i<partNum; i++) {
		allP[i]->updateCUDArray(i);
	}
}

void Cuda::hostToDevice(){

	cout << "cuda: hostTOdevice called!" << endl; //zum test

	//cudaMemcpy(d_A, h_A, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbMass, h_rbMass, bodyNum*sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbForce, h_rbForce, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbPos, h_rbPos, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbVeloc, h_rbVeloc, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbLinMom, h_rbLinMom, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rgRotQuat, h_rgRotQuat, bodyNum*sizeof(glm::quat), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbRotMat, h_rbRotMat, bodyNum*sizeof(glm::mat3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbAngVeloc, h_rbAngVeloc, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbAngMom, h_rbAngMom, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbInitInversInertTensDiago, h_rbInitInversInertTensDiago, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_rbInverseInertTens, h_rbInverseInertTens, bodyNum*sizeof(glm::mat3), cudaMemcpyHostToDevice);

	cudaMemcpy(d_pMass, h_pMass, bodyNum*sizeof(float), cudaMemcpyHostToDevice);
	cudaMemcpy(d_pPos, h_pPos, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_pVeloc, h_pVeloc, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);
	cudaMemcpy(d_pForce, h_pForce, bodyNum*sizeof(glm::vec3), cudaMemcpyHostToDevice);

	//vbo data

}

void Cuda::initCUDAGrid(){

	cout << "cuda: initGrid called!" << endl; //zum test

	//TODO

	//host arrays
	h_pGridIndex = new glm::vec3[partNum];

	int gS = UniformGrid::getInstance()->getGridSize();
	h_gCountGrid = new int[gS];
	h_gIndexGrid = new glm::vec4[gS];	//int4?!

	//device arrays
	cudaMalloc((void**)&d_pGridIndex, bodyNum*sizeof(glm::vec3));

	cudaMalloc((void**)&d_gCountGrid, bodyNum*sizeof(int));
	cudaMalloc((void**)&d_gIndexGrid, bodyNum*sizeof(glm::vec4));	//int4?!
}

void Cuda::stepCUDA(){

	cout << "cuda: stepCUDA!" << endl; //zum test

	//TODO!

	//schritte nacheinander aufrufen
	//...
	resetGrid();
	updateGrid();
	calcCollForces();
	updateMom();
	iterate();
	updatePart();

	//rendern!?
	//bzw. cuda opengl austausch
	// siehe --> cuda samples - 2_graphics - simpleGL
	
	//irgwo vorher noch vbo mit cuda daten initial f�llen, hier dann immer updaten!!
	cudaGLRegisterBufferObject(bufferObj);
	cudaGLMapBufferObject((void**)&devPtr,bufferObj);

	//kernel aufrufen

	cudaGraphicsUnmapResources(1, &cuda_vbo_resource1, 0);	// give access authority of vbo1 back to openGL  
	cudaGraphicsUnregisterResource(cuda_vbo_resource1);		// unregiste the resource   

	//mit opengl rendern

}

/*
void Cuda::initHostArrays(){

	//host arrays f�llen
	//mit in init gepackt
}
*/
