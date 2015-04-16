#!/usr/bin/env python

from __future__ import print_function
import argparse, csv, os, os.path, signal, sys, urllib2

from conf import config, ConfigError

## set up help options
parser = argparse.ArgumentParser(description="Fetch the public Sequell nickmap "
                                 "and make a player title file for WebTiles.")
parser.add_argument("-q", dest="quiet", action="store_true", default=False,
                    help="suppress all messages but warnings/errors.")
args = parser.parse_args()

if config.get("pidfile"):
    try:
        pid_fh = open(config["pidfile"], "r")
        pid = int(pid_fh.readline())
        pid_fh.close()
    except EnvironmentError as e:
        print("Warning: Can't read WebTiles server pidfile {0} "
              "({1})!".format(config["pidfile"], e.strerror), file=sys.stderr)
        pid = None
else:
     print("Warning: No server pid file set in WebTiles configuration!",
           file=sys.stderr)

if not config.get("nickdb_url"):
    sys.exit("Sequell nickdb URL (option nickdb_url) not set in configuration!")

if config.get("player_title_file"):
    title_file = config["player_title_file"]
else:
    sys.exit("Player title file (option player_title_file) not set in"
             "configuration!")

if not config.get("title_names"):
    sys.exit("Title names (option title_names) not set in configuration!")

## Retrieve the nickdb and build a dict of the nick data for the titles we
## want.
try:
    nick_fh = urllib2.urlopen(config["nickdb_url"])
except urllib2.HTTPError as e:
    sys.exit("Unable to retreive nickdb: {0}".format(e.reasonerrmsg))

nick_r = csv.reader(nick_fh, delimiter=" ", quoting=csv.QUOTE_NONE)
titles = {}
row_count = 0
for row in nick_r:
    if row[0] in config["title_names"]:
        titles[row[0]] = row[1:]
    row_count += 1
nick_fh.close()
if not args.quiet:
    print("Fetched {0} lines from {1}".format(row_count, config["nickdb_url"]))

## Filter out the title nick data and write to the player title file
try:
    title_fh = open(title_file, "w")
except EnvironmentError as e:
    sys.exit("Unable to open player title file {0} ({1})".format(title_file,
                                                                 e.strerror))

title_w = csv.writer(title_fh, delimiter=" ", quoting=csv.QUOTE_NONE)
title_count = 0
for t in config["title_names"]:
    if t in titles:
        title_w.writerow([t] + titles[t])
        title_count += 1
title_fh.close()
if not args.quiet:
    print("Wrote {0} title entries to {1}".format(title_count,
                                                  config["player_title_file"]))

## Tell WebTiles server to reload the player titles
if pid:
    if not args.quiet:
        print("Sending SIGUSR2 to WebTiles process {0}".format(pid))
    try:
        os.kill(pid, signal.SIGUSR2)
    except OSError as e:
        print("Could not send SIGUSR2 to WebTiles process id {0} "
              "({1})".format(pid, e.strerror), file=sys.stderr)
