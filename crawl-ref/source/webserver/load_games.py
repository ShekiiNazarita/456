import collections
import logging
import os
from typing import Dict
from typing import List
from typing import Union

import yaml

GameDefinition = Dict[str, Union[str, bool, List[str], Dict[str, str]]]
GamesConfig = Dict[str, GameDefinition]


def load_games(existing_games):  # type: (GamesConfig) -> GamesConfig
    """
    Load game definitions from games.d/*.yaml and merge with existing config.

    This function will be called on startup and every time the webserver is sent
    a USR1 signal (eg `kill -USR1 <pid>`). This allows dynamic modification of
    the games list without interrupting active game sessions.

    The format of the source YAML files is: `games: [<game>, ...]`. Each game is
    a dictionary matching the format of entries in the `games` variable above,
    with an extra key `id` for the game's ID. Directory paths support %n as
    described above.

    This function will only add or update games. It doesn't support removing or
    reordering games. If you want to add support for either, read the caveats
    below and please contribute the improvement as a pull request!

    Dynamic update caveats:

    1. The main use-case is to support adding new game modes to a running
       server. Other uses (like modifying or removing an existing game mode, or
       re-ordering the games list) may work, but are not well tested.
    2. If you do modify a game entry, the changed settings only affect new game
       sessions (including new spectators). Existing sessions use the old config
       until they close.
    3. Some settings affect spectators. If you modify a running game's config,
       the mismatch of settings between the player and new spectators might
       cause spectating to fail until the player restarts with new settings.
    """
    conf_subdir = "games.d"
    base_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), conf_subdir)
    delta = collections.OrderedDict()
    delta_messages = []
    new_games = collections.OrderedDict()
    new_games.update(existing_games)
    for file_name in sorted(os.listdir(base_path)):
        path = os.path.join(base_path, file_name)
        if not file_name.endswith('.yaml') and not file_name.endswith('.yml'):
            logging.warn("Skipping non-yaml file %s", file_name)
            continue
        try:
            with open(path) as f:
                yaml_text = f.read()
        except OSError as e:
            logging.warn("Couldn't read file %s: %s", path, e)
        try:
            data = yaml.safe_load(yaml_text)
        except yaml.YAMLError as e:
            logging.warning("Failed to load games from %s, skipping (parse failure: %s).",
                            file_name, e)
            continue
        if data is None or 'games' not in data:
            logging.warning("Failed to load games from %s, skipping (no 'games' key).",
                            file_name)
            continue
        if len(data.keys()) != 1:
            extra_keys = ",".join(key for key in data.keys() if key != 'games')
            logging.warning(
              "Found extra top-level keys '%s' in %s, ignoring them"
              " (only 'games' key will be parsed).",
              extra_keys, file_name)
        logging.info("Loading data from %s", file_name)
        for game in data['games']:
            if not validate_game_dict(game):
                continue
            game_id = game['id']
            if game_id in delta:
                logging.warning(
                  "Game %s from %s was specified in an earlier config file, skipping.",
                  game_id, path)
            delta[game_id] = game
            action = "Updated" if game_id in existing_games else "Loaded"
            msg = ("%s game config %s (from %s).", action, game_id, file_name)
            delta_messages.append(msg)
    if delta:
        assert len(delta.keys()) == len(delta_messages)
        logging.info("Updating live games config")
        new_games.update(delta)
        for message in delta_messages:
            logging.info(*message)
    return new_games


def validate_game_dict(game):
    """Validate a game dictionary.

    Log warnings about issues and return validity.
    """
    if 'id' not in game:
        # Log the full game definition to help identify the game
        logging.warn("Missing 'id' from game definition %r", game)
        return False
    found_errors = False
    required = ('id', 'name', 'crawl_binary', 'rcfile_path', 'macro_path',
                'morgue_path', 'inprogress_path', 'ttyrec_path',
                'socket_path', 'client_path')
    optional = ('dir_path', 'cwd', 'morgue_url', 'milestone_path',
                'send_json_options', 'options', 'env', 'separator')
    boolean = ('send_json_options',)
    string_array = ('options',)
    string_dict = ('env', )
    for prop in required:
        if prop not in game:
            found_errors = True
            logging.warn("Missing required property '%s' in game '%s'",
                         prop, game['id'])
    for prop, value in game.items():
        expected = prop in required or prop in optional
        if not expected:
            # Don't count this as an error
            logging.warn("Unknown property '%s' in game '%s'",
                         prop, game['id'])
        if prop in boolean:
            if not isinstance(value, bool):
                found_errors = True
                logging.warn("Property '%s' value should be boolean in game '%s'",
                             prop, game['id'])
        elif prop in string_array:
            if not isinstance(value, list):
                found_errors = True
                logging.warn("Property '%s' value should be list of strings in game '%s'",
                             prop, game['id'])
                continue
            for item in value:
                if not isinstance(item, str):
                    found_errors = True
                    logging.warn(
                      "Item '%s' in property '%s' should be a string in game '%s'",
                      item, prop, game['id'])
        elif prop in string_dict:
            if not isinstance(value, dict):
                found_errors = True
                logging.warn(
                  "Property '%s' value should be a map of string: string in game '%s'",
                  prop, game['id'])
                continue
            for item_key, item_value in value.items():
                if not isinstance(item_key, str):
                    found_errors = True
                    logging.warn(
                      "Item key '%s' in property '%s' should be a string in game '%s'",
                      item_key, prop, game['id'])
                if not isinstance(item_value, str):
                    found_errors = True
                    logging.warn("Item value '%s' of key '%s' in property '%s'"
                                 " should be a string in game '%s'",
                                 item_value, item_key, prop, game['id'])
        else:
            # String property
            if not isinstance(value, str):
                found_errors = True
                logging.warn("Property '%s' value should be string in game '%s'",
                             prop, game['id'])
    return not found_errors
