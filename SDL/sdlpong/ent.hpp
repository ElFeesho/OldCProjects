#ifndef __ENT__HPP__
#define __ENT__HPP__

#include <string>

class Engine;

class Ent
{
   public:
		Ent(const char *id, double x_vel, double y_vel, double x_pos, double y_pos, double w, double h);
		~Ent();
      
		//Entities can override update if they wish
		virtual bool update(int keys);
		//Each entity can override draw
      virtual void draw();
		
		//Setters
		void set_x(double x);
		void set_y(double y);
		void set_xv(double xv);
		void set_yv(double yv);
		void set_w(double w);
		void set_h(double w);
		void set_eng(Engine *eng);
		
		//Getters
		double get_x();
		double get_y();
		double get_xv();
		double get_yv();
		double get_w();
		double get_h();
		Engine *get_engine();
		long get_time();
		std::string get_name();
		
		//Generic Funcs
		void update_time();
   private:
		//Velocity
		double xvel;
		double yvel;
		
		//Position
		double xpos;
		double ypos;
		
		//Dimensions
		double width;
		double height;
		
		//Time keeping
		long current_tick;
		long last_tick;
		
		//Entity name - for various funcs (right now just collide function)
		std::string name;
		
		//Engine handle
		Engine *engine;
};

#endif
