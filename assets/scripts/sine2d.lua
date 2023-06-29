
-- num leds x & y

num_leds_width = 21
num_leds_height = 21
offset_center_leds_x = math.floor(num_leds_width / 2)
offset_center_leds_y = math.floor(num_leds_height / 2)

function sineCircularFunction(x, y, r, t)
	return math.sin(r * math.sqrt(x * x + y * y) + t);
end

function update_leds(time)

	for j = 0, num_leds_height - 1 do
		for i = 0, num_leds_width - 1 do
			local led_index = i + j * num_leds_width;

			local r = math.abs(math.floor(60 * sineCircularFunction((i - offset_center_leds_x), (j - offset_center_leds_y), 1, 2 * time)))
			local g = math.abs(math.floor(234 * sineCircularFunction((i - offset_center_leds_x), (j - offset_center_leds_y), 1, 2 * time)))
			local b = math.abs(math.floor(144 * sineCircularFunction((i - offset_center_leds_x), (j - offset_center_leds_y), 1, 2 * time)))

			set_led(led_index, r, g, b)
		end
	end
end