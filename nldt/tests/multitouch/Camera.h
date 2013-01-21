#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <string>
#include <list>
using namespace std;

class Camera
{
public:
  virtual ~Camera();
  virtual int *grab_image() = 0;
  virtual int get_width() = 0;
  virtual int get_height() = 0;
  void push_error(string error);
  string pop_error();
private:
  list <string> errors;
};

#endif

