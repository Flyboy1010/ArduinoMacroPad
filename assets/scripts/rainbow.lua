-- num leds x & y

num_leds_width = 21
num_leds_height = 21
offset_center_leds_x = math.floor(num_leds_width / 2)
offset_center_leds_y = math.floor(num_leds_height / 2)

function colorWheelPattern(x, y, t)
    local angle = math.atan2(y - offset_center_leds_y, x - offset_center_leds_x)
    local hue = (angle + t) % (2 * math.pi) -- Rotate through hues over time

    local r, g, b = hsvToRgb(hue, 1, 1)

    return r, g, b
end

function hsvToRgb(h, s, v)
    local c = v * s
    local x = c * (1 - math.abs((h / (math.pi / 3)) % 2 - 1))
    local m = v - c

    local r, g, b

    if h >= 0 and h < math.pi / 3 then
        r, g, b = c, x, 0
    elseif h >= math.pi / 3 and h < 2 * math.pi / 3 then
        r, g, b = x, c, 0
    elseif h >= 2 * math.pi / 3 and h < math.pi then
        r, g, b = 0, c, x
    elseif h >= math.pi and h < 4 * math.pi / 3 then
        r, g, b = 0, x, c
    elseif h >= 4 * math.pi / 3 and h < 5 * math.pi / 3 then
        r, g, b = x, 0, c
    else
        r, g, b = c, 0, x
    end

    return math.floor(r * 255), math.floor(g * 255), math.floor(b * 255)
end

function update_leds(time)
    for j = 0, num_leds_height - 1 do
        for i = 0, num_leds_width - 1 do
            local led_index = i + j * num_leds_width

            local r, g, b = colorWheelPattern(i, j, time)

            set_led(led_index, r, g, b)
        end
    end
end
