-- Gizmo Meta Data --
gizmo.name = "Pong"
gizmo.author = "Feesh"
gizmo.version = "1.0"
gizmo.description = [[
"Yearn for simpler times? Graphics got you down? Pong it baby!"
]]

-- Applet Dimensions --
gizmo.x = 30
gizmo.y = 20
gizmo.w = 500
gizmo.h = 400
gizmo.bg_col = "#000"
-- Define the paddle and ball tables (objects)
paddle = {}
paddle[0] = {}
paddle[1] = {}
ball = {}

paddle[0].user_id = -1  -- Unassigned User Id
paddle[0].score = 0
-- Beginning of rectangle data 
paddle[0].x = 10
paddle[0].w = 16
paddle[0].h = 64
paddle[0].y = gizmo.h/2-paddle[0].h/2
paddle[0].cols = {}
paddle[0].cols.bg = "#fff"
paddle[0].cols.fg = "#fff"
-- End of Rectangle data

paddle[1].user_id = -1  -- Unassigned User Id
paddle[1].score = 0
-- Beginning of rectangle data 
paddle[1].w = 16
paddle[1].h = 64
paddle[1].x = gizmo.w-10-paddle[1].w
paddle[1].y = gizmo.h/2-paddle[0].h/2
paddle[1].cols = {}
paddle[1].cols.bg = "#fff"
paddle[1].cols.fg = "#fff"
-- End of rectangle data

ball.w = 16
ball.h = 16
ball.x = gizmo.w/2-ball.w/2
ball.y = gizmo.h/2-ball.h/2
ball.cols = {}
ball.cols.bg = "#fff"
ball.cols.fg = "#000"
ball.xspeed = 2.0
ball.yspeed = 3.0
players = 0
pong_sound = nil

function draw_paddles()
	draw.rect(paddle[0], gizmo.bg_buf)
	draw.rect(paddle[1], gizmo.bg_buf)
end

function draw_ball()
	draw.rect(ball, gizmo.bg_buf)
end

function draw_scores()
	if(paddle[0].user_id ~= -1) then
		draw.text(gizmo.w/2-32,8,12,users[paddle[0].user_id].cols.fg,"<b>"..users[paddle[0].user_id].name.."</b>")
	end
	draw.text(gizmo.w/2-32,24,24,"#fff",paddle[0].score)
	if(paddle[1].user_id ~= -1) then
		draw.text(gizmo.w/2+16,8,12,users[paddle[1].user_id].cols.fg,"<b>"..users[paddle[1].user_id].name.."</b>")
	end
		draw.text(gizmo.w/2+16,24,24,"#fff",paddle[1].score)
end

function on_init()
	pong_sound = sound.load("test/pong.ogg")
	pong_sound.pitch = 1.0
	draw_paddles()
	draw_scores()
	draw_ball()
end

function on_update()
	--ball.x = ball.x + ball.xspeed
	ball.y = ball.y + ball.yspeed
	if(ball.y+ball.h > gizmo.h) then
		ball.yspeed = ball.yspeed * -1
		ball.y = gizmo.h-ball.h
		sound.play(pong_sound)
	end
	if(ball.y<0) then
		ball.y = 0
		ball.yspeed = ball.yspeed * -1
		sound.play(pong_sound)
	end
	draw.clear()
	draw_paddles()
	draw_scores()
	draw_ball()
	draw.update_rect(0,0,gizmo.w, gizmo.h)
end

function reset_paddle(pad)
	pad.cols.bg = "#fff"
	pad.cols.fg = "#fff"
	pad.user_id = -1
	players = players - 1
end

function set_paddle(pad, user_id)
	pad.cols.fg = users[user_id].cols.fg
	pad.cols.bg = users[user_id].cols.bg
	pad.user_id = user_id
	players = players + 1
end

function on_mouse_down(x, y, button)
	if ((paddle[0].user_id == -1 or paddle[0].user_id == gizmo.current_user) and paddle[1].user_id ~= gizmo.current_user) then	 
		if (x > paddle[0].x and x < paddle[0].x+paddle[0].w) then
			if (y > paddle[0].y and y < paddle[0].y+paddle[0].h) then
				if (paddle[0].user_id == -1) then
					set_paddle(paddle[0],gizmo.current_user)
				elseif (paddle[0].user_id == gizmo.current_user) then
					reset_paddle(paddle[0])
				end
				draw_paddles()
				draw.update_rect(0,0,gizmo.w, gizmo.h)
			end
		end
	end
	if ((paddle[1].user_id == -1 or paddle[1].user_id == gizmo.current_user) and paddle[0].user_id ~= gizmo.current_user) then	 
		if (x > paddle[1].x and x < paddle[1].x+paddle[0].w) then
			if (y > paddle[1].y and y < paddle[1].y+paddle[1].h) then
				if (paddle[1].user_id == -1) then
					set_paddle(paddle[1],gizmo.current_user);
				elseif (paddle[1].user_id == gizmo.current_user) then
					reset_paddle(paddle[1])
				end
				draw_paddles()
				draw.update_rect(0,0,gizmo.w, gizmo.h)
			end
		end
	end
end

function on_mouse_move(x, y, button)
	if (paddle[0].user_id == gizmo.current_user) then
		paddle[0].y = y-paddle[0].h/2
		if (paddle[0].y<0) then
			paddle[0].y = 0
		elseif(paddle[0].y > gizmo.h-paddle[0].h) then
			paddle[0].y = gizmo.h-paddle[0].h
		end
	end
	if (paddle[1].user_id == gizmo.current_user) then
		paddle[1].y = y-paddle[0].h/2
		if (paddle[1].y<0) then
			paddle[1].y = 0
		elseif (paddle[1].y > gizmo.h-paddle[1].h) then
			paddle[1].y = gizmo.h-paddle[1].h
		end
	end
end

function on_key_down(keyval)

end

function on_key_up(keyval)

end
