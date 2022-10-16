if UiContainer == nil then UiContainer = {} end

print("ADD helpers")

--- Populates a character data.
--
-- @param widget Name of the widget with the character data.
-- @param character The character to get the data from. If nill, the character details will be
-- hidden.
UiContainer.populate_character_data = function(widget, character)
    if character == nil then
        ui_manager:get_widget(widget):set_visible(false)
        return 0
    end

    -- TODO: When MAX HP and MP values are fixed, use them instead of base.

    local char_id = character.char_id
    ui_manager:get_widget(widget):set_visible(true)

    -- Critical?
    if character.stats.hp.current == 0 then
        ui_manager:get_widget(widget .. ".Data.HpCurrent"):set_colour(1.0, 0, 0)
    elseif character.stats.hp.current <= character.stats.hp.base / 4 then
        ui_manager:get_widget(widget .. ".Data.HpCurrent"):set_colour(1.0, 1.0, 0)
    else
        ui_manager:get_widget(widget .. ".Data.HpCurrent"):set_colour(0.9, 0.9, 0.9)
    end

    -- Text data
    ui_manager:get_widget(widget .. ".Data.Name"):set_text(character.name)
    ui_manager:get_widget(widget .. ".Data.LvNumber"):set_text(tostring(character.level))
    ui_manager:get_widget(widget .. ".Data.HpCurrent"):set_text(tostring(character.stats.hp.current))
    ui_manager:get_widget(widget .. ".Data.HpMax"):set_text(tostring(character.stats.hp.base))
    ui_manager:get_widget(widget .. ".Data.MpCurrent"):set_text(tostring(character.stats.mp.current))
    ui_manager:get_widget(widget .. ".Data.MpMax"):set_text(tostring(character.stats.mp.base))

    -- Calculate the HP and MP bar.
    local max_hp_width= ui_manager:get_widget(widget .. ".Data.HpLine"):get_width()
    local hp_width = character.stats.hp.current * max_hp_width / character.stats.hp.base
    ui_manager:get_widget(widget .. ".Data.HpLineCurrent"):set_width(hp_width)
    local max_mp_width= ui_manager:get_widget(widget .. ".Data.MpLine"):get_width()
    local mp_width = character.stats.mp.current * max_mp_width / character.stats.mp.base
    ui_manager:get_widget(widget .. ".Data.MpLineCurrent"):set_width(mp_width)

    -- Status (Fury/Sadness)
    local fury = false
    local sadness = false
    for _, st in ipairs(character.status) do
        if st == 4 then
            sadness = true
            break
        elseif st == 5 then
            fury = true
            break
        end
    end
    if fury == true then
        ui_manager:get_widget(widget .. ".Data.Status"):set_colour(0.8, 0, 1)
        ui_manager:get_widget(widget .. ".Data.Status"):set_text("Fury")
    elseif sadness == true then
        ui_manager:get_widget(widget .. ".Data.Status"):set_colour(0.8, 0, 1)
        ui_manager:get_widget(widget .. ".Data.Status"):set_text("Sadness")
    elseif sadness == false and fury == false then
        ui_manager:get_widget(widget .. ".Data.Status"):set_colour(0.8, 0, 1)
        ui_manager:get_widget(widget .. ".Data.Status"):set_text("")
    end

    return 0
end
