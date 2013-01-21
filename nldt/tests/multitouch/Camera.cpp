#include "Camera.h"

Camera::~Camera()
{
}

void Camera::push_error(string error)
{
  if(error != "")
  {
    errors.push_back(error);
  }
}

string Camera::pop_error()
{
  string error = "";
  while(error.size() > 0)
  {
    error = errors.front();
    errors.pop_front();
  }
  return error;
}

