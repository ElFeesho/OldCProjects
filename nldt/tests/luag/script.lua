
x = 0.0
y = 0.0
w = 0.0
h = 0.0
img_x = 0.0
img_y = 0.0
img_w = 0.0
img_h = 0.0
xspeed = 0.0
yspeed = 0.0
draw=0.0
function initialise(nx,ny,nw,nh)
	x = nx
	y = ny
	w = nw
	h = nh
end

function on_update()
	if (draw==1) then
		load_image("test.png",img_x,img_y,img_w, img_h)	
	end
	x = x+xspeed
	y = y+yspeed
	if(xspeed>0) then 
		xspeed=xspeed-0.1
	elseif(xspeed<0) then
		xspeed=xspeed+0.1
	end
	if(yspeed>0) then 
		yspeed=yspeed-0.1
	elseif(yspeed<0) then
		yspeed=yspeed+0.1
	end
end

function on_key_press(key)
	if(key==32 and xspeed < 4.0) then
		xspeed = xspeed+0.2
	end
	if(key==31 and xspeed > -4.0) then
		xspeed = xspeed-0.2
	end
	if(key==33 and yspeed > -4.0) then
		yspeed = yspeed-0.2
	end
	if(key==34 and yspeed < 4.0) then
		yspeed = yspeed+0.2
	end
end

function on_click(nx, ny)
	img_x = nx
	img_y = ny
	draw = 0
end

function on_mouse_move(mx, my)
	if(mx>img_x) then
		img_w = mx - img_x
	else
		img_w = img_x - mx
	end
	if(my>img_y) then
		img_h = my - img_y
	else
		img_h = img_y - my
	end
end

function on_mouse_up(nx, ny)
	draw = 1
end
