-- Good ol Card game known as "Pairs" - to me any who

gizmo.name = "Pairs"
gizmo.author = "Feesh!"
gizmo.version = 1.0
gizmo.description = [["Memorise card positions and match up the pairs you can find. The fewer
attempts you use the better!"
]]

gizmo.x = 0
gizmo.y = 0
gizmo.w = 600
gizmo.h = 500

gizmo.bg_col = "#090" -- Classic dark green background for card game

math.randomseed(os.time()) -- start off a random seed

ui = {} -- Table of UI elements
ui.start = nil
ui.new_game = nil
ui.card_bg = nil
ui.cards = {} -- Table of all the possible card combos

cards = {} -- Table of 'physical' cards containing positions

-- UI Callbacks

card_one = -1 -- Store the current flipped cards
card_two = -1

game_mode = 0 -- 0 = Menu, 1 = Game, 2 = Stats screen

attempts = 0 -- How many attempts have been taken

matched = 0

timeout = 0.5 -- Time till cards reset after wrong pair

locked = false -- Whether a card can be turned or not

last_flip_time = 0

function start_game(widget)
	attempts = 0
	game_mode = 1
	matched = 0
	generate_cards()
	locked = false
	gtk.hide(widget)
end

function new_game(widget)
	game_mode = 0
	gtk.hide(widget)
	draw.clear()
	draw.text(30,30,50,"#fff","Pairs!")
	draw.update_rect(0,0,gizmo.w, gizmo.h)
	gtk.show(ui.start)
end

-- /UI Callbacks

-- Helper Functions

-- For use with the on_mouse_down callback
function flip_card(index)

end

-- Draw the cards appropriately
function draw_cards()
	for ccard = 0, 31, 1 do
		if(cards[ccard].flipped == true) then
			draw.image(ui.cards[cards[ccard].index],cards[ccard])
		else
			draw.image(ui.card_bg,cards[ccard])
		end
	end
end

-- Generate a random assortment of cards
function generate_cards()
-- Nice long string to jumble up - Base 20
	indices = {}
	for cindex = 1, 32, 1 do
		indices[cindex-1] = math.ceil(cindex/2)-1
	end
-- Jumble time
	for cindex = 0, 31, 1 do
		cur_ind = indices[cindex]
		swap_ind = math.random(0,31)
		indices[cindex] = indices[swap_ind]
		indices[swap_ind] = cur_ind
	end
	
-- Initialise Card table (40 cards)
	for ccard = 0, 31, 1 do
		cards[ccard] = {}
		cards[ccard].index = indices[ccard]
		cards[ccard].flipped = false
		cards[ccard].x = ccard*64+20 - (math.floor(ccard/8)*512) + (ccard*5- (math.floor(ccard/8)*40))
		cards[ccard].y = 40+(math.floor(ccard/8)*96)+(math.floor(ccard/8)*5)
		cards[ccard].w = 64
		cards[ccard].h = 96
	end
end

-- /Helper Functions

-- Engine Specific Functions

function on_init()
	-- Initialise UI elements
	ui.start = gtk.button_new("Start Game",gizmo.w/2-40,gizmo.h/2-20,80,40)
	gtk.signal_connect(ui.start,"clicked","start_game")
	ui.new_game = gtk.button_new("New Game",gizmo.w/2-40,gizmo.h/2-20,80,40)
	gtk.signal_connect(ui.new_game,"clicked","new_game")
	gtk.hide(ui.new_game)
	
	-- Load card background
	ui.card_bg = draw.image_load("images/cardbg.png")
	
	-- Load card images
	for ccard = 0, 15, 1 do
		ui.cards[ccard] = draw.image_load("images/card"..ccard..".png")
	end	
	
	draw.text(30,30,50,"#fff","Pairs!")
end

function on_update(time)
	if(game_mode == 0) then
		return  -- Nothing to draw on the menu screen
	end
	if(game_mode == 1) then
		draw.clear()
		draw_cards()
		if(card_one ~= -1 and card_two ~= -1) then
			locked = true
			if(last_flip_time == 0) then
				last_flip_time = time
			end
			if(cards[card_one].index ~= cards[card_two].index) then
				if(time>last_flip_time+timeout) then
					locked = false
					last_flip_time = 0
					cards[card_one].flipped = false
					cards[card_two].flipped = false
					card_one = -1
					card_two = -1
				end
			else
				matched = matched + 1
				locked = false
				card_one = -1
				card_two = -1
				last_flip_time = 0
			end
		end
		draw.update_rect(0,0,gizmo.w, gizmo.h)
		if(matched == 16) then
			game_mode = 2
			draw.clear()
			draw.text(20,20,36,"#fff","Congratulations!")
			draw.text(20,80,16,"#fff","You took just "..attempts.." attempts to match all the pairs")
			draw.update_rect(0,0,gizmo.w, gizmo.h)
			gtk.show(ui.new_game)
		end
	end
end

function on_expose()

end

function on_mouse_down(x, y, button)
	if(locked == false) then
		for ccards = 0, 31,1 do
			if(x > cards[ccards].x and x < cards[ccards].x+cards[ccards].w and y > cards[ccards].y and y < cards[ccards].y+cards[ccards].h and cards[ccards].flipped == false) then
				cards[ccards].flipped = true
				if(card_one == -1) then	
					card_one = ccards
				else
					card_two = ccards
					attempts = attempts + 1
				end
			end
		end
	end
end

function on_mouse_move(x, y, button)

end

function on_mouse_up(x, y, button)

end

function on_key_down(keyval)

end

function on_key_up(keyval)

end

function on_destroy(keyval)

end

-- /Engine specific functions
