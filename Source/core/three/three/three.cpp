#include <three/core/geometry.hpp>
#include <three/cameras/perspective_camera.hpp>
#include <three/extras/image_utils.hpp>
#include <three/objects/mesh.hpp>
#include <three/materials/shader_material.hpp>
#include <three/materials/mesh_face_material.hpp>
#include <three/objects/particle_system.hpp>
#include <three/renderers/renderer_parameters.hpp>
#include <three/renderers/gl_renderer.hpp>

#include <three/extras/geometries/cube_geometry.hpp>
#include <three/extras/geometries/sphere_geometry.hpp>
#include <three/extras/geometry_utils.hpp>

#include <three/materials/particle_basic_material.hpp>
#include <three/scenes/fog_exp2.hpp>
#include <three/three.h>

namespace three {

std::shared_ptr<PerspectiveCamera> ExampleSession::camera = 0;
std::shared_ptr<ParticleSystem> ExampleSession::object = 0;
std::shared_ptr<Scene> ExampleSession::scene = 0;
size_t ExampleSession::vc1 = 0;
std::shared_ptr<Texture> ExampleSession::texture = 0;
std::shared_ptr<ParticleBasicMaterial> ExampleSession::material = 0;

std::shared_ptr<GLRenderer> ExampleSession::renderer = 0;
void ExampleSession::init(int width, int height) {
    renderer = three::GLRenderer::create( three::RendererParameters() );
    renderer->setSize(width, height);
}

Texture::Ptr ExampleSession::generateDataTexture( int width, int height, Color color ) {

  const auto size = width * height;
  if ( size <= 0 ) {
    console().error("three::ImageUtils::generateDataTexture: Texture must have positive dimensions");
    return Texture::Ptr();
  }

  std::vector<unsigned char> data( 3 * size );

  const auto r = (unsigned char)Math::floor( color.r * 255 );
  const auto g = (unsigned char)Math::floor( color.g * 255 );
  const auto b = (unsigned char)Math::floor( color.b * 255 );

  for ( int i = 0; i < size; i ++ ) {
    data[ i * 3 ]     = r;
    data[ i * 3 + 1 ] = g;
    data[ i * 3 + 2 ] = b;
  }

  auto texture = Texture::create(
    TextureDesc( Image(data, width, height), THREE::RGBFormat )
  );
  texture->needsUpdate = true;

  return texture;

}


void ExampleSession::simple() {
  // // Camera
  // auto camera = PerspectiveCamera::create(
  //   50, ( float )renderer->width() / renderer->height(), .1f, 1000.f
  // );
  // camera->position.z = 300;


  // // Scene
  // auto scene = Scene::create();
  // scene->add( camera );


  // // Lights
  // auto pointLight = PointLight::create( 0xFFFFFF );
  // pointLight->position = Vector3( 10, 50, 130 );
  // scene->add( pointLight );


  // // Materials
  // auto sphereMaterial = MeshLambertMaterial::create(
  //   Material::Parameters().add( "color", Color( 0xcc0000 ) )
  // );


  // // Geometries
  // float radius = 50, segments = 16, rings = 16;
  // auto sphereGeometry = SphereGeometry::create( radius, segments, rings );

  // auto sphere = Mesh::create( sphereGeometry, sphereMaterial );
  // scene->add( sphere );

  //   camera->lookAt( scene->position );

  //   renderer->render( *scene, *camera );
}

void ExampleSession::particle3() {
const std::string vertexShader =
"attribute float size;\n"
"attribute vec4 ca;\n"
"varying vec4 vColor;\n"
"void main() {\n"
"  vColor = ca;\n"
"  vec4 mvPosition = modelViewMatrix * vec4( position, 1.0 );\n"
"  gl_PointSize = size * ( 150.0 / length( mvPosition.xyz ) );\n"
"  gl_Position = projectionMatrix * mvPosition;\n"
"}\n";

const std::string fragmentShader =
"uniform vec3 color;\n"
"uniform sampler2D texture;\n"
"varying vec4 vColor;\n"
"void main() {\n"
"  vec4 outColor = texture2D( texture, gl_PointCoord );\n"
"  if ( outColor.a < 0.5 ) discard;\n"
"  gl_FragColor = outColor * vec4( color * vColor.xyz, 1.0 );\n"
"  float depth = gl_FragCoord.z / gl_FragCoord.w;\n"
"  const vec3 fogColor = vec3( 0.0 );\n"
"  float fogFactor = smoothstep( 200.0, 600.0, depth );\n"
"  gl_FragColor = mix( gl_FragColor, vec4( fogColor, gl_FragColor.w ), fogFactor );\n"
"}\n";

  camera = PerspectiveCamera::create(
    40, ( float )renderer->width() / renderer->height(), 1, 1000
  );
  camera->position.z = 500;

  scene = Scene::create();

  texture = generateDataTexture(1, 1, Color(0xffffff));
  texture->wrapS = texture->wrapT = THREE::RepeatWrapping;

  Attributes attributes;
  attributes[ "size" ] = Attribute( THREE::f );
  attributes[ "ca" ]   = Attribute( THREE::c );

  Uniforms uniforms;
  uniforms[ "color" ]      = Uniform( THREE::c, Color( 0xffffff ) );
  uniforms[ "texture" ]    = Uniform( THREE::t, texture.get() );

  auto shaderMaterial = ShaderMaterial::create(
    vertexShader,
    fragmentShader,
    uniforms,
    attributes
  );

  // Geometries
  auto geometry = Geometry::create();

  geometry->vertices.reserve( 100000 );

  const auto radius = 100.f, inner = 0.6f * radius;
  for ( auto i = 0; i < 100000; i ++ ) {

    Vector3 vertex( Math::random( -1.f, 1.f ),
                    Math::random( -1.f, 1.f ),
                    Math::random( -1.f, 1.f ) );
    vertex.multiplyScalar( radius );

    if ( ( vertex.x > inner || vertex.x < -inner ) ||
         ( vertex.y > inner || vertex.y < -inner ) ||
         ( vertex.z > inner || vertex.z < -inner )  )

      geometry->vertices.push_back( vertex );

  }
  vc1 = geometry->vertices.size();

  // geometry 2

  auto dummyMaterial = MeshFaceMaterial::create();

  const auto radius2 = 200.f;
  auto geometry2 = CubeGeometry::create( radius2, 0.1f * radius2, 0.1f * radius2, 50, 5, 5 );

  auto addGeo = [&]( const Geometry::Ptr& geo, float x, float y, float z, float ry ) {
    auto m = Mesh::create( geo, dummyMaterial );
    m->position.set( x, y, z );
    m->rotation.y = ry;

    GeometryUtils::merge( *geometry, *m );
  };

  // side 1

  addGeo( geometry2, 0,  110,  110, 0 );
  addGeo( geometry2, 0,  110, -110, 0 );
  addGeo( geometry2, 0, -110,  110, 0 );
  addGeo( geometry2, 0, -110, -110, 0 );

  // side 2

  addGeo( geometry2,  110,  110, 0, Math::PI()/2 );
  addGeo( geometry2,  110, -110, 0, Math::PI()/2 );
  addGeo( geometry2, -110,  110, 0, Math::PI()/2 );
  addGeo( geometry2, -110, -110, 0, Math::PI()/2 );

  // corner edges

  auto geometry3 = CubeGeometry::create( 0.1f * radius2, radius2 * 1.2f, 0.1f * radius2, 5, 60, 5 );

  addGeo( geometry3,  110, 0,  110, 0 );
  addGeo( geometry3,  110, 0, -110, 0 );
  addGeo( geometry3, -110, 0,  110, 0 );
  addGeo( geometry3, -110, 0, -110, 0 );

  // particle system

  object = ParticleSystem::create( geometry, shaderMaterial );
  object->geometry->dynamic = true;

  const auto& vertices = object->geometry->vertices;
  const auto vertexCount = vertices.size();

  std::vector<float> valuesSize( vertexCount );
  std::vector<Color> valuesColor( vertexCount );

  for ( size_t v = 0; v < vertexCount; v++ ) {

    valuesColor[ v ] = Color( 0xffffff );

    if ( v < vc1 ) {
      valuesSize[ v ] = 10;
      valuesColor[ v ].setHSV( 0.5f  + 0.2f * ( (float)v / vc1 ),
                               0.99f,
                               1.f );
    } else {
      valuesSize[ v ] = 55;
      valuesColor[ v ].setHSV( 0.1f,
                               0.99f,
                               1.f );
    }

  }

  auto& size  = shaderMaterial->attributes[ "size" ];
  auto& color = shaderMaterial->attributes[ "ca" ];

  size.value  = std::move(valuesSize);
  color.value = std::move(valuesColor);

  scene->add( object );

  /////////////////////////////////////////////////////////////////////////

  // auto running = true, renderStats = true;
  // sdl::addEventListener( SDL_KEYDOWN, [&]( const sdl::Event& e ) {
  //   switch (e.key.keysym.sym) {
  //   case SDLK_q:
  //   case SDLK_ESCAPE:
  //     running = false; break;
  //   default:
  //     renderStats = !renderStats; break;
  //   };
  // } );

  // sdl::addEventListener( SDL_QUIT, [&]( const sdl::Event& ) {
  //   running = false;
  // } );

  // sdl::addEventListener( SDL_VIDEORESIZE, [&]( const sdl::Event event ) {
  //   camera->aspect = ( float )event.resize.w / event.resize.h;
  //   camera->updateProjectionMatrix();
  //   renderer->setSize( event.resize.w, event.resize.h );
  // } );

  /////////////////////////////////////////////////////////////////////////

  // stats::Stats stats( *renderer );
}

void ExampleSession::billboard() {
  camera = PerspectiveCamera::create(
    55, ( float )renderer->width() / renderer->height(), 2.f, 2000
  );
  camera->position.z = 1000;

  scene = Scene::create();
  scene->fog = FogExp2::create( 0x000000, .001f );

  auto geometry = Geometry::create();

  const auto particleCount = 10000;
  geometry->vertices.reserve( particleCount );

  std::generate_n( std::back_inserter( geometry->vertices ),
                   particleCount,
                   [] { return Vector3( Math::random(-1000.f, 1000.f),
                                        Math::random(-1000.f, 1000.f),
                                        Math::random(-1000.f, 1000.f) ); } );

  texture = generateDataTexture(1, 1, Color(0xffffff));

  material = ParticleBasicMaterial::create(
    Material::Parameters().add( "size", 35.f )
                          .add( "map", texture )
                          .add( "sizeAttenuation", false )
  );
  material->color.setHSV( 1.f, 0.2f, 0.8f );

  auto particles = ParticleSystem::create( geometry, material );
  particles->sortParticles = true;
  scene->add( particles );

}

void ExampleSession::run() {
    // particle3();
  billboard();
}

// Partical 3
// void ExampleSession::step(double dt) {
//     object->rotation.y = object->rotation.z = dt;

//     // auto& sizes = size.value.cast<std::vector<float>>();
//     // for( size_t i = 0; i < sizes.size(); i++ ) {
//     //   if ( i < vc1 )
//     //     sizes[ i ] = 26.f + 32.f * Math::sin( 0.1f * i + time );
//     // }
//     // size.needsUpdate = true;

//     renderer->render( *scene, *camera );
// }

void ExampleSession::step(double dt) {\
    const auto h = Math::fmod( 360.f * ( 1.f + (float)dt ), 360.f ) / 360.f;
    material->color.setHSV( h, 0.75f, 0.8f );
    renderer->render( *scene, *camera );
}
}
