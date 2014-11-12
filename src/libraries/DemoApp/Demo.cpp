
//!noch gl fehler!

// <<<<<<<<<< includes >>>>>>>>>> //
#include "Demo.h"
#include "Scene.h"
#include "PhysicEngine/UniformGrid.h"
#include "PhysicEngine/Cuda.h"

using namespace std;

Demo::Demo(int wwIN, int whIN, float durIN, float tvIN, float wsIN, float prIN, float scIN, float dcIN){

	cout << "demo: demo constr called!" << endl; //zum test

	physicsWorld = new World(wsIN,prIN,scIN,dcIN);
	virtObjNum = 0;
	time = new Timing();
	windowWidth = wwIN;
	windowHeight = whIN;
	duration = durIN;
	terminalVeloc = tvIN;
	camera = new CVK::Trackball(wwIN,whIN);
}

Demo::~Demo(){

	//...
}

void Demo::run(){

	cout << "demo: run!" << endl; //zum test

	// Init GLFW and GLEW
	glfwInit();
	CVK::useOpenGL33CoreProfile();
	GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "RBPE-Demo", 0, 0);
	glfwSetWindowPos( window, 100, 50);
	glfwMakeContextCurrent(window);

	glewInit();

	//vbos generieren und binden. eins f�r gesamtes szene oder pro box eins??
	glGenBuffers(1, &rbVBO);
	glBindBuffer(GL_ARRAY_BUFFER, rbVBO);
	int bufferSize = vertexCount * 3 * sizeof(float);
	glBufferData(GL_ARRAY_BUFFER, 3 * bufferSize, vertexData, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, numParticles * 3 * sizeof(float), 0, GL_DYNAMIC_DRAW); // locate the memory, but without initialize the values  

	camera->setCenter( glm::vec3( 0.0f, 0.0f, 0.0f));
	camera->setRadius( 5);
	camera->setNearFar( 1.0f, 10.0f);

	glfwSetWindowSizeCallback( window, resizeCallback);

	initScene();

	//load, compile and link Shader
	const char *shadernames[2] = {SHADERS_PATH "/Examples/Phong.vert", SHADERS_PATH "/Examples/Phong.frag"};
	CVK::ShaderPhong phongShader( VERTEX_SHADER_BIT|FRAGMENT_SHADER_BIT, shadernames);

	//OpenGL parameters
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	CVK::State::getInstance()->setCamera( camera);

	//define light
	CVK::Light plight( glm::vec4( -1, 1, 1, 1), grey, glm::vec3( 0, 0, 0), 1.0f, 0.0f);
	CVK::State::getInstance()->addLight( &plight);

	//define Scene uniforms (ambient and fog)
	CVK::State::getInstance()->updateSceneSettings( darkgrey, 0, white, 1, 10, 1);
	CVK::State::getInstance()->setShader(&phongShader);

	//...

	//init cuda
	if(isGPU == true){
		Cuda::getInstance()->initCUDA();
	}

	//schauen wie am besten machen mit virtobjs und step simulation
	while( !glfwWindowShouldClose(window)){

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		time->startFrame();
		//display();	//�berfl�ssig
		stepSimulation(duration);

		//Update Camera
		camera->update(window);

		//update shader and render
		phongShader.update();

		//unterschiedlich bei cpu o. gpu ausf�hrung
		//vorher positionen und orientierung von VOs aktualisieren
		//obj rendern
		//earthNode->render();

		glfwSwapBuffers(window);
		glfwPollEvents();

		time->endFrame();

		float fps = (float)time->updateFPS();
		char title[64];
		sprintf(title, "Rigid Body | %d fps", (int)fps);
		glfwSetWindowTitle(window, title);
	}
	Cuda::getInstance()->~Cuda();	//free cuda stuff

	glfwDestroyWindow( window);
	glfwTerminate();
}

void Demo::initScene(){

	cout << "demo: initScene called!" << endl; //zum test

	//gpu benutzt andere create grid
	if(isGPU == false){
		UniformGrid::getInstance()->createGrid();
	}

	//obj initialisieren
	initObjs();
}

//wenn nur step simulation drin bleibt, dann ja eig �berfl�ssig
/*void Demo::display(){

	stepSimulation(timeDelta);

}*/

void Demo::stepSimulation(float duration){

	cout << "demo: step simulation!" << endl; //zum test

	World::getInstance()->stepPhysics(duration,isGPU);

	/*
	//update part. values
	for(std::vector<VirtualObject*>::iterator it = virtualObjs.begin(); it != virtualObjs.end(); ++it){
		(*it)->updatePartValuesVO();
	}

	//update grid
	updateGrid();

	//update momenta
	for(std::vector<VirtualObject*>::iterator it = virtualObjs.begin(); it != virtualObjs.end(); ++it){
		(*it)->updateMomentaVO(duration);
	}

	//iterate
	for(std::vector<VirtualObject*>::iterator it = virtualObjs.begin(); it != virtualObjs.end(); ++it){
		(*it)->iterateVO(duration);
	}
	*/
}

void Demo::resetScene(){

	//...
}
