import math

class TestObj:
	x = 0.0
	y = 0.0
	
	cx = 0.0
	cy = 0.0
	rot = 0.0
	
	xspeed = 0.0
	yspeed = 0.0
	width = 0.0
	height = 0.0
	dstx = 0.0
	dsty = 0.0
	def __init__(self, x, y, w, h):
		self.x = 0
		self.y = 0
		self.width = w
		self.height = h
		self.xspeed = 2.0
		self.yspeed = 2.0
		self.dstx = 250.0
		self.dsty = 200.0
		self.cx = x
		self.cy = y
		self.rot = 0

	def on_update(self):
		if self.cx < self.dstx:
			self.cx += (self.dstx - self.cx)*0.2
		else:
			self.cx -= (self.cx - self.dstx)*0.2

		if self.cy < self.dsty:
			self.cy += (self.dsty - self.cy)*0.2
		else:
			self.cy -= (self.cy - self.dsty)*0.2
		if self.cx > self.dstx-0.1 and self.cx < self.dstx+0.1:
			self.cx = self.dstx
		if self.cy > self.dsty-0.1 and self.cy < self.dsty+0.1:
			self.cy = self.dsty
		
		self.x = self.cx + (50*math.cos(self.rot))
		self.y = self.cy + (40*math.sin(self.rot))
		self.rot += .1

	def on_click(self, x, y):
		self.dstx = x-self.width/2.0
		self.dsty = y-self.width/2.0
