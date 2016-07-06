
#include <three/textures/texture.hpp>

namespace three {

class GLRenderer;
class Color;
class PerspectiveCamera;
class ParticleSystem;
class Attribute;
class Scene;
class Texture;
class ParticleBasicMaterial;

class ExampleSession {
public:
	static void init(int width, int height);
	static void run();
	static void simple();
	static void particle3();
	static std::shared_ptr<GLRenderer> renderer;
    static Texture::Ptr generateDataTexture( int width, int height, Color color );
    static void step(double dt);
    static void billboard();

private:
	static std::shared_ptr<PerspectiveCamera> camera;
	static std::shared_ptr<ParticleSystem> object;
	static std::shared_ptr<Scene> scene;
	static std::shared_ptr<Texture> texture;
	static std::shared_ptr<ParticleBasicMaterial> material;
	// static std::shared_ptr<ShaderMaterial> shadermaterial;
	static size_t vc1;
};

}