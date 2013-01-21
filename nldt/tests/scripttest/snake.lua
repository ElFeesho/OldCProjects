-- Snake! 

gizmo.author = "Feesh!"
gizmo.name = "Snake"
gizmo.description = "Good old snake"
gizmo.version = 1.0

gizmo.x = 10
gizmo.y = 10
gizmo.w = 500
gizmo.h = 400
gizmo.bg_col = "#fff"

--Generate a random seed for the apples
math.randomseed(os.time())

board = {}
board.x = 10
board.y = 20
board.w = math.floor((gizmo.w - 20)/20)*20
board.h = math.floor((gizmo.h - 30)/20)*20
board.sw = 1
board.cols = {}
board.cols.fg = "#000"
board.cols.bg = "#ddd"

apple = {}
apple.x = math.random(0,(math.floor((gizmo.w -20)/20))-1)*20+10
apple.y = math.random(0,(math.floor((gizmo.h -30)/20))-1)*20+20
apple.w = 20
apple.h = 20
apple.cols = {}
apple.cols.fg = "#000"
apple.cols.bg = "#f00"

snake = {} 

apple_img = nil

pause = 0
score = 0
sncount = 5
gameover = -1
can_change = 1
dir = 3 -- 0 up, 1, down, 2 left, 3 right

barriers = 0

last_time = 0
update_time = 0.05

chomp_sound = nil
--Widgets
main_menu = nil
easy = nil
normal = nil
hard = nil
barrier_button = nil
retry_button = nil

function eat_apple()
	score = score + 1
	chomp_sound.pitch = 1.0 + 0.01*score
	sound.play(chomp_sound)
	sncount = sncount+1
	snake[sncount] = {}
	snake[sncount].x = snake[sncount-1].x
	snake[sncount].y = snake[sncount-1].y
	snake[sncount].w = snake[sncount-1].w
	snake[sncount].h = snake[sncount-1].h
	snake[sncount].cols = {}
	snake[sncount].cols.fg = snake[sncount-1].cols.fg
	snake[sncount].cols.bg = snake[sncount-1].cols.bg
	ok_apple = false
	while (ok_apple == false) do
		apple.x = math.random(0,(math.floor((gizmo.w -20)/20)-1))*20+10
		apple.y = math.random(0,(math.floor((gizmo.h -30)/20)-1))*20+20
		ok_apple = true
		for cseg = 0, #snake, 1 do
			if(snake[cseg].x == apple.x and snake[cseg].y == apple.y) then
				ok_apple = false
				break
			end
		end
	end
end

function game_over()
	gameover = 1
	for cseg = 0, #snake, 1 do
		snake[cseg] = nil
	end
	draw.rect(board,gizmo.bg_buf)	

	draw.text(20,40,63,"#000","Game Over!")
	draw.text(gizmo.w/3,300,20,"#000","Your Score: "..score)
	draw.update_rect(0,0,gizmo.w,gizmo.h)
	main_menu = gtk.button_new("Main Menu",gizmo.w/2-50,gizmo.h/2-50,100,30)
	retry_button = gtk.button_new("Retry?",gizmo.w/2-50,gizmo.h/2-15,100,30)
	gtk.signal_connect(retry_button, "clicked","restart")
	gtk.signal_connect(main_menu,"clicked","main_menu_show")
end

function main_menu_show(widget)
	gtk.destroy(widget)
	gtk.destroy(retry_button)
	sncount = 5
	dir = 3
	score = 0
	gameover = -1
	on_init()
end

function restart(widget)
	gtk.destroy(widget)
	gtk.destroy(main_menu)
	sncount = 5
	on_init()
	dir = 3
	gameover = 0
	score = 0
	gtk.hide(easy)
	gtk.hide(normal)
	gtk.hide(hard)
	gtk.hide(barrier_button)
end

function easy_start(widget)
	gameover = 0
	score = 0
	dir = 3
	update_time = 0.09
	gtk.hide(easy)
	gtk.hide(normal)
	gtk.hide(hard)
	gtk.hide(barrier_button)
end

function normal_start(widget)
	gameover = 0
	score = 0
	dir = 3
	update_time = 0.05
	gtk.hide(easy)
	gtk.hide(normal)
	gtk.hide(hard)
	gtk.hide(barrier_button)
end

function hard_start(widget)
	gameover = 0
	score = 0
	dir = 3
	update_time = 0.02
	gtk.hide(easy)
	gtk.hide(normal)
	gtk.hide(hard)
	gtk.hide(barrier_button)
end

function toggle_barriers(widget)
	if(barriers == 0) then
		gtk.button_set_text(widget, "Barriers")
		board.sw = 2
		barriers = 1
	else
		gtk.button_set_text(widget, "No Barriers")
		board.sw = 1
		barriers = 0
	end		
end

function on_init()
	draw.clear()
	if(chomp_sound == nil) then
		chomp_sound = sound.load("sounds/chomp.ogg")
	end
	if(easy == nil) then
		easy = gtk.button_new("Easy",gizmo.w/2-40,gizmo.h/2-70,80,30)
		gtk.signal_connect(easy,"clicked","easy_start")
	else
		gtk.show(easy)
	end
	if(normal == nil) then
		normal = gtk.button_new("Normal",gizmo.w/2-40,gizmo.h/2-35,80,30)
		gtk.signal_connect(normal,"clicked","normal_start")
	else
		gtk.show(normal)
	end
	if(hard == nil) then
		hard = gtk.button_new("Hard",gizmo.w/2-40,gizmo.h/2,80,30)
		gtk.signal_connect(hard,"clicked","hard_start")
	else
		gtk.show(hard)
	end
	if(barrier_button == nil) then
		barrier_button = gtk.button_new("No Barriers",gizmo.w/2-40,gizmo.h/2+35,80,30)
		gtk.signal_connect(barrier_button,"clicked","toggle_barriers")
	else
		gtk.show(barrier_button)
	end
	if(apple_img == nil) then
		apple_img = draw.image_load("images/apple.png")
	end
	for cseg = 0,sncount,1 do
		snake[cseg] = {}
		snake[cseg].cols = {}
		snake[cseg].cols.fg = "#000"
		snake[cseg].cols.bg = users[gizmo.current_user].cols.bg
		snake[cseg].x = (math.floor((gizmo.w/2)/20)*20 - (20*cseg))+10
		snake[cseg].y = math.floor((gizmo.h/2)/20)*20
		snake[cseg].w = 20
		snake[cseg].h = 20
	end
	if (gameover == -1) then
		draw.text(gizmo.w/2-90,20,48,"#000","<b>Snake</b>")		
		draw.update_rect(0,0,gizmo.w,gizmo.h)
	end
end

function on_update(time)
	if(time<last_time+update_time or gameover ~= 0) then
		return
	end
	last_time = time
	draw.clear()
	draw.rect(board, gizmo.bg_buf)
	if(gameover == 0) then	
		if(pause == 0) then
			draw.text(10,0,12,users[gizmo.current_user].cols.fg,users[gizmo.current_user].name.." Score: "..score)
			for cseg = #snake,0,-1 do
				if(cseg~=0) then
					snake[cseg].x = snake[cseg-1].x
					snake[cseg].y = snake[cseg-1].y
					if(snake[0].x == snake[cseg].x and snake[0].y == snake[cseg].y and cseg>1) then
						game_over()
						return
					end
				else
					if(dir == 0) then
						snake[0].y = snake[0].y - 20
					end
					if(dir == 1) then
						snake[0].y = snake[0].y + 20
					end
					if(dir == 2) then
						snake[0].x = snake[0].x - 20
					end
					if(dir == 3) then
						snake[0].x = snake[0].x + 20
					end
					if(snake[0].x < board.x or snake[0].y < board.y or snake[0].x >= board.x+board.w or snake[0].y >= board.y+board.h) then
						if(barriers == 1) then
							game_over()
							return
						else
							if(snake[0].x < board.x) then
								snake[0].x = board.x+board.w-20
							end
							if(snake[0].y < board.y) then
								snake[0].y = board.y+board.h-20
							end
							if(snake[0].x >= board.x+board.w) then
								snake[0].x = board.x
							end
							if(snake[0].y >= board.y+board.h) then
								snake[0].y = board.y
							end
						end
					end
					if(snake[0].x == apple.x and snake[0].y == apple.y) then
						eat_apple()
					end
				end
				draw.rect(snake[cseg], gizmo.bg_buf)
			end
			draw.image(apple_img, apple)
		else
				draw.text(20,40,63,"#000","Paused (P)")
		end
	end
	can_change = 1
	draw.update_rect(0,0,gizmo.w,gizmo.h)
end

function on_mouse_move(x, y, button)

end

function on_mouse_down(x, y, button)

end

function on_mouse_up(x, y, button)

end

function on_key_down(button)
	if(can_change == 1) then
		if(button == 65362 and dir~=1) then
			dir = 0
			can_change = 0
		end
		if(button == 65364 and dir~=0) then
			dir = 1
			can_change = 0
		end
		if(button == 65361 and dir~=3) then
			dir = 2
			can_change = 0
		end
		if(button == 65363 and dir~=2) then
			dir = 3
			can_change = 0
		end
		if(button == string.byte("p")) then
			if(pause == 0) then
				pause = 1
			else
				pause = 0
			end
		end
	end
end

function on_key_up(button)

end
