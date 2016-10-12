#include "pch.h"

#include <Kore/System.h>
#include <Kore/Math/Random.h>

#include "Particles.h"
#include "Rockets.h"
#include "Scheduler.h"

using namespace Kore;

namespace {
	const int width = 1024;
	const int height = 768;

	vec4 cameraStart;
	mat4 view;
	mat4 projection;
	
	void update() {
		float deltaT = updateScheduler();

		vec4 newCameraPos = /*mat4::RotationY(System::time() / 4) **/ cameraStart;
		view = mat4::lookAt(newCameraPos,
			vec3(0, 0, 0),
			vec3(0, 1, 0)
		);
		
		updateRockets(deltaT);
		updateParticleSystem(deltaT);

		Graphics::begin();
		Graphics::clear(Graphics::ClearColorFlag | Graphics::ClearDepthFlag, 0xFFFFBD00, 1.0f);

		renderRockets(view, projection);
		renderParticles(view, projection);

		Graphics::end();
		Graphics::swapBuffers();
	}
}

int kore(int argc, char** argv) {
	char* name = "Particles Example";

	Kore::System::setName(name);
	Kore::System::setup();
	Kore::WindowOptions options;
	options.title = name;
	options.width = width;
	options.height = height;
	options.x = 100;
	options.y = 100;
	options.targetDisplay = -1;
	options.mode = WindowModeWindow;
	options.rendererOptions.depthBufferBits = 16;
	options.rendererOptions.stencilBufferBits = 8;
	options.rendererOptions.textureFormat = 0;
	options.rendererOptions.antialiasing = 0;
	Kore::System::initWindow(options);

	cameraStart = vec4(0, 0, 25);
	//cameraStart = vec4(0, 50, 1);
	projection = mat4::Perspective(0.5f * pi, (float)width / height, 0.1f, 100.0f);

	initScheduler();
	initParticleSystem();
	initRockets();

	addSchedulerTask(Callback(&fireRocketRaw, 0, 0, 0, 10, 0, 0), 1);

	int c = 5;
	/*for (int x = -c; x <= c; ++x) {
		for (int z = -c; z <= c; ++z) {
			addParticleEmitter(vec3(x, 0, z), 1,
				vec3(0, 1, 0), 0,
				pi,
				1, 2,
				0, 0,
				0.1f, 0.2f,
				3, 5,
				0.25f, 0.5f,
				vec4(0.5f, 0, 0, 1), vec4(0.5f, 0.5f, 0, 1),
				vec4(0.5f, 0.5f, 0.5f, 0), vec4(0.5f, 0.5f, 0.5f, 0),
				vec2(0, 0));

		}
	}
	addParticleEmitter(vec3(5, 5, 0), 0,
		vec3(-1, 0, 0), 0,
		pi,
		3, 4,
		-1, -5,
		0.4f, 0.6f,
		3, 5,
		0.75f, 1,
		vec4(0.5f, 0.5f, 0.5f, 1), vec4(0.5f, 0.5f, 0.5f, 1),
		vec4(0.5f, 0.5f, 0.5f, 0), vec4(0.5f, 0.5f, 0.5f, 0),
		vec2(0, 1));
	addParticleEmitter(vec3(-5, 0, 0), 0,
		vec3(0, 1, 0), 0.25f * pi,
		pi,
		2, 2,
		0, 0,
		0.5f, 0.5f,
		3, 3,
		1, 1,
		vec4(0, 0, 0, 1), vec4(1, 1, 1, 1),
		vec4(0, 0, 0, 0), vec4(1, 1, 1, 0),
		vec2(1, 1));*/

	fireRocket(vec3(0, 0, 0), vec3(10, 0, 0));
	fireRocket(vec3(0, 0, 0), vec3(-10, 0, 0));
	fireRocket(vec3(0, 0, 0), vec3(0, 0, 10));
	fireRocket(vec3(0, 0, 0), vec3(0, 0, -10));
	float s = Kore::sqrt(50);
	fireRocket(vec3(0, 0, 0), vec3(-s, 0, -s));
	fireRocket(vec3(0, 0, 0), vec3(-s, 0, s));
	fireRocket(vec3(0, 0, 0), vec3(s, 0, -s));
	fireRocket(vec3(0, 0, 0), vec3(s, 0, s));
	//fireRocket(vec3(0, 0, 0), vec3(1, 0, 1));

	Kore::System::setCallback(update);

	Random::init(System::time() * 100);
	Kore::System::start();

	return 0;
}
