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

	vec4 cameraStart;
	mat4 view;
	mat4 projection;
	
	void update() {
		float deltaT = updateScheduler();
		log(LogLevel::Info, "%i FPS", (int) round(1 / deltaT));

		vec4 newCameraPos = /*mat4::RotationY(System::time() / 4) **/ cameraStart;
		view = mat4::lookAt(newCameraPos,
			vec3(0, 0, 0),
			vec3(0, 1, 0)
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

	cameraStart = vec4(0, 2, 25); // cameraStart = vec4(0, 50, 1);
	projection = mat4::Perspective(0.5f * pi, (float)width / height, 0.1f, 100.0f);

	initScheduler();
	initGround();
	initParticleSystem();
	initRockets();

	Random::init(System::time() * 100);
	
	/*int c = 5;
	for (int x = -c; x <= c; ++x) {
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
	}*/

	/*addSchedulerTask(Task(1, &fireRocketRaw, 0, 0, 0, 10, 0, 0));
	addSchedulerTask(Task(1, &fireRocketRaw, 0, 0, 0, -10, 0, 0));
	addSchedulerTask(Task(1, &fireRocketRaw, 0, 0, 0, 0, 0, 10));
	addSchedulerTask(Task(1, &fireRocketRaw, 0, 0, 0, 0, 0, -10));
	float s = Kore::sqrt(50);
	addSchedulerTask(Task(1, &fireRocketRaw, 0, 0, 0, -s, 0, -s));
	addSchedulerTask(Task(1, &fireRocketRaw, 0, 0, 0, -s, 0, s));
	addSchedulerTask(Task(1, &fireRocketRaw, 0, 0, 0, s, 0, -s));
	addSchedulerTask(Task(1, &fireRocketRaw, 0, 0, 0, s, 0, s));*/
	int steps = 1;//13;
	for (int i = 1; i < 360; ++i) {
		vec3 pos = getRandomGroundPosition(((steps * i) % 360) * pi / 180.0f);
		vec3 pos2 = getRandomGroundPosition(((steps * i) % 360 + 200) * pi / 180.0f);
		addSchedulerTask(Task(0, &fireRocketRaw, pos.x(), 0, pos.z(), -pos.x(), 0, -pos.z()));
		//addSchedulerTask(Task(1 + 0 * i, &fireRocketRaw, pos.x(), 0, pos.z(), pos2.x(), 0, pos2.z()));
	}
	//vec3 pos = getRandomGroundPosition(0);
	//addSchedulerTask(Task(1, &fireRocketRaw, pos.x(), 0, pos.z(), -pos.x(), 0, -pos.z()));

	Kore::System::setCallback(update);
	Kore::System::start();

	return 0;
}
