-- example_esp.lua
-- ESP через новый API: бокс, ник, хп-бар, дистанция, оффскрин-стрелка.
-- Включай/выключай в меню -> Scripts.

local settings = {
    box = true,
    box_color = color(84, 124, 176),
    name = true,
    name_color = color.white,
    health_bar = true,
    distance = true,
    distance_color = color(150, 150, 150),
    offscreen_arrow = true,
    arrow_color = color(255, 90, 90),
    max_distance = 0, -- 0 = без ограничения
}

-- позиция сущности + прибавка по высоте
local function origin_of(ent)
    local origin = ent["m_vecAbsOrigin"]
    if origin == nil then
        return nil
    end
    return origin
end

local function bounds_of(ent)
    local origin = ent["m_vecAbsOrigin"]
    if origin == nil then
        return nil
    end

    local eye = origin + vector(0, 0, 72)
    local top = eye + vector(0, 0, 24)
    local bottom = eye + vector(0, 0, -72)

    local top_screen = top:to_screen()
    local bottom_screen = bottom:to_screen()

    if top_screen == nil or bottom_screen == nil then
        return nil
    end

    local height = math.abs(bottom_screen.y - top_screen.y)
    local width = height * 0.55

    return {
        x = top_screen.x - width * 0.5,
        y = top_screen.y,
        w = width,
        h = height,
        cx = top_screen.x,
        cy = (top_screen.y + bottom_screen.y) * 0.5,
        head_y = top_screen.y,
    }
end

local function health_color(health)
    if health > 75 then
        return color(84, 176, 96)
    elseif health > 35 then
        return color(212, 176, 84)
    end
    return color(196, 84, 84)
end

function on_paint()
    if not globals.is_in_game then
        return
    end

    local me = entity.get_local_player()
    if me == nil then
        return
    end

    local my_origin = origin_of(me)

    for _, ent in ipairs(entity.get_players(true)) do
        if ent:is_alive() then
            local origin = origin_of(ent)
            local distance = 0

            if origin ~= nil and my_origin ~= nil then
                distance = (origin - my_origin):length2d()
            end

            local in_range = settings.max_distance <= 0 or distance <= settings.max_distance

            if in_range then
                local bounds = bounds_of(ent)

                if bounds ~= nil then
                    if settings.box then
                        render.rect_outline(
                            bounds.x, bounds.y,
                            bounds.x + bounds.w, bounds.y + bounds.h,
                            settings.box_color, 1.0)
                    end

                    if settings.health_bar then
                        local health = ent:get_health()
                        local frac = math.max(0, math.min(100, health)) / 100
                        render.rect(
                            bounds.x - 5, bounds.y + bounds.h * (1 - frac),
                            bounds.x - 2, bounds.y + bounds.h,
                            health_color(health))
                    end

                    if settings.name then
                        render.text(bounds.cx, bounds.head_y - 16, settings.name_color, "c", ent:get_name())
                    end

                    if settings.distance then
                        render.text(bounds.cx, bounds.y + bounds.h + 4,
                            settings.distance_color, "c", string.format("%dm", math.floor(distance)))
                    end
                elseif settings.offscreen_arrow then
                    local origin = origin_of(ent)
                    if origin ~= nil then
                        local sx, sy, angle, offscreen = render.get_offscreen(origin, 0.45)
                        if offscreen then
                            render.text(sx, sy, settings.arrow_color, "c", string.format("%d", math.floor(angle)))
                        end
                    end
                end
            end
        end
    end
end