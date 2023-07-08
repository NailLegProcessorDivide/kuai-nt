#include "kuai.h"

#include <random>
#include <glm/gtx/string_cast.hpp>

using namespace kuai;

struct ProfileResult
{
	const char* name;
	float time;
};

std::vector<ProfileResult> profileResults;

class BaseLayer : public Layer
{
public:
	Rc<Entity> myEntity;
	Rc<Entity> pointLight;

	float lastX = (float)App::get().getWindow().getWidth() / 2.0;
	float lastY = (float)App::get().getWindow().getHeight() / 2.0;
	bool firstMouse = true;

	float speed = 10.0f;
	float sensitivity = 0.1f;

	float counter = 0;
	float x = 0.0f;

	Rc<Model> cubeModel = makeRc<Model>("C:/Users/David/Documents/cube.obj");

	std::vector<Rc<Entity>> entities;

	BaseLayer() : Layer("Base"), scene(std::make_unique<Scene>())
	{
		auto whiteTex = makeRc<Texture>();

		std::vector<std::string> faces =
		{
			"C:/Users/David/Pictures/skybox/right.jpg",
			"C:/Users/David/Pictures/skybox/left.jpg",
			"C:/Users/David/Pictures/skybox/top.jpg",
			"C:/Users/David/Pictures/skybox/bottom.jpg",
			"C:/Users/David/Pictures/skybox/front.jpg",
			"C:/Users/David/Pictures/skybox/back.jpg"
		};

		auto& cubemap = makeRc<Cubemap>(faces);
		auto& skybox = makeRc<Skybox>(cubemap);

		auto& skyboxEntity = scene->createEntity();
		skyboxEntity->addComponent<MeshRenderer>(skybox);

		auto& grassTex = makeRc<Texture>("C:/Users/David/Pictures/grass.png");
		auto& grassMat = makeRc<DefaultMaterial>(grassTex, grassTex, 10.0f);

		auto& plane = makeRc<Model>("C:/Users/David/Documents/plane.obj");

		plane->getMaterials()[0] = grassMat;

		auto& planeEntity = scene->createEntity();
		planeEntity->addComponent<MeshRenderer>(plane);
		planeEntity->getTransform().setPos(0, -1, -2);
		planeEntity->getTransform().setScale(1, 0.5f, 1);

		auto& model = makeRc<Model>("C:/Users/David/Documents/bunny.obj");
		auto& modelMat = makeRc<DefaultMaterial>(whiteTex, whiteTex, 30.0f);
		//modelMat->setReflection(cubemap);
		model->getMaterials()[0] = modelMat;
		
		myEntity = scene->createEntity();
		myEntity->getTransform().translate(0, -1, -2);

		myEntity->addComponent<MeshRenderer>(model);

		pointLight = scene->createEntity();
		pointLight->addComponent<Light>().setIntensity(0.5f);
		pointLight->getTransform().setPos(-1, 4, -2);

		auto& audio = makeRc<AudioClip>("C:/Users/David/Music/jigsaw.wav");

		myEntity->addComponent<AudioSourceComponent>(false);
		auto& a = myEntity->getComponent<AudioSourceComponent>();

		a.get()->setAudioClip(audio);
		a.get()->setPitch(1.1f);
		a.get()->play();

		entities.resize(10000);
		for (auto& entity : entities)
		{
			entity = scene->createEntity();

			entity->getTransform().setPos(-20 + rand() % 33, - 30 +rand() % 55, -30 + rand() % 64);
			entity->getTransform().setScale(glm::vec3(0.3f, 0.3f, 0.3f));

			entity->addComponent<MeshRenderer>(cubeModel);
		}
	}

	void update(float dt) override
	{
		scene->update(dt);

		float velocity = speed * dt;

		if (Input::isKeyPressed(Key::A))
		{
			scene->getMainCam().getTransform().translate(
				-scene->getMainCam().getTransform().getRight() * velocity);
		}
		if (Input::isKeyPressed(Key::D))
		{
			scene->getMainCam().getTransform().translate(
				scene->getMainCam().getTransform().getRight() * velocity);
		}
		if (Input::isKeyPressed(Key::W))
		{
			scene->getMainCam().getTransform().translate(
				scene->getMainCam().getTransform().getForward() * velocity);
		}
		if (Input::isKeyPressed(Key::S))
		{
			scene->getMainCam().getTransform().translate(
				-scene->getMainCam().getTransform().getForward() * velocity);
		}
		if (Input::isKeyPressed(Key::Space))
		{
			scene->getMainCam().getTransform().translate(0, velocity, 0);
		}
		if (Input::isKeyPressed(Key::LeftShift))
		{
			scene->getMainCam().getTransform().translate(0, -velocity, 0);
		}

		counter += dt;
		
		myEntity->getTransform().translate(0, 0, -0.05f);
	}

	void onEvent(Event* event) override
	{
		if (event->getEventType() == EventType::MouseMove)
		{
			MouseMoveEvent* e = (MouseMoveEvent*)event;
			float xpos = static_cast<float>(e->getX());
			float ypos = static_cast<float>(e->getY());
			if (firstMouse)
			{
				lastX = xpos;
				lastY = ypos;
				firstMouse = false;
			}

			float xoffset = xpos - lastX;
			float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top

			lastX = xpos;
			lastY = ypos;

			xoffset *= sensitivity; // yaw
			yoffset *= sensitivity; // pitch

			Transform t = scene->getMainCam().getTransform();
			scene->getMainCam().getTransform().rotate(yoffset, -xoffset, 0);

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (t.getRot().x > 89.0f) 
				t.setRot(89.0f, t.getRot().y, t.getRot().z);
			if (t.getRot().x < -89.0f)
				t.setRot(-89.0f, t.getRot().y, t.getRot().z);
		}
	}

private:
	Box<Scene> scene;
};

int main()
{
	Log::Init();

	App* app = new App();
	App::get().getWindow().setVSync(false);
	app->pushLayer(new BaseLayer());

	app->run();

	delete app;

	return 0;
}
