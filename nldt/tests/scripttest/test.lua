gizmo.name = "Drawing Test"
gizmo.author = "Feesh!"
gizmo.description = "Drawing Test Script: Showcases Drawing Methods"

gizmo.x = 10
gizmo.y = 10
gizmo.w = 580
gizmo.h = 450
gizmo.bg_col = "#fff"

-- Test Boxes

box = {}
box[0] = {}
box[1] = {}
box[2] = {}
box[3] = {}
box[4] = {}
box[5] = {}
box[6] = {}
box[7] = {}
box[8] = {}
box[9] = {}

current_box = -1
hl_box = -1

for cbox=0,9,1 do
	box[cbox].x = 48+cbox*32
	box[cbox].y = 50
	box[cbox].w = 32
	box[cbox].h = 32
	box[cbox].pitch = 1+(0.1*cbox)
	box[cbox].cols = {}
	box[cbox].cols.fg = "black"
	box[cbox].cols.bg = "blue"
	box[cbox].key = -1
end

function on_init()
	tone = sound.load("sounds/hello.ogg")
	for cbox = 0, 9, 1 do
		draw.rect(box[cbox],gizmo.bg_buf)
	end
end

function on_update()
	draw.clear()
	draw.text(30,10,24,"#000","NLDT Test Bed")
	draw.text(28,8,24,"#fff","NLDT Test Bed")
	if(hl_box~=-1) then
		box[hl_box].cols.fg = "#ff0"
	end
	if(current_box~=-1) then
		box[current_box].sw = 2
	end
	for cbox = 0, 9, 1 do
		draw.rect(box[cbox],gizmo.bg_buf)
		if(box[cbox].key~=-1) then
			draw.text(box[cbox].x+5, box[cbox].y,20,"#fff",string.char(box[cbox].key))
		end
	end
	if(current_box~=-1) then
		box[current_box].sw = 1
	end
	if(hl_box~=-1) then
		box[hl_box].cols.fg = "#000"
	end
	draw.update_rect(0,0,gizmo.w,gizmo.h)
end

function on_mouse_down(x, y, button)
	for cbox=0,9,1 do
		if(x > box[cbox].x and x < box[cbox].x+ box[cbox].w and y > box[cbox].y and y < box[cbox].y+box[cbox].h) then
			if(button == 1) then
				sound.stop(tone)
				current_box = cbox
				tone.pitch = box[cbox].pitch
				sound.play(tone)
			elseif(button == 2) then
				if(hl_box ~= cbox) then
					hl_box = cbox
				else
					hl_box = -1
				end
			end
		end
	end
end

function on_mouse_move(x,y,button)
	if(button == 1) then
		for cbox=0,9,1 do
			if(x > box[cbox].x and x < box[cbox].x+ box[cbox].w and y > box[cbox].y and y < box[cbox].y+box[cbox].h and current_box ~= cbox) then
				current_box = cbox
				tone.pitch = box[cbox].pitch
				sound.play(tone)
			end
		end
	end
end

function on_key_down(key_val)
	if(hl_box~=-1) then
		for cbox=0,9,1 do
			if(box[cbox].key == key_val) then
				box[cbox].key = -1
				if(hl_box == cbox) then
					hl_box = -1
					return  --We effectively just resetted the key for the box to -1
				end
				break
			end
		end
		box[hl_box].key = key_val
		hl_box=-1
	else
		for cbox=0,9,1 do
			if(box[cbox].key == key_val) then
				sound.stop(tone)
				tone.pitch = box[cbox].pitch
				sound.play(tone)
			end
		end
	end
end

function on_key_up(button)

end
