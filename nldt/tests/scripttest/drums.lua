-- Initialise Drum Gizmo

gizmo.name = "NLDT Drum Machine"
gizmo.author = "Feesh!"
gizmo.description = "Drum Machine!"

gizmo.w = 400
gizmo.h = 400
gizmo.x = 20
gizmo.y = 20
gizmo.bg_col = "#f90"

sounds = {}

function on_init()
	sounds[0] = sound.load("sounds/bdrum.ogg")
	sounds[1] = sound.load("sounds/snarel.ogg")
	sounds[2] = sound.load("sounds/snarer.ogg")
	sounds[3] = sound.load("sounds/crash.ogg")
	sounds[4] = sound.load("sounds/chcrash.ogg")
	sounds[5] = sound.load("sounds/hihatc.ogg")
	sounds[6] = sound.load("sounds/hihat.ogg")
	sounds[7] = sound.load("sounds/hihatp.ogg")
	sounds[8] = sound.load("sounds/htom.ogg")
	sounds[9] = sound.load("sounds/mtom.ogg")
	sounds[10] = sound.load("sounds/ltom.ogg")
	sounds[11] = sound.load("sounds/ride.ogg")
	sounds[12] = sound.load("sounds/stsnare.ogg")
	sounds[13] = sound.load("sounds/splash.ogg")
end

function on_update()
	draw.clear()
	draw.text(gizmo.w/2-100, gizmo.h/2,36,"#000","NLDT Drum Machine")
	draw.update_rect(0,0,gizmo.w, gizmo.h)
end

function on_mouse_down(x,y,button)

end

function on_mouse_move(x,y,button)

end

function on_mouse_up(x,y,button)

end

function on_key_down(keyval)
	if(keyval == string.byte(" ")) then
		sound.play(sounds[0])  -- Bass Drum
	end
	-- Snares
	if(keyval == string.byte("d")) then
		sound.play(sounds[1])
	end
	if(keyval == string.byte("f")) then
		sound.play(sounds[1])
	end
	if(keyval == string.byte("s")) then
		sound.play(sounds[12])
	end
	-- Crash
	if(keyval == string.byte("y")) then
		sound.play(sounds[3])
	end
	if(keyval == string.byte("u")) then
		sound.play(sounds[4])
	end
	--Splash
	if(keyval == string.byte("e")) then
		sound.play(sounds[13])
	end
	--Ride
	if(keyval == string.byte("r")) then
		sound.play(sounds[11])
	end
	-- Toms
	if(keyval == string.byte("l")) then
		sound.play(sounds[8])	
	end
	if(keyval == string.byte("j")) then
		sound.play(sounds[9])
	end
	if(keyval == string.byte("h")) then
		sound.play(sounds[10])
	end
	if(keyval == string.byte("a")) then
		sound.play(sounds[10])
	end
	print("Val: ",keyval)
end

function on_key_up(button)
end
