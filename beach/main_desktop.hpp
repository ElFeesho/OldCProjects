#ifndef __MAIN_DESKTOP_HPP__
class MainDesktop
{
public:
	MainDesktop();
	~MainDesktop();

	void flush();
private:
	long cx, cy;
	int screenw;
	int screenh;
	unsigned long ltick;
	bool dragging;
};

#endif
