-- Gizmo Data
gizmo.name = "Asteroids"
gizmo.author = "Feesh!"
gizmo.version = 1.0
gizmo.description = "The Timeless Classic! Asteroids"

gizmo.bg_col = "#000"

--Gizmo position data

gizmo.x = 0
gizmo.y = 0
gizmo.w = 600
gizmo.h = 480

--Random seed baby
math.randomseed(os.time())

--Ship
ship = {}
ship.x = gizmo.w/2
ship.y = gizmo.h/2
ship.rot = -90
ship.xspeed = 0
ship.yspeed = 0
ship.alive = 1
ship.die_time = 0

--Bullets
bullets = {}

--Lives
lives = 3
score = 0
gameover = 0
accel = 0.1

--Asteroids
asteroids = {}

key_left = 0
key_right = 0
key_up = 0
key_fire = 0

fire_shot = 0

max_bullets = 100
bullet_timeout = 1.5
bullet_speed = 10
bullet_count = 0

round = 1

clear_time = 0

restart_but = nil

function restart()
	on_init()
	gtk.destroy(restart_but)
	restart_but = nil
end

function draw_ship(time)
	if(ship.alive == 1) then
		tp_x = ship.x+math.cos(math.rad(ship.rot))*20
		tp_y = ship.y+math.sin(math.rad(ship.rot))*20
		br_x = ship.x+math.cos(math.rad(ship.rot+120))*10
		br_y = ship.y+math.sin(math.rad(ship.rot+120))*10
		bl_x = ship.x+math.cos(math.rad(ship.rot+240))*10
		bl_y = ship.y+math.sin(math.rad(ship.rot+240))*10
		draw.line(tp_x, tp_y, br_x, br_y,2,users[0].cols.fg,gizmo.bg_buf)
		draw.line(tp_x,tp_y,bl_x,bl_y,2,users[0].cols.fg,gizmo.bg_buf)
		draw.line(bl_x,bl_y,ship.x,ship.y,2,users[0].cols.fg,gizmo.bg_buf)
		draw.line(br_x,br_y,ship.x,ship.y,2,users[0].cols.fg,gizmo.bg_buf)
		if(asteroids~=nil) then
			for cast = 0, #asteroids, 1 do
				if(ship.x>asteroids[cast].x-asteroids[cast].r and ship.x<asteroids[cast].x+asteroids[cast].r and ship.y>asteroids[cast].y-asteroids[cast].r and ship.y<asteroids[cast].y+asteroids[cast].r) then
					ship.alive = 0
					ship.die_time = time
					ship.debris = {}
					ship.debris.points = {}
					ship.debris.points[0] = {}
					ship.debris.points[0].x = tp_x
					ship.debris.points[0].y = tp_y
					ship.debris.points[0].rot = math.random(0,360)
					ship.debris.points[0].rotspeed = math.random(3,8)
					ship.debris.points[1] = {}
					ship.debris.points[1].x = tp_x
					ship.debris.points[1].y = tp_y
					ship.debris.points[1].rot = math.random(0,360)
					ship.debris.points[1].rotspeed = math.random(3,8)
					ship.debris.points[2] = {}
					ship.debris.points[2].x = br_x
					ship.debris.points[2].y = br_y
					ship.debris.points[2].rot = math.random(0,360)
					ship.debris.points[2].rotspeed = math.random(3,8)
					ship.debris.points[3] = {}
					ship.debris.points[3].x = bl_x
					ship.debris.points[3].y = bl_y
					ship.debris.points[3].rot = math.random(0,360)
					ship.debris.points[3].rotspeed = math.random(3,8)
					destroy_asteroid(cast)
					lives = lives - 1
					break
				end
			end
		end
	else
		if(time<ship.die_time+3) then
			for debris = 0, 3, 1 do
				ship.debris.points[debris].x = ship.debris.points[debris].x + ship.xspeed
				ship.debris.points[debris].y = ship.debris.points[debris].y + ship.yspeed
				ship.debris.points[debris].rot = ship.debris.points[debris].rot + ship.debris.points[debris].rotspeed
				draw.line(ship.debris.points[debris].x,ship.debris.points[debris].y,ship.debris.points[debris].x+math.cos(math.rad(ship.debris.points[debris].rot))*20,ship.debris.points[debris].y+math.sin(math.rad(ship.debris.points[debris].rot))*20,2,users[0].cols.fg,gizmo.bg_buf)
			end
		else
			if(lives==0) then
				gameover = 1
				draw.clear()
				asteroids = nil
				draw.text(gizmo.w/4,gizmo.h/2-36,36,"#fff","GAME OVER")
				draw.update_rect(gizmo)
				restart_but = gtk.button_new("Restart?",gizmo.w/2-40,gizmo.h/2+50,80,30)
				gtk.signal_connect(restart_but,"clicked","restart")
				return
			end
			ship.x = gizmo.w/2
			ship.y = gizmo.h/2
			ship.xspeed = 0
			ship.yspeed = 0
			ship.rot = -90
			if(asteroids~=nil) then
				for cast = 0, #asteroids, 1 do
					if(ship.x>asteroids[cast].x-asteroids[cast].r and ship.x<asteroids[cast].x+asteroids[cast].r and ship.y>asteroids[cast].y-asteroids[cast].r and ship.y<asteroids[cast].y+asteroids[cast].r) then
						return
					end
				end
			end
			ship.alive = 1
		end
	end
end

function generate_debris(x,y,radius)
	for index=#asteroids+1,#asteroids+2,1 do
		asteroids[index] = {}
		asteroids[index].x = x
		asteroids[index].y = y	
		asteroids[index].r = radius
		asteroids[index].xspeed = math.random(0.3,1.5)
		asteroids[index].yspeed = math.random(0.3,1.5)
		if(math.random(1,2)<1.5) then
			asteroids[index].xspeed = asteroids[index].xspeed * -1
		end
		if(math.random(1,2)<1.5) then
			asteroids[index].yspeed = asteroids[index].yspeed * -1
		end
		asteroids[index].rotspeed = math.random(-6,6)
		asteroids[index].rot = math.random(0,359)
		asteroids[index].points = {}
		for cang = 0, 10, 1 do
			asteroids[index].points[cang] = {}
			asteroids[index].points[cang].offset = math.random(asteroids[index].r/8*7,asteroids[index].r)
		end
	end
end

function generate_asteroids(amount)
	asteroids = nil
	if(asteroids==nil) then
		asteroids = {}
	end
	for cast = 0, amount-1, 1 do
		asteroids[cast] = {}
		if(math.random(0,2)<1) then
			asteroids[cast].x = math.random(0,150)
			asteroids[cast].y = math.random(0,gizmo.h)
		else
			asteroids[cast].x = math.random(gizmo.w-150,gizmo.w)
			asteroids[cast].y = math.random(0,gizmo.h)
		end
		asteroids[cast].r = 50
		asteroids[cast].xspeed = math.random(0.3,1)
		asteroids[cast].yspeed = math.random(0.3,1)
		if(math.random(1,2)<1.5) then
			asteroids[cast].xspeed = asteroids[cast].xspeed * -1
		end
		if(math.random(1,2)<1.5) then
			asteroids[cast].yspeed = asteroids[cast].yspeed * -1
		end
		asteroids[cast].rotspeed = math.random(-3,3)
		asteroids[cast].rot = math.random(0,359)
		asteroids[cast].points = {}
		for cang = 0, 10, 1 do
			asteroids[cast].points[cang] = {}
			asteroids[cast].points[cang].offset = math.random(asteroids[cast].r/8*7,asteroids[cast].r)
		end
	end
end

function draw_asteroids()
	if(asteroids == nil) then
		return
	end
	for cast = 0,#asteroids,1 do
		asteroids[cast].x = asteroids[cast].x + asteroids[cast].xspeed
		asteroids[cast].y = asteroids[cast].y + asteroids[cast].yspeed
		asteroids[cast].rot = asteroids[cast].rot + asteroids[cast].rotspeed
		if(asteroids[cast].x-asteroids[cast].r > gizmo.w) then
			asteroids[cast].x = 0-asteroids[cast].r
		end
		if(asteroids[cast].y-asteroids[cast].r > gizmo.h) then
			asteroids[cast].y = 0-asteroids[cast].r
		end
		if(asteroids[cast].x+asteroids[cast].r < 0) then
			asteroids[cast].x = gizmo.w+asteroids[cast].r
		end
		if(asteroids[cast].y+asteroids[cast].r < 0) then
			asteroids[cast].y = gizmo.h+asteroids[cast].r
		end
		for cang = 0, 10, 1 do
			if(cang~=10) then
				px_one = asteroids[cast].x + math.cos(math.rad(asteroids[cast].rot+(36*cang)))*asteroids[cast].points[cang].offset 
				py_one = asteroids[cast].y + math.sin(math.rad(asteroids[cast].rot+(36*cang)))*asteroids[cast].points[cang].offset
				px_two = asteroids[cast].x + math.cos(math.rad(asteroids[cast].rot+(36*(cang+1))))*asteroids[cast].points[cang+1].offset
				py_two = asteroids[cast].y + math.sin(math.rad(asteroids[cast].rot+(36*(cang+1))))*asteroids[cast].points[cang+1].offset
				draw.line(px_one,py_one,px_two,py_two,2,"#08f",gizmo.bg_buf)
			else
				px_one = asteroids[cast].x + math.cos(math.rad(asteroids[cast].rot+(36*cang)))*asteroids[cast].points[cang].offset 
				py_one = asteroids[cast].y + math.sin(math.rad(asteroids[cast].rot+(36*cang)))*asteroids[cast].points[cang].offset
				px_two = asteroids[cast].x + math.cos(math.rad(asteroids[cast].rot))*asteroids[cast].points[0].offset
				py_two = asteroids[cast].y + math.sin(math.rad(asteroids[cast].rot))*asteroids[cast].points[0].offset
				draw.line(px_one,py_one,px_two,py_two,2,"#08f",gizmo.bg_buf)
			end
		end
	end
end

function destroy_asteroid(index,time)
	if(asteroids[index].r > 10) then
		generate_debris(asteroids[index].x,asteroids[index].y,asteroids[index].r/2)
	end
	if(#asteroids == 0) then
		--Next level
		clear_time = time
		asteroids = nil
	else
		table.remove(asteroids,index)
	end
	score = score+1
end

function create_bullet(time)
	if(bullet_count<=max_bullets) then
		bullets[bullet_count] = {}
		bullets[bullet_count].x = ship.x+math.cos(math.rad(ship.rot))*20
		bullets[bullet_count].y = ship.y+math.sin(math.rad(ship.rot))*20
		bullets[bullet_count].rot = ship.rot
		bullets[bullet_count].time = time
		bullets[bullet_count].active = 1
		bullet_count = bullet_count + 1
	end
end

function update_bullets(time)
	for cbul = bullet_count, 0, -1 do
		if(bullets[cbul]~=nil) then
			if(bullets[cbul].active == 1) then
				if(bullets[cbul].time<time-bullet_timeout) then
					table.remove(bullets,cbul)
					if(bullet_count>0) then
						bullet_count = bullet_count - 1
					end
				else
					draw.line(bullets[cbul].x,bullets[cbul].y,bullets[cbul].x,bullets[cbul].y,3,users[0].cols.bg,gizmo.bg_buf)
					bullets[cbul].x = bullets[cbul].x + math.cos(math.rad(bullets[cbul].rot))*bullet_speed 
					bullets[cbul].y = bullets[cbul].y + math.sin(math.rad(bullets[cbul].rot))*bullet_speed
					if(asteroids~=nil) then
						for cast = 0, #asteroids, 1 do
							--INSERT CIRCULAR COLISION HERE!
							if(bullets[cbul].x > asteroids[cast].x-asteroids[cast].r and bullets[cbul].x < asteroids[cast].x+asteroids[cast].r and bullets[cbul].y > asteroids[cast].y-asteroids[cast].r and bullets[cbul].y < asteroids[cast].y+asteroids[cast].r) then
								destroy_asteroid(cast,time)
								if(cbul~=0)then
									table.remove(bullets,cbul)
								else
									bullets[cbul] = nil
								end
								if(bullet_count>0) then
									bullet_count = bullet_count - 1
								end
								break
							end
						end
					end
				end
			end
		end
	end
end

function draw_score()
	draw.text(0,0,10,users[0].cols.fg,"<b>Score: </b>"..score)
end

function draw_lives()
	for count = 1, lives, 1 do
		x = count*8+10
		y = 20
		draw.line(x,y,x-4,y+10,1,users[0].cols.fg,gizmo.bg_buf)
		draw.line(x,y,x+4,y+10,1,users[0].cols.fg,gizmo.bg_buf)
		draw.line(x-4,y+10,x,y+7,1,users[0].cols.fg,gizmo.bg_buf)
		draw.line(x+4,y+10,x,y+7,1,users[0].cols.fg,gizmo.bg_buf)
	end
end

function on_init()
	--Prolly a better way of doing this
	ship.x = gizmo.w/2
	ship.y = gizmo.h/2
	ship.rot = -90
	ship.xspeed = 0
	ship.yspeed = 0
	ship.alive = 1
	ship.die_time = 0
	score = 0
	lives = 3
	gameover = 0
	fire_shot = 0
	accel = 0
	round = 1
	generate_asteroids(4)
end

function on_update(time)
	if(gameover~=1) then
		draw.clear()
		if(ship.alive == 1) then
			if(key_left == 1) then
				ship.rot = ship.rot - 5
			end
			if(key_right == 1) then
				ship.rot = ship.rot + 5
			end
			if(key_up == 1) then		
					if(accel<1) then
						accel = accel + 0.1
					end
					ship.xspeed = ship.xspeed + math.cos(math.rad(ship.rot))*accel
					ship.yspeed = ship.yspeed + math.sin(math.rad(ship.rot))*accel
			else
				accel = accel * 0.5
			end
			if(fire_shot == 1) then
				create_bullet(time)
				fire_shot = 0
			end
		end
		update_bullets(time)
		ship.x = ship.x + ship.xspeed
		ship.y = ship.y + ship.yspeed
		if(ship.x<0) then
			ship.x = gizmo.w
		end
		if(ship.y<0) then
			ship.y = gizmo.h
		end
		if(ship.x>gizmo.w) then
			ship.x = 0
		end
		if(ship.y>gizmo.h) then
			ship.y = 0
		end
		draw_ship(time)
		if(asteroids~=nil) then
			draw_asteroids()
		else
			if(clear_time ~= nil and ship.alive == 1) then
				draw.text(gizmo.w/2+50,gizmo.h/2-36,36,"#fff","Round "..(round+1))
				if(time>clear_time+3) then
					if(ship.x > gizmo.w/2 - 100 and ship.x < gizmo.w/2 + 100 and ship.y > gizmo.h/2-100 and ship.y < gizmo.h/2+100) then
						round = round + 1
						generate_asteroids(4+(math.floor(round/2)))
						clear_time = nil
					end
				end
			end
		end
		draw_score()
		draw_lives()
		draw.update_rect(0,0,600,490)	
	end
	
end

function on_mouse_down(x, y, button)

end

function on_mouse_move(x, y, button)

end

function on_mouse_up(x, y, button)

end

function on_key_down(keyval)
	if(keyval == 65361) then
		key_left = 1
	end
	if(keyval == 65363) then
		key_right = 1
	end
	if(keyval == 65362) then
		key_up = 1
	end
	if(keyval == 32 and key_fire == 0) then
		key_fire = 1
		fire_shot = 1
	end
	if(keyval == 65293) then
		print("DEBUG")
		print("Asteroids: ",#asteroids)
		print("/DEBUG")
	end
end

function on_key_up(keyval)
	if(keyval == 65361) then
		key_left = 0
	end
	if(keyval == 65363) then
		key_right = 0
	end
	if(keyval == 65362) then
		key_up = 0
	end
	if(keyval == 32) then
		key_fire = 0
	end
end
