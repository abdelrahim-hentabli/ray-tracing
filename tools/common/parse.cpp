#include "parse.hpp"

#include <map>
#include <sstream>
#include <string>

#include "lights.hpp"
#include "objects/mesh.hpp"
#include "objects/plane.hpp"
#include "objects/sphere.hpp"
#include "types.hpp"

void Parse(Render_World &world, int &width, int &height,
           const char *test_file) {
  FILE *F = fopen(test_file, "r");
  if (!F) {
    std::cout << "Failed to open file '" << test_file << "'\n";
    exit(EXIT_FAILURE);
  }

  double f0, ior, ci;
  char buff[1000];
  vec3 u, v, w;
  vec3 velocity, acceleration;
  std::string name, s0, s1, s2;

  std::map<std::string, vec3> colors;
  std::map<std::string, Object *> objects;
  std::map<std::string, shader_data> shaders;

  shader_data default_bg;
  default_bg.type = flat_shader;
  world.sd = default_bg;

  while (fgets(buff, sizeof(buff), F)) {
    std::stringstream ss(buff);
    std::string item, name;
    if (!(ss >> item) || !item.size() || item[0] == '#') continue;
    if (item == "size") {
      ss >> width >> height;
      assert(ss);
    } else if (item == "color") {
      ss >> name >> u;
      assert(ss);
      colors[name] = u;
    } else if (item == "plane") {
      ss >> name >> u >> v >> s0;
      assert(ss);
      Object *o = new Plane(u, v);
      std::map<std::string, shader_data>::const_iterator sh = shaders.find(s0);
      assert(sh != shaders.end());
      o->sd = sh->second;
      if (name == "-")
        world.objects.push_back(o);
      else
        objects[name] = o;
    } else if (item == "sphere") {
      ss >> name >> u >> f0 >> s0;
      assert(ss);
      Object *o = new Sphere(u, f0);
      std::map<std::string, shader_data>::const_iterator sh = shaders.find(s0);
      assert(sh != shaders.end());
      o->sd = sh->second;
      if (name == "-")
        world.objects.push_back(o);
      else
        objects[name] = o;
    } else if (item == "mesh") {
      ss >> name >> s0 >> s1;
      assert(ss);
      Mesh *o = new Mesh;
      o->Read_Obj(s0.c_str());
      std::map<std::string, shader_data>::const_iterator sh = shaders.find(s1);
      assert(sh != shaders.end());
      o->sd = sh->second;
      if (name == "-")
        world.objects.push_back(o);
      else
        objects[name] = o;
    } else if (item == "flat_shader") {
      ss >> name >> s0;
      assert(ss);
      std::map<std::string, vec3>::const_iterator c0 = colors.find(s0);
      assert(c0 != colors.end());
      shader_data temp;
      temp.type = flat_shader;
      temp.color_ambient = c0->second;
      shaders[name] = temp;
    } else if (item == "phong_shader") {
      ss >> name >> s0 >> s1 >> s2 >> f0;
      assert(ss);
      std::map<std::string, vec3>::const_iterator c0 = colors.find(s0);
      std::map<std::string, vec3>::const_iterator c1 = colors.find(s1);
      std::map<std::string, vec3>::const_iterator c2 = colors.find(s2);
      assert(c0 != colors.end());
      assert(c1 != colors.end());
      assert(c2 != colors.end());
      shader_data temp;
      temp.type = phong_shader;
      temp.color_ambient = c0->second;
      temp.color_diffuse = c1->second;
      temp.color_specular = c2->second;
      temp.specular_power = f0;
      shaders[name] = temp;
    } else if (item == "reflective_shader") {
      ss >> name >> s0 >> f0;
      assert(ss);
      std::map<std::string, shader_data>::const_iterator sh = shaders.find(s0);
      assert(sh != shaders.end());
      shader_data temp;
      switch (sh->second.type) {
        case (flat_shader):
          temp.type = reflective_flat;
          temp.color_ambient = sh->second.color_ambient;
          break;
        case (phong_shader):
          temp.type = reflective_phong;
          temp.color_ambient = sh->second.color_ambient;
          temp.color_diffuse = sh->second.color_diffuse;
          temp.color_specular = sh->second.color_specular;
          temp.specular_power = sh->second.specular_power;
          break;
        default:
          temp.type = reflective_flat;
          temp.color_ambient = sh->second.color_ambient;
          break;
      }
      temp.color_intensity = (1 - f0);
      shaders[name] = temp;
    } else if (item == "refractive_shader") {
      ss >> name >> s0 >> ior >> ci;
      assert(ss);
      std::map<std::string, shader_data>::const_iterator sh = shaders.find(s0);
      assert(sh != shaders.end());
      shader_data temp;
      switch (sh->second.type) {
        case (flat_shader):
          temp.type = refractive_flat;
          temp.color_ambient = sh->second.color_ambient;
          break;
        case (phong_shader):
          temp.type = refractive_phong;
          temp.color_ambient = sh->second.color_ambient;
          temp.color_diffuse = sh->second.color_diffuse;
          temp.color_specular = sh->second.color_specular;
          temp.specular_power = sh->second.specular_power;
          break;
        default:
          temp.type = refractive_flat;
          temp.color_ambient = sh->second.color_ambient;
          break;
      }
      temp.incidence_of_refraction = ior;
      temp.color_intensity = ci;
      shaders[name] = temp;
    } else if (item == "point_light") {
      ss >> u >> s0 >> f0;
      assert(ss);
      std::map<std::string, vec3>::const_iterator c0 = colors.find(s0);
      assert(c0 != colors.end());
      light_data temp;
      temp.type = point_light;
      temp.position = u;
      temp.color = c0->second;
      temp.brightness = f0;
      world.lights.push_back(temp);
    } else if (item == "direction_light") {
      ss >> u >> s0 >> f0;
      assert(ss);
      std::map<std::string, vec3>::const_iterator c0 = colors.find(s0);
      assert(c0 != colors.end());
      light_data temp;
      temp.type = direction_light;
      temp.position = u;
      temp.color = c0->second;
      temp.brightness = f0;
      world.lights.push_back(temp);
    } else if (item == "spot_light") {
      double max_angle, exponent;
      vec3 direction;
      ss >> u >> s0 >> f0 >> max_angle >> exponent >> direction;
      assert(ss);
      std::map<std::string, vec3>::const_iterator c0 = colors.find(s0);
      assert(c0 != colors.end());
      light_data temp;
      temp.type = spot_light;
      temp.position = u;
      temp.color = c0->second;
      temp.brightness = f0;
      temp.min_cos_angle = max_angle;
      temp.falloff_exponent = exponent;
      temp.direction = direction;
      world.lights.push_back(temp);
    } else if (item == "ambient_light") {
      ss >> s0 >> f0;
      assert(ss);
      std::map<std::string, vec3>::const_iterator c0 = colors.find(s0);
      assert(c0 != colors.end());
      world.ambient_color = c0->second;
      world.ambient_intensity = f0;
    } else if (item == "camera") {
      ss >> u >> v >> w >> f0 >> velocity >> acceleration;
      assert(ss);
      world.camera.Position_And_Aim_Camera(u, v, w);
      world.camera.Focus_Camera(1, (double)width / height, f0 * (pi / 180));
      world.camera.velocity = velocity;
      world.camera.acceleration = acceleration;
    } else if (item == "background") {
      ss >> s0;
      assert(ss);
      std::map<std::string, shader_data>::const_iterator sh = shaders.find(s0);
      assert(sh != shaders.end());
      world.sd = sh->second;
    } else if (item == "enable_shadows") {
      ss >> world.enable_shadows;
      assert(ss);
    } else if (item == "recursion_depth_limit") {
      ss >> world.recursion_depth_limit;
      assert(ss);
    } else {
      std::cout << "Failed to parse: " << buff << std::endl;
      exit(EXIT_FAILURE);
    }
  }
  world.camera.Set_Resolution(ivec2(width, height));
}
