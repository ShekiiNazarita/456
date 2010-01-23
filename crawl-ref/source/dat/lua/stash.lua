---------------------------------------------------------------------------
-- stash.lua
-- Annotates items for the stash-tracker's search, and for autopickup
-- exception matches.
--
-- To use this, add this line to your init.txt:
--   lua_file = lua/stash.lua
--
-- Available annotations:
-- {artefact} for artefacts.
-- {ego} for identified branded items.
-- { <skill> } - the relevant weapon skill for weapons.
-- { <class> } - item class: gold, weapon, missile, armour, wand, carrion,
--               food, scroll, jewellery, potion, book, staff, orb, misc
-- {stick} for items suitable for "sticks to snakes"
--
-- You can optionally disable annotate items with the item class name 
-- (such as "weapon" for weapons) by setting
--   annotate_item_class = false
-- in your init.txt.
--
-- Item annotations are always prefixed to the item name. For instance:
-- {artefact} the Staff of Wucad Mu
---------------------------------------------------------------------------

local ch_annotate_item_class   = nil
local ch_annotate_item_dropped = nil

-- Annotate items for searches
function ch_stash_search_annotate_item(it)
  local annot = ""

  if ch_annotate_item_dropped == nil then
    ch_annotate_item_dropped = opt_boolean("annotate_item_dropped", false)
  end
  
  if ch_annotate_item_dropped and item.dropped(it) then
    annot = annot .. "{dropped} "
  end
  
  if item.artefact(it) then
    annot = annot .. "{artefact} "
  elseif item.branded(it) then
    annot = annot .. "{ego} "
  end

  if item.snakable(it) then
    annot = annot .. "{stick} "
  end

  local skill = item.weap_skill(it)
  if skill then
    annot = annot .. "{" .. skill .. "} "
  end

  if ch_annotate_item_class == nil then
    ch_annotate_item_class = opt_boolean("annotate_item_class", true)
  end
  
  if ch_annotate_item_class then
    annot = annot .. "{" .. item.class(it, true) .. "}"
  end

  return annot
end

--- If you want dumps (.lst files) to be annotated, uncomment this line:
-- ch_stash_dump_annotate_item = ch_stash_search_annotate_item

