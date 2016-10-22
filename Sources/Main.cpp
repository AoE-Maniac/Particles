#include "pch.h"

#include <Kore/Log.h>
#include <Kore/System.h>
#include <Kore/Math/Random.h>

#include "Ground.h"
#include "Particles.h"
#include "Rockets.h"
#include "Scheduler.h"

using namespace Kore;

namespace {
	const int width = 1024;
	const int height = 768;
	const float delay = 1;

	int pattern;
	vec4 cameraStart;
	mat4 view;
	mat4 projection;
	
	void update() {
		float deltaT = updateScheduler();
		log(LogLevel::Info, "%i FPS", (int) round(1 / deltaT));

		vec4 newCameraPos = /*mat4::RotationY(System::time() / 4) **/ cameraStart;
		view = mat4::lookAt(newCameraPos,
			vec3(0, 0, 0),
			vec3(0, 0, -1)
		);
		
		updateRockets(deltaT);
		updateParticleSystem(deltaT);

		Graphics::begin();
		Graphics::clear(Graphics::ClearColorFlag | Graphics::ClearDepthFlag, 0xFFFFBD00, 1.0f);

		renderGround(view, projection);
		renderRockets(view, projection);
		renderParticles(view, projection);

		Graphics::end();
		Graphics::swapBuffers();
	}

	void startNextPattern() {
		pattern = (pattern + 1) % 5;
		
		switch (pattern) {
		case 0: {
			// Longer series
			int steps = 43;
			for (int i = 0; i < 36; ++i) {
				vec3 pos = getCirclePosition(((steps * i) % 360) * pi / 180.0f);
				addSchedulerTask(Task(delay + 1.5f * i, &fireRocketRaw, pos.x(), 0, pos.z(), -pos.x(), 0, -pos.z()));
			}
			break;
		}
		case 1: {
			// Single rocket to show animation
			vec3 pos = getCirclePosition(0);
			addSchedulerTask(Task(delay, &fireRocketRaw, pos.x(), 0, pos.z(), -pos.x(), 0, -pos.z()));
			break;
		}
		case 2: {
			// "Realistic" flight paths
			for (int i = 0; i < 100; ++i) {
				vec3 pos = getRandomSidePosition(i % 2 == 0);
				vec3 pos2 = getRandomSidePosition(i % 2 == 1);
				addSchedulerTask(Task(delay + 0.1f * i, &fireRocketRaw, pos.x(), 0, pos.z(), pos2.x(), 0, pos2.z()));
			}
			break;
		}
		case 3: {
			// Expanding pattern
			for (int i = 0; i < 16; ++i) {
				vec3 pos = getCirclePosition(22.5f * i * pi / 180.0f);
				addSchedulerTask(Task(delay, &fireRocketRaw, 0, 0, 0, pos.x(), 0, pos.z()));
			}
			break;
		}
		case 4: {
			// Contracting pattern, max amount of rockets at the same time
			int steps = 1;
			for (int i = 0; i < 360; ++i) {
				vec3 pos = getCirclePosition(((steps * i) % 360) * pi / 180.0f);
				vec3 pos2 = getCirclePosition(((steps * i) % 360 + 200) * pi / 180.0f);
				addSchedulerTask(Task(delay, &fireRocketRaw, pos.x(), 0, pos.z(), pos2.x(), 0, pos2.z()));
			}
			break;
		}
		}
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

	cameraStart = vec4(0, 30, 25); // cameraStart = vec4(0, 2, 25); // cameraStart = vec4(0, 50, 1);
	projection = mat4::Perspective(0.5f * pi, (float)width / height, 0.1f, 100.0f);

	initScheduler();
	initGround();
	initParticleSystem();
	initRockets(&startNextPattern);

	Random::init(System::time() * 100);

	pattern = 0;
	startNextPattern();

	Kore::System::setCallback(update);
	Kore::System::start();

	return 0;
}
