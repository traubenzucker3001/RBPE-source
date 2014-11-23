//!noch particle array zugriffs fehler!

// <<<<<<<<<< includes >>>>>>>>>> //
#include "UniformGrid.h"
#include "World.h"
#include "Particle.h"

//link fix try 4
extern World* world;

UniformGrid::UniformGrid(){
	
	cout << "grid: grid constr. called!" << endl; //zum test

	voxelSize = 0;
	indexGrid = 0;
	countGrid = 0;
	gridLength = 0;
	gridMinPos = 0;
	partPerVoxel = 0;

	gridSize = 0;
	xSteps = 0;
	ySteps = 0;
}

UniformGrid::~UniformGrid(){

	delete indexGrid;
	delete countGrid;
}

void UniformGrid::createGrid(){

	cout << "grid: create grid!" << endl; //zum test

	float worldS = world->getWorldSize();
	float partR = world->getPartRadius();

	voxelSize = 2.00f * partR;
	gridLength = (int)ceil(2.0f * worldS/ voxelSize);
	/* 2 extra grid voxels in each dimension in case particles go slightly outside the world borders */
	gridLength += 2;

	xSteps = gridLength*gridLength;
	ySteps = gridLength;

	std::cout << "Grid dimensions: " << gridLength << "*" << gridLength << "*" << gridLength << std::endl;

	gridMinPos = (-worldS) - voxelSize;
	//std::cout << "Grid minimum position: " << gridMinPos << std::endl;
	//std::cout << "Grid maximum position: " << gridMinPos + gridLength*voxelSize << std::endl;

	//std::cout << "Grid voxel size: " << voxelSize << std::endl;

	gridSize = gridLength * gridLength * gridLength;

	indexGrid = new int[gridSize*partPerVoxel];
	countGrid = new int[gridSize];

	for (int i=0; i<gridSize*partPerVoxel; i++) {
		indexGrid[i] = -1;
	}

	for (int i=0; i<gridSize; i++) {
		countGrid[i] = 0;
	}
}

void UniformGrid::updateGrid(){

	cout << "grid: update grid!" << endl; //zum test

	//partikel/body vektoren zu arrays �ndern?!
	int aPartN = world->getAllPartNum();
	Particle** allPart = world->getAllParticles();	//schauen ob pointer so richtig

	for (int i=0; i<gridSize*partPerVoxel; i++) {
		indexGrid[i] = -1;
	}
	for (int i=0; i<gridSize; i++) {
		countGrid[i] = 0;
	}

	for (int i=0; i<aPartN; i++) {

		allPart[i]->updateGridIndex();
		glm::vec3 gridIndex = allPart[i]->getGridIndex();

		if (isValidIndex(gridIndex) == true) {
			int flatCountGridIndex = (int)(gridIndex[0]*xSteps + gridIndex[1]*ySteps + gridIndex[2]);
			int flatIndexGridIndex = flatCountGridIndex * partPerVoxel;

			indexGrid[flatIndexGridIndex + countGrid[flatCountGridIndex]] = i;

			countGrid[flatCountGridIndex]++;
		}
	}
}

bool UniformGrid::isValidIndex(glm::vec3 gridIndex){

	cout << "grid: valid grid index?!" << endl; //zum test

	bool valid = true;

	if (gridIndex.x >= gridLength || gridIndex.x < 0) {
		valid= false;
	}
	if (gridIndex.y >= gridLength || gridIndex.y < 0) {
		valid= false;
	}
	if (gridIndex.z >= gridLength || gridIndex.z < 0) {
		valid= false;
	}
	return valid;
}

int* UniformGrid::getNeighborPartIndices(glm::vec3 gridIndex){

	cout << "grid: get neighbors!" << endl; //zum test

	//If you're at the border, go 1 cell inwards to prevent going out of bounds
	if (gridIndex.x == 0) {
		gridIndex.x++;
	} else if (gridIndex.x == (gridLength-1)) {
		gridIndex.x--;
	}
	if (gridIndex.y == 0) {
		gridIndex.y++;
	} else if (gridIndex.y == (gridLength-1)) {
		gridIndex.y--;
	}
	if (gridIndex.z == 0) {
		gridIndex.z++;
	} else if (gridIndex.z == (gridLength-1)) {
		gridIndex.z--;
	}

	glm::vec3 checkIndex = glm::vec3(gridIndex.x-1,gridIndex.y-1,gridIndex.z-1);

	int neighborCount = 0;
	int* indices = new int[27*partPerVoxel];

	for (int x=0; x<3; x++) {
		checkIndex.y = gridIndex.y - 1; //reset y index before y-loop

		for (int y=0; y<3; y++) {
			checkIndex.z = gridIndex.z-1; //reset z index before z-loop

			for (int z=0; z<3; z++) {
				int flatCountGridIndex = (int)checkIndex.x*xSteps + (int)checkIndex.y*ySteps + (int)checkIndex.z;
				int flatIndexGridIndex = flatCountGridIndex * partPerVoxel;

				indices[neighborCount] = indexGrid[flatIndexGridIndex];
				indices[neighborCount+1] = indexGrid[flatIndexGridIndex+1];
				indices[neighborCount+2] = indexGrid[flatIndexGridIndex+2];
				indices[neighborCount+3] = indexGrid[flatIndexGridIndex+3];

				neighborCount += 4;
				checkIndex.z++;
			}
			checkIndex.y++;
		}
		checkIndex.x++;
	}
	return indices;
}
