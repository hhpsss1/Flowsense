-- example_chat.lua
-- Отправка сообщений в игровой чат через cvar say + лог отправленного.
-- Включай/выклювай в меню -> Scripts.

local settings = {
    enabled = false,        -- чтобы не спамить сразу после загрузки
    messages = {
        "hello from aetheris lua",
        "gg",
        "reloading in 3",
    },
    interval = 2.5,         -- секунд между сообщениями
    join_chat = true,       -- открывать чат перед отправкой
    show_log = true,
}

local state = {
    next_send = 0,
    index = 1,
    history = {},
    max_history = 5,
    last_sent = 0,
}

local function trim_history()
    while #state.history > state.max_history do
        table.remove(state.history, 1)
    end
end

local function push_history(text)
    table.insert(state.history, text)
    trim_history()
end

local function send(text)
    if type(text) ~= "string" or text == "" then
        return
    end

    -- say_all, чтобы не зависеть от открытого чата
    cvar["say_all"]:call(text)
    push_history(text)
    state.last_sent = globals.curtime
end

local function tick_chat()
    if not settings.enabled then
        return
    end

    if #settings.messages == 0 then
        return
    end

    if globals.curtime < state.next_send then
        return
    end

    state.next_send = globals.curtime + settings.interval
    send(settings.messages[state.index])
    state.index = state.index % #settings.messages + 1
end

function on_createmove()
    tick_chat()
end

function on_paint()
    if not settings.show_log or not globals.is_in_game then
        return
    end

    local h = render.screen_size().y
    local y = h - 90

    local status = settings.enabled and "CHAT: ON" or "CHAT: OFF"
    render.text(20, y, settings.enabled and color.green or color.grey, "", status)

    local last = state.last_sent
    if last > 0 then
        local ago = math.floor(globals.curtime - last)
        render.text(20, y + 16, color(140, 140, 140), "", "last message", ago, "s ago")
    end

    for i = #state.history, 1, -1 do
        local index = #state.history - i + 1
        local line_y = y + 32 + index * 14
        if line_y > h - 10 then
            break
        end

        render.text(20, line_y, color(180, 180, 180), "", ">", state.history[ i ])
    end
end