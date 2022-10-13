# ### Server Operators ###
# If you want to customise any settings in this file, you'll need to work around
# the fact that is version controlled. Some options:
# 1. Use a local commit and always do a rebase pull on top of it. (This may
#    affect versioning, though.)
# 2. Create config.yml and write your overrides in there.
# 3. Use a more complicated scripting system; for example, dgamelaunch-config
#    supports templating this file and automatically handles merging the
#    server config with the repository files.
#
# ### Developers ###
# Warning! Servers will not update or merge with the version controlled copy of
# this file, so any parameters here should be presented as recommendations or
# documentation of the default, not a default value, and option name changes
# here will require manual intervention on the part of server admins. New
# values should always come with a default that does not lead to crashes, so
# that something sensible happens on DGL upgrade.
#
# To add new default values, see the `webtiles.config` module. Note that
# webtiles.config.get(x) returns `None` if `x` is not present in the config
# data, so this can be taken as a default default value.
#
# Bad assumptions about config defaults particularly impacts templated html
# files, which are loaded and called dynamically, so *do* get updated
# immediately on a rebuild (unlike python code in this module or
# webtiles.config). If something like client.html raises an exception, this
# will trigger 500 errors across the whole server.

import logging
import os

import yaml

# Where to look for `games.d/`, `config.yml`, and other things.
server_path = os.path.dirname(os.path.abspath(__file__))

# dgl_mode = True

bind_nonsecure = True # Set to false to only use SSL
bind_address = ""
bind_port = 8080
# Or listen on multiple address/port pairs (overriding the above) with:
# bind_pairs = (
#     ("127.0.0.1", 8080),
#     ("localhost", 8082),
#     ("", 8180), # All addresses
# )

# to log to a file directly, uncomment and add something like:
#    "filename": "webtiles.log",
# logging_config = {
#     "level": logging.INFO,
#     "format": "%(asctime)s %(levelname)s: %(message)s"
# }

# sometimes useful for debugging:
# logging.getLogger('asyncio').setLevel(logging.DEBUG)

password_db = "./webserver/passwd.db3"
# Uncomment and change if you want this db somewhere separate from the
# password_db location.
#settings_db = "./webserver/user_settings.db3"

static_path = "./webserver/static"
template_path = "./webserver/templates/"

# Path for server-side unix sockets (to be used to communicate with crawl)
# server_socket_path = None # None: Uses global temp dir

# Server name, so far only used in the ttyrec metadata
server_id = ""

# Disable caching of game data files
game_data_no_cache = True

# Watch socket dirs for games not started by the server
# watch_socket_dirs = False

# Game configs
# You can define game configs in two ways:
# 1. As *.yml files in `games.d/`. (default; see games.d/base.yml)
# 2. With a dictionary `games` in this file (not recommended except for dgamelaunch-config servers).

# to override 1, simply set `games` in this file to something non-empty; see
# the below example. To force using both, set `use_game_yaml` to True. Games
# defined in this file will precede games defined in games.d.
# use_game_yaml = True

# Example of defining `games` directly via a dictionary.
# This setting to be an OrderedDict pre python 3.6, so this example uses that
# for compatibility:

# import collections
# games = collections.OrderedDict([
#     ("dcss-web-trunk", dict(
#         name = "Play trunk",
#         crawl_binary = "./crawl",
#         rcfile_path = "./rcs/",
#         macro_path = "./rcs/",
#         morgue_path = "./rcs/%n",
#         inprogress_path = "./rcs/running",
#         ttyrec_path = "./rcs/ttyrecs/%n",
#         socket_path = "./rcs",
#         client_path = "./webserver/game_data/",
#         # dir_path = ".",
#         # cwd = ".",
#         morgue_url = None,
#         show_save_info = True,
#         allowed_with_hold = True,
#         # milestone_path = "./rcs/milestones",
#         send_json_options = True,
#         # env = {"LANG": "en_US.UTF8"},
#         )),
# ])

dgl_status_file = "./rcs/status"

# Extra paths to tail for milestone updates. This is a legacy setting, you
# should use `milestone_path` or `dir_path` for each game in the games dict.
# (This setting can be a string or list of strings.)
# milestone_file = ["./milestones"]

# status_file_update_rate = 5
# lobby_update_rate = 2

# recording_term_size = (80, 24)

# max_connections = 100

# Script to initialize a user, e.g. make sure the paths
# and the rc file exist. This is not done by the server
# at the moment. This value
init_player_program = "./util/webtiles-init-player.sh"

ssl_options = None # No SSL
#ssl_options = {
#    "certfile": "./webserver/localhost.crt",
#    "keyfile": "./webserver/localhost.key"
#}
ssl_address = ""
ssl_port = 8081
# Or listen on multiple address/port pairs (overriding the above) with:
# ssl_bind_pairs = (
#     ("127.0.0.1", 8081),
#     ("localhost", 8083),
# )

# connection_timeout = 600
# max_idle_time = 5 * 60 * 60

# use_gzip = True

# Seconds until stale HTTP connections are closed
# This needs a patch currently not in mainline tornado.
# http_connection_timeout = None

# Set this to true if you are behind a reverse proxy
# Your proxy must set header X-Real-IP
#
# Enabling this option when webtiles is NOT protected behind a reverse proxy
# introduces a security risk. An attacker could inject a false address into the
# X-Real-IP header. Do not enable this option if the webtiles server is
# directly exposed to users.
# http_xheaders = None

# kill_timeout = 10 # Seconds until crawl is killed after HUP is sent

# a nick is allowed if it matches this regex
# nick_regex = r"^[a-zA-Z0-9]{3,20}$"
# max_passwd_length = 20

# or you can define a function that returns true on an acceptable nick:
# def nick_check_fun(s):
#     return s != "plog"

# Set to True to allow users to request a password reset email. Some settings
# must be properly configured for this to work:
# allow_password_reset = False
# Set to True to allow dgl admin users to generate password reset tokens in the
# admin panel. Only use if you really trust your admin users!
# admin_password_reset = False

# Set to the primary URL where a player would reach the main lobby
# For example: "http://crawl.akrasiac.org/"
# This is required for for password reset, as it will be the base URL for
# recovery URLs. Use "http://localhost:8080/" for testing.
lobby_url = None

# Proper SMTP settings are required for password reset to function properly.
# if smtp_host is anything other than `localhost`, you may need to adjust the
# timeout settings (see server.py, calls to ioloop.set_blocking_log_threshold).
# TODO: set_blocking_log_threshold is deprecated in tornado 5+...
# Ideally, test out these settings carefully in a non-production setting
# before enabling this, as there's a bunch of ways for this to go wrong and you
# don't want to get your SMTP server blacklisted.
smtp_host = "localhost"
smtp_port = 25
smtp_use_ssl = False
smtp_user = "" # set to None for no auth
smtp_password = ""
smtp_from_addr = "noreply@crawl.example.org" # The address from which automated
                                             # emails will be sent

# crypt() algorithm, e.g. "1" for MD5 or "6" for SHA-512; see crypt(3). If
# false, use traditional DES (but then only the first eight characters of the
# password are significant). If set to "broken", use traditional DES with
# the password itself as the salt; this is necessary for compatibility with
# dgamelaunch, but should be avoided if possible because it leaks the first
# two characters of the password's plaintext.
# crypt_algorithm = "broken"

# The length of the salt string to use. If crypt_algorithm is false, this
# setting is ignored and the salt is two characters.
# crypt_salt_length = 16

# login_token_lifetime = 7 # Days

uid = None  # If this is not None, the server will setuid to that (numeric) id
gid = None  # after binding its sockets.

umask = None # e.g. 0077

chroot = None

pidfile = None
# daemon = False # If true, the server will detach from the session after startup

# Set to a URL with %s where lowercased player name should go in order to
# hyperlink WebTiles spectator names to their player pages.
# For example: "http://crawl.akrasiac.org/scoring/players/%s.html"
# Set to None to disable player page hyperlinks
player_url = None

# set one of these for various moderation modes. Disabled preempts hold. In
# account hold mode, new accounts cannot use chat, cannot spectate, and do
# not appear in the lobby until explicitly approved by an admin. They can still
# play. (Of course, they can still log out and spectate as anon.)
# new_accounts_disabled = True
# new_accounts_hold = True

# If set to True, a SIGHUP triggers an attempt to reload the config and game
# data. Some values cannot be reloaded (including this one), and to reset a
# value to its default, you need to explicitly set the value rather than
# comment it out.
# If not explicitly set, this defaults to False.
hup_reloads_config = True

# set to do periodic logging of user load
# load_logging_rate = 10 # seconds, set to 0 to explicitly disable
# slow_callback_alert = 0.25 # seconds, set to None to explicitly disable

# Only for development:
# This is insecure; do not set development_mode = True in production!
# development_mode = False

# Disable caching of static files which are not part of game data.
# no_cache = development_mode
# Automatically log in all users with the username given here.
autologin = None
