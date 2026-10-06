# Aetheris Lua API

Документация по Lua API чита. Скрипты лежат в папке `scripts` рядом с `cs2.exe`:

```
...\Counter-Strike Global Offensive\game\bin\win64\scripts\
```

Точный путь всегда видно в меню, вкладка **Scripts** (строка `folder: ...`) — он
читается из пути главного модуля. Папка создаётся автоматически при загрузке.
Скрипты подхватываются сами и перезагружаются при изменении файла. Список
скриптов, их включение/выключение и ошибки — в том же меню.

## Как начать

1. Создай файл `scripts/hello.lua` (именно `.lua`, расширение обязательно).
2. Напиши в нём функции `on_paint` и/или `on_createmove`:

```lua
function on_paint()
    render.text(20, 20, "hello from lua", color.red)
end
```

3. Открой меню → Scripts → убедись, что скрипт включён (клик по строке).

Скрипт перезагрузится сам, если сохранить файл заново. Кнопка **Reload** в меню
форсирует перезагрузку всех скриптов.

Если скрипт упал с ошибкой, текст ошибки появится внизу вкладки Scripts.

## Точки входа

| Функция | Когда вызывается |
|---|---|
| `on_paint()` | каждый кадр рендера |
| `on_createmove()` | каждый тик CreateMove (перед применениемcmd в игру) |

Также доступны события (см. ниже) — они вызываются в те же моменты:

```lua
events.paint:set(function() end)     -- вызовется в on_paint
events.createmove:set(function() end) -- вызовется в on_createmove

local fn = function() end
events.paint:set(fn)
events.paint:unset(fn)
```

## Глобалы

`globals` — доступ только на чтение:

| Поле | Тип | Описание |
|---|---|---|
| `globals.curtime` | number | текущее игровое время |
| `globals.tickcount` | number | текущий тик |
| `globals.tickinterval` | number | интервал тика |
| `globals.realtime` | number | реальное время |
| `globals.framecount` | number | номер кадра |
| `globals.frametime` | number | dt рендера в секундах |
| `globals.max_players` | number | количество игроков |
| `globals.is_connected` | boolean | подключён ли клиент |
| `globals.is_in_game` | boolean | валидна ли локальная сущность |

## vector

`vector(x, y, z)` создаёт вектор. Поля: `x`, `y`, `z` (или `[1]`, `[2]`, `[3]`).

| Метод | Описание |
|---|---|
| `v.x`, `v.y`, `v.z` | чтение/запись компонент |
| `v:length()` | длина |
| `v:length2d()` | длина в плоскости XY |
| `v:length_sqr()` | квадрат длины |
| `v:copy()` | копия вектора |
| `v:set(x, y, z?)` | изменить на месте |
| `v:to_screen()` | `vector(x, y)` на экране или `nil` |
| `a + b`, `a - b` | сложение/вычитание |
| `a * n`, `a / n` | умножение/деление на число |
| `-a` | отрицание |
| `tostring(v)` | `"vector(x, y, z)"` |

## color

`color(r, g, b, a?)` — цвет, компоненты `0..255`. Если `a` в диапазоне `0..1`,
считается как нормализованная альфа и умножается на 255.

Поля: `c.r`, `c.g`, `c.b`, `c.a` (чтение и запись).

Готовые цвета: `color.white`, `color.black`, `color.red`, `color.green`,
`color.blue`, `color.yellow`, `color.orange`, `color.purple`, `color.pink`,
`color.grey`/`color.gray`, `color.silver`, `color.gold`, `color.cyan`,
`color.brown`, `color.transparent`.

## render

Позицию можно передавать двумя числами (`x, y`) или одним вектором — как удобнее.

| Вызов | Описание |
|---|---|
| `render.screen_size()` | `vector(w, h)` |
| `render.camera_position()` | `vector` позиции камеры |
| `render.camera_angles()` | `vector` углов камеры |
| `render.world_to_screen(v)` | `vector(x, y)` или `nil`, если точка за камерой |
| `render.get_offscreen(v, radius_pct?)` | `x, y, angle_deg, is_offscreen` |
| `render.measure_text(text)` | `vector(w, h)` |
| `render.rect(ax, ay, bx, by, color, rounding?)` | залитый прямоугольник по двум точкам |
| `render.rect_outline(ax, ay, bx, by, color, thickness?, rounding?)` | обводка |
| `render.gradient(ax, ay, bx, by, tl, tr, br, bl)` | градиент по 4 углам |
| `render.line(x1, y1, x2, y2, color, thickness?)` | линия |
| `render.circle(cx, cy, radius, color, start_deg?, sweep_pct?, thickness?)` | дуга/круг |
| `render.circle_outline(cx, cy, radius, color, thickness?)` | окружность |
| `render.circle_3d(world_vec, color, radius, thickness?, fill?, outline?)` | круг в мире |
| `render.circle_3d_outline(world_vec, color, radius, thickness?, fill?, outline?)` | круг в мире, обводка |
| `render.text(x, y, color, flags?, ...)` | текст |
| `render.push_clip_rect(ax, ay, bx, by)` | обрезание |
| `render.pop_clip_rect()` | снять обрезание |

`render.text` принимает любое количество аргументов после флагов и склеивает их
через пробел, так что можно передавать числа и значения любых типов:

```lua
render.text(20, 50, color.white, "c", "hp:", player:get_health())
```

Флаги текста: `c` — центрировать по X, `r` — выровнять правый край по X.

## entity

| Вызов | Описание |
|---|---|
| `entity.get_local_player()` | локальный игрок или `nil` |
| `entity.get_players(enemies_only?)` | массив игроков |
| `entity.get(index, by_userid?)` | сущность по индексу entity list |

Методы объекта игрока/сущности:

| Метод | Описание |
|---|---|
| `e:is_player()` | является ли игроком |
| `e:is_enemy()` | враг ли относительно локального |
| `e:is_alive()` | жив ли |
| `e:get_index()` | индекс в entity list |
| `e:get_health()` | хп |
| `e:get_team()` | команда |
| `e:get_flags()` | флаги |
| `e:get_velocity()` | `vector` скорости |
| `e:get_speed()` | модуль скорости |
| `e:get_origin()` | `vector` позиции |
| `e:get_name()` | имя |
| `e:get_ping()` | пинг |

### Доступ к любым полям через схемы

Можно читать и писать любое поле, которое есть в схемах, по имени через `.` или
`[]`. Тип определяется по префиксу имени:

| Префикс | Тип |
|---|---|
| `m_b...` | boolean |
| `m_fl...` | float |
| `m_vec...`, `m_v...`, `m_ang...` | vector |
| `m_s...` | string |
| `m_ul...` | int64 |
| остальные `m_i...` и прочие | integer |

```lua
local ent = entity.get_local_player()

local hp = ent["m_iHealth"]
local vel = ent["m_vecAbsVelocity"]
local eye = ent["m_vecAbsOrigin"] + vector(0, 0, 64)

ent["m_flFlashDuration"] = 0.0
ent["m_vecAbsOrigin"] = vector(0, 0, 0)
```

Поиск идёт по списку классов: сначала павны
(`CCSPlayerPawn`, `C_CSPlayerPawn`, `C_BasePlayerPawn`, `C_BaseEntity`),
затем контроллеры (`CCSPlayerController`, `CBasePlayerController`). Поле ищется
в каждом классе по очереди, первое найденное и используется. Неизвестное поле
возвращает `nil` при чтении и молча игнорируется при записи.

## cvar

`cvar["name"]` возвращает объект cvar или `nil`, если такого нет.

| Метод | Описание |
|---|---|
| `cv:int(value?)` | прочитать/записать int |
| `cv:float(value?)` | прочитать/записать float |
| `cv:bool(value?)` | прочитать/записать bool |
| `cv:string(value?)` | прочитать/записать строку |
| `cv:call(...)` | выполнить команду (аргументы склеиваются через пробел) |

Если передан аргумент — значение записывается, если нет — читается.

```lua
local fov = cvar["cl_fov"]
local current = fov:int()
fov:int(120)

cvar["cl_fov"]:call()
cvar["say"]:call("hello")
```

## client

| Вызов | Описание |
|---|---|
| `client.log(...)` | вывести в лог чита (и в debug output) |
| `client.get_latency()` | пинг |
| `client.get_name()` | имя игрока |

## Устаревшее API (`aetheris.*)

Оставлено для совместимости. Работает, но новые скрипты лучше писать на
`render.*`, `entity.*` и т.д.

| Вызов | Описание |
|---|---|
| `aetheris.delta_time()` | dt |
| `aetheris.framerate()` | fps |
| `aetheris.viewport()` | `w, h` |
| `aetheris.color(r, g, b, a?)` | таблица цвета |
| `aetheris.velocity()` | `x, y, z` скорости |
| `aetheris.speed()` | скорость |
| `aetheris.health()` | хп |
| `aetheris.flags()` | флаги |
| `aetheris.alive()` | жив ли локальный |
| `aetheris.rect_filled(x, y, w, h, color)` | прямоугольник |
| `aetheris.rect(x, y, w, h, color)` | обводка |
| `aetheris.line(x1, y1, x2, y2, color)` | линия |
| `aetheris.circle(x, y, r, color)` | окружность |
| `aetheris.circle_filled(x, y, r, color)` | круг |
| `aetheris.text(x, y, text, color)` | текст |
| `aetheris.text_size(text)` | `w, h` |
| `aetheris.log(...)` | лог |
| `aetheris.config_name()` | имя активного конфига |
| `aetheris.ping()` | пинг |
| `aetheris.steam_name()` | ник из Steam |

## Готовые примеры

В папке `scripts` лежат рабочие примеры:

- `example_esp.lua` — боксы, ники, хп-бары, дистанция и оффскрин-стрелки.
- `example_chat.lua` — отправка сообщений в чат, rate-limit и лог отправленного.
- `example_hud.lua` — hud с пингом, тикрейтом, фпс и логом ошибок скрипта.

Кнопки usercmd (кнопки движения/атаки) в API не выведены, поэтому bunnyhop или
подобное напрямую из Lua недоступны — только то, что есть в списке выше.

## Замечания

- Пиши тяжёлое только в `on_paint` аккуратно: рендер и CreateMove делят один
  мьютекс движка, долгий скрипт в `on_paint` будет подтормаживать кадры.
- В `on_createmove` есть `globals.frametime`/`globals.tickinterval` для расчёта
  дельты; сам usercmd скриптам не передаётся, только событие.
- Скрипты выполняются в том же треде, что иCreateMove/рендер. Не делай бесконечных
  циклов и тяжёлых обходов entity list в горячих функциях.