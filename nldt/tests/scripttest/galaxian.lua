gizmo.author = "Feesh!"
gizmo.name = "Galaxian"
gizmo.description = "The latest in Feesh!'s knock offs, Galaxian (space invaders)"

gizmo.bg_col = "#000"
gizmo.x = 0
gizmo.y = 0
gizmo.w = 600
gizmo.h = 500

-- Global Vars

ship = {}
ship.h = 32
ship.w = 32
ship.x = gizmo.w/2-16
ship.y = gizmo.h-48
ship.xspeed = 0

invaders = nil

inv_dir = 1 -- Move to the right to start off with

game_mode = 0 -- Main screen

update_time = 0.7
last_time = 0

resources = nil

max_bullets = 2
bullet_count = 0
bullets = nil

start_game = nil

--/Global Vars

-- Helper Functions

function load_resources()
	--resources = {}
	--resources.invaders = {}
	--for inv = 0, 6, 1 do
	--	resources.invaders[inv] = draw.image_load("images/inv"..inv..".png")
	--end
	--resources.ship = draw.image_load("images/ship.png")
	
	--TODO: Sounds
	
end

function generate_bullet()
	if(bullets == nil) then
		bullets = {}
	end
	if(bullet_count==max_bullets) then
		return
	end
	bullets[bullet_count] = {}
	bullets[bullet_count].x = ship.x+14
	bullets[bullet_count].y = ship.y-8
	bullets[bullet_count].w = 4
	bullets[bullet_count].h = 8
	bullets[bullet_count].cols = {}
	bullets[bullet_count].cols.fg = "#0f0"
	bullets[bullet_count].cols.bg = "#0f0"
	bullet_count = bullet_count + 1
end

function update_bullets()
	if(bullets ~= nil) then
		for bcount = #bullets, 0,-1 do
			if(bullets[bcount]~=nil) then
				bullets[bcount].y = bullets[bcount].y - 4
				if(bullets[bcount].y < -16) then
					table.remove(bullets,bcount)
					bullet_count = bullet_count - 1
				end
				for cols = 0, 6, 1 do
					invaders[cols] = {} -- Make 7 columns for invaders
					for rows = 0, 5, 1 do
						if(invaders[cols][rows]~=nil) then
							if(bullets[bcount].x+4 > invaders[cols][rows].x and bullets[bcount].x < invaders[cols][rows].x+32 and bullets[bcount].y+8 > invaders[cols][rows].y and bullets[bcount].y < invaders[cols][rows].y+32) then
								table.remove(bullets,bcount)
								table.remove(invaders[cols],rows)
								bullet_count = bullet_count - 1
								break
							end
						end
					end
				end
			end
		end
	end
end

function draw_bullets()
	if(bullets~=nil) then
		for bcount = #bullets, 0,-1 do
			if(bullets[bcount]~=nil) then
				draw.rect(bullets[bcount])
			end
		end
	end
end

function generate_invaders()
	-- 400px wide screen, 32px wide bad guys, 14 guys length ways
	-- 500px tall, need a bit of space between em, 7 rows? (32px high)
	if(resources == nil) then
		load_resources() -- Make sure images are loaded
	end
	invaders = nil -- Make gc swallow any old invaders
	invaders = {}
	for cols = 0, 6, 1 do
		invaders[cols] = {} -- Make 7 columns for invaders
		for rows = 0, 5, 1 do
			invaders[cols][rows] = {} -- Make 6 rows of invaders
			invaders[cols][rows].x = cols * 40 +10  -- 8px gap between invaders
			invaders[cols][rows].y = rows * 40 +10 -- 8px gap between invaders
			invaders[cols][rows].w = 32
			invaders[cols][rows].h = 32
			invaders[cols][rows].img_inx = rows
		end
	end
	print("Generated")
end

function draw_main_screen()
	draw.clear()
	draw.text(10,10,50,"#fff","Galaxian")
	draw.update_rect(0,0,gizmo.w, gizmo.h)
	if(start_game == nil) then
		start_game = gtk.button_new("Start Game",gizmo.w/2-40,gizmo.h/2-20,80,40)
		gtk.signal_connect(start_game,"clicked","game_start")
	else
		gtk.show(start_game)
	end	
end

function reset_globals()
	inv_dir = 1
	generate_invaders()
end

function game_start(widget)
	gtk.hide(widget)
	game_mode = 1
	reset_globals()
end

function draw_ship()
	ship.x = ship.x + ship.xspeed
	draw.rect(ship.x, ship.y, ship.w, ship.h, "#fff","#f00",gizmo.bg_buf)
end

function update_invaders()
	if(invaders == nil) then
		return
	end
	if(inv_dir == 1) then
		if(invaders[#invaders][0] ~= nil) then
			if(invaders[#invaders][0].x+32 < gizmo.w-40) then
				for cols = 0, #invaders, 1 do
					for rows = 0, #invaders[cols], 1 do
						if(invaders[cols][rows]~=nil) then
							invaders[cols][rows].x = invaders[cols][rows].x + 40
						end
					end
				end
			else
				for cols = 0, #invaders, 1 do
					for rows = 0, #invaders[cols], 1 do
						if(invaders[cols][rows]~=nil) then
							invaders[cols][rows].y = invaders[cols][rows].y + 40
						end
					end
				end
				inv_dir = 0
			end
		end
	else
		if(invaders[0][0] ~= nil) then
			if(invaders[0][0].x > 40) then
				for cols = 0, #invaders, 1 do
					for rows = 0, #invaders[cols], 1 do
						if(invaders[cols][rows]~=nil) then
							invaders[cols][rows].x = invaders[cols][rows].x - 40
						end
					end
				end
			else
				for cols = 0, #invaders, 1 do
					for rows = 0, #invaders[cols], 1 do
						if(invaders[cols][rows]~=nil) then
							invaders[cols][rows].y = invaders[cols][rows].y + 40
						end
					end
				end
				inv_dir = 1
			end
		end
	end
end

function draw_invaders()
	for cols = 0, #invaders, 1 do
		for rows = 0, #invaders[cols], 1 do
			if(invaders[cols][rows]~=nil) then
				draw.rect(invaders[cols][rows].x, invaders[cols][rows].y, invaders[cols][rows].w, invaders[cols][rows].h, "#0f0","#060",gizmo.bg_buf)
			end
		end
	end
end

--/Helper Functions

function on_init()
	draw_main_screen()
end

function on_update(time)
	if(game_mode == 1) then
		draw.clear()
		draw_ship()
		if(time>last_time + update_time) then
			update_invaders()
			last_time = time
		end
		draw_invaders()
		update_bullets()
		draw_bullets()
		draw.update_rect(0,0,gizmo.w, gizmo.h)
	end
end

function on_key_down(keyval)
	if(game_mode == 1) then
		if(keyval==65361) then
			ship.xspeed = -5
		elseif(keyval == 65363) then
			ship.xspeed = 5
		end
		if(keyval == 32 and bullet_count < max_bullets) then
			generate_bullet()
		end
	end
end

function on_key_up(keyval)
	if(keyval==65361 or keyval == 65363) then
		ship.xspeed = 0
	end
end

function on_mouse_down(x, y, button)

end

function on_mouse_move(x, y, button)

end

function on_mouse_up(x, y, button)

end