

--[[Yeah, I robbed this from yuriks, who robbed it from mr. carmack- even if he 
didn't realise it maybe, having object's 'think' is just a way of game 
programming. At any rate... this function basically tells our lil paddle to 
get it's ass into gear.]]

yspeed = 5

function do_update()
	if(ball.x>320) then
		if(paddle.y<ball.y) then
			paddle.y = paddle.y + yspeed
			if(paddle.y+paddle.h > 480) then
				paddle.y = 480 - paddle.h
			end
		else
			paddle.y = paddle.y - yspeed
			if(paddle.y < 0) then
				paddle.y = 0
			end
		end
	else
		if(paddle.y > 240-5 and paddle.y < 245) then
			paddle.y = 240
		else
			if(paddle.y > 240) then
				paddle.y = paddle.y - yspeed
			else
				paddle.y = paddle.y + yspeed
			end	
		end
	end
end
