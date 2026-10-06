-- example_hud.lua
-- Небольшой hud: пинг, тикрейт, фпс, позиция, локальный игрок.
-- Включай/выключай в меню -> Scripts.

local settings = {
    show_ping = true,
    show_tickrate = true,
    show_framerate = true,
    show_position = true,
    show_name = true,
    x = 20,
    y = 20,
}

local colors = {
    label = color(150, 150, 150),
    value = color.white,
    good = color(84, 176, 96),
    warn = color(212, 176, 84),
    bad = color(196, 84, 84),
}

local function ping_color(ping)
    if ping < 60 then
        return colors.good
    elseif ping < 140 then
        return colors.warn
    end
    return colors.bad
end

local function tickrate_color(tickrate)
    if tickrate >= 64 then
        return colors.good
    end
    return colors.warn
end

local function row(y, label, value, value_color)
    render.text(settings.x, y, colors.label, "", label)
    render.text(settings.x + 90, y, value_color or colors.value, "", tostring(value))
    return y + 16
end

function on_paint()
    local w, h = render.screen_size().x, render.screen_size().y

    if not globals.is_in_game then
        render.text(settings.x, settings.y, colors.label, "",
            "aetheris lua hud", "waiting for game")
        return
    end

    local y = settings.y

    if settings.show_ping then
        local ping = client.get_latency()
        y = row(y, "ping", ping .. " ms", ping_color(ping))
    end

    if settings.show_tickrate then
        local interval = globals.tickinterval
        local tickrate = interval > 0 and math.floor(1 / interval + 0.5) or 0
        y = row(y, "tickrate", tickrate .. " tick", tickrate_color(tickrate))
    end

    if settings.show_framerate then
        y = row(y, "framerate", string.format("%.0f fps", aetheris.framerate()))
    end

    local me = entity.get_local_player()
    if me ~= nil then
        if settings.show_name then
            y = row(y, "name", me:get_name())
        end

        if settings.show_position then
            local origin = me["m_vecAbsOrigin"]
            if origin ~= nil then
                y = row(y, "position", string.format("%.0f %.0f %.0f", origin.x, origin.y, origin.z))
            end
        end

        local velocity = me:get_velocity()
        y = row(y, "speed", string.format("%.1f u/s", velocity:length2d()))
    end

    render.rect_outline(settings.x - 6, settings.y - 6, settings.x + 210, y + 2, color(60, 60, 60), 1.0, 6.0)
end