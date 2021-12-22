#include <random>
#include <entt/entt.hpp>
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/gl.h>

struct PositionComponent
{
	float x;
	float y;
};

struct VelocityComponent
{
	float x;
	float y;
};

struct DrawableComponent
{
	float r, g, b, a;
	float size;
};

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

int main(int argc, char** argv)
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		return -1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	uint32_t window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;
	SDL_Window* window = SDL_CreateWindow("entt sample", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, window_flags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_MakeCurrent(window, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

	std::random_device rnd;
	std::mt19937 gen(rnd());
	std::uniform_real_distribution<> dist(-100, 100);
	std::uniform_real_distribution<> colors(0, 1.0f);

	entt::registry registry;

	for (int i = 0; i < 10; i++)
	{
		auto entity = registry.create();

		registry.emplace<PositionComponent>(entity, (float)dist(gen) + WINDOW_WIDTH / 2, (float)dist(gen) + WINDOW_HEIGHT / 2);
		registry.emplace<VelocityComponent>(entity, (float)dist(gen), (float)dist(gen));
		registry.emplace<DrawableComponent>(entity, (float)colors(gen), (float)colors(gen), (float)colors(gen), 1.0f, (float)fabs(dist(gen)));
	}

	bool running = true;

	uint32_t frameTime = SDL_GetTicks();
	uint32_t frames = 0;
	uint32_t startTime = frameTime;

	while (running)
	{
		SDL_Event sdlEvent;

		while (SDL_PollEvent(&sdlEvent))
		{
			if (sdlEvent.type == SDL_QUIT ||
				(sdlEvent.type == SDL_WINDOWEVENT && sdlEvent.window.event == SDL_WINDOWEVENT_CLOSE && sdlEvent.window.windowID == SDL_GetWindowID(window)))
			{
				running = false;
			}
		}

		uint32_t newTime = SDL_GetTicks();

		uint32_t diff = (newTime - frameTime);

		frameTime = newTime;

		float delta = diff / 1000.0f;

		if (newTime - startTime >= 1000.0f)
		{
			startTime = newTime;

			char buffer[128];

			sprintf(buffer, "entt sample - %u FPS", frames);

			frames = 0;

			SDL_SetWindowTitle(window, buffer);
		}

		frames++;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);

		glPushMatrix();
		glLoadIdentity();

		glOrtho(0, WINDOW_WIDTH, WINDOW_HEIGHT, 0, -1, 1);

		glMatrixMode(GL_MODELVIEW);

		auto positionView = registry.view<PositionComponent, VelocityComponent>();
		auto drawableView = registry.view<PositionComponent, DrawableComponent>();

		for (auto entity : positionView)
		{
			auto& position = positionView.get<PositionComponent>(entity);
			auto& velocity = positionView.get<VelocityComponent>(entity);

			position.x += velocity.x * delta;
			position.y += velocity.y * delta;
		}

		for (auto entity : drawableView)
		{
			auto& position = drawableView.get<PositionComponent>(entity);
			auto& drawable = drawableView.get<DrawableComponent>(entity);

			glColor4f(drawable.r, drawable.g, drawable.b, drawable.a);

			glBegin(GL_QUADS);

			float size = drawable.size * 0.5f;

			glVertex3f(-size + position.x, -size + position.y, 0.0f);
			glVertex3f(-size + position.x, size + position.y, 0.0f);
			glVertex3f(size + position.x, size + position.y, 0.0f);
			glVertex3f(size + position.x, -size + position.y, 0.0f);

			glColor4f(1, 1, 1, 1);

			glEnd();

		}

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
