#include "camera.hpp"

Camera::Camera() : colors(0) {}

Camera::~Camera() {
  if (colors) {
    delete[] colors;
  }
}

Camera::Camera(const Camera &other) {
  position = other.position;
  film_position = other.film_position;
  look_vector = other.look_vector;
  vertical_vector = other.vertical_vector;
  horizontal_vector = other.horizontal_vector;

  min = other.min;
  max = other.max;

  image_size = other.image_size;
  pixel_size = other.pixel_size;
  number_pixels = other.number_pixels;

  colors = new Pixel[number_pixels[0] * number_pixels[1]];
}

Camera &Camera::operator=(const Camera &other) {
  position = other.position;
  film_position = other.film_position;
  look_vector = other.look_vector;
  vertical_vector = other.vertical_vector;
  horizontal_vector = other.horizontal_vector;

  min = other.min;
  max = other.max;

  image_size = other.image_size;
  pixel_size = other.pixel_size;
  number_pixels = other.number_pixels;

  if (colors) {
    delete[] colors;
  }
  colors = new Pixel[number_pixels[0] * number_pixels[1]];

  return *this;
}

void Camera::Position_And_Aim_Camera(const vec3 &position_input,
                                     const vec3 &look_at_point,
                                     const vec3 &pseudo_up_vector) {
  position = position_input;
  look_vector = (look_at_point - position).normalized();
  horizontal_vector = cross(look_vector, pseudo_up_vector).normalized();
  vertical_vector = cross(horizontal_vector, look_vector).normalized();
}

void Camera::Focus_Camera(double focal_distance, double aspect_ratio,
                          double field_of_view) {
  film_position = position + look_vector * focal_distance;
  double width = 2.0 * focal_distance * tan(.5 * field_of_view);
  double height = width / aspect_ratio;
  image_size = vec2(width, height);
}

void Camera::Set_Resolution(const ivec2 &number_pixels_input) {
  number_pixels = number_pixels_input;
  if (colors) delete[] colors;
  colors = new Pixel[number_pixels[0] * number_pixels[1]];
  min = -0.5 * image_size;
  max = 0.5 * image_size;
  pixel_size = image_size / vec2(number_pixels);
}

// Find the world position of the input pixel
vec3 Camera::World_Position(const ivec2 &pixel_index) {
  vec2 cellCenterScreenSpace = Cell_Center(pixel_index);
  vec3 result =
      film_position + ((cellCenterScreenSpace[0] * horizontal_vector) +
                       cellCenterScreenSpace[1] * vertical_vector);
  return result;
}
