import crypt
import logging
import os.path
import random
import re
import sqlite3
import time

from conf import config

def user_passwd_match(username, passwd):
    """Return the correctly cased username."""
    try:
        passwd = passwd[0:config.max_passwd_length]
    except:
        return None

    try:
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()
        c.execute("select username,password from dglusers where username=? "
                  "collate nocase", (username,))
        result = c.fetchone()

        if result is None:
            return None
        elif crypt.crypt(passwd, result[1]) == result[1]:
            return result[0]
    finally:
        if c: c.close()
        if conn: conn.close()

def ensure_user_db_exists():
    if not os.path.exists(config.password_db):
        logging.warn("User database didn't exist; creating it now.")
        c = None
        conn = None
        try:
            conn = sqlite3.connect(config.password_db)
            c = conn.cursor()
            c.execute("CREATE TABLE dglusers (id integer primary key, username "
                      "text, email text, env text, password text, flags "
                      "integer);")
            conn.commit()
        finally:
            if c: c.close()
            if conn: conn.close()

    c = None
    conn = None
    try:
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()
        c.execute("CREATE TABLE IF NOT EXISTS login_tokens (username text, "
                  "seqid text, token text, expires integer, PRIMARY KEY "
                  "(username, seqid));")
        conn.commit()
    finally:
        if c: c.close()
        if conn: conn.close()


saltchars = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"

def make_salt(saltlen):
    return "".join(random.choice(saltchars) for x in xrange(0,saltlen))

def get_salt(passwd):
    algo = config.get("crypt_algorithm", "6")
    if algo:
        salt = "${0}${1}$".format(algo, make_salt(config.crypt_salt_length))
    else:
        salt = make_salt(2)
    return salt

def username_exists(username, cursor):
    """Return True if given username exists or is illegal."""
    if not username:
        return True
    if len(username) < config.min_username_length:
        return True
    cursor.execute("select username from dglusers where username=? collate "
                   "nocase", (username,))
    result = cursor.fetchone()
    if result:
        return True

def find_unused_guest_name():
    """Return an unused guest name or None if we can't."""
    try:
        name = None
        loops = 0
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()
        while username_exists(name, c):
            loops += 1
            if loops == 10:
                logging.warning("Couldn't find an unused guest username. "
                                "(hint: try increasing guest_name_suffix_len)")
                name = None
                break
            name = config.guest_name_prefix
            for _ in range(config.guest_name_suffix_len):
                name += random.choice('abcdefghijklmnopqrstuvwxyz0123456789')
    finally:
        if c: c.close()
        if conn: conn.close()

    return name

def register_user(username, passwd, email):
    '''Register a new user account.

    Returns None on success or error string.
    Set passwd to None to disable password authentication.
    Note: email is not checked for validity.'''
    if not passwd:
        return "The password can't be empty!"
    passwd = passwd[0:config.max_passwd_length]
    username = username.strip()
    if len(username) < config.min_username_length:
        return "Username too short."
    if not re.match(config.nick_regex, username):
        return "Invalid username!"

    if not config.get('dev_nicks_can_be_registered') \
       and config.get_devname(username):
        return "Reserved username!"

    if passwd:
        salt = get_salt(passwd)
        crypted_pw = crypt.crypt(passwd, salt)
        # crypt.crypt can fail if you pass in a salt it doesn't like
        if not crypted_pw:
            logging.warning("User registration failed; crypt_algorithm setting? "
                            "Tested salt was '{0}'".format(salt))
            return "Internal registration error." # deliberately generic
    else:
        crypted_pw = 'No Password'

    try:
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()

        if username_exists(username, c):
            return "User already exists!"

        c.execute("insert into dglusers(username, email, password, flags, env) "
                  "values (?,?,?,0,'')", (username, email, crypted_pw))

        conn.commit()

        return None
    finally:
        if c: c.close()
        if conn: conn.close()

def set_password(username, passwd): # Returns True or False
    if passwd == "": return False
    passwd = passwd[0:config.max_passwd_length]

    salt = get_salt(passwd)
    crypted_pw = crypt.crypt(passwd, salt)

    try:
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()

        c.execute("update dglusers set password=? where username=?",
                  (crypted_pw, username))

        conn.commit()

        return True
    finally:
        if c: c.close()
        if conn: conn.close()

sessions = {}
rand = random.SystemRandom()

def pack_sid(sid):
    return "{0:x}".format(sid)

def session_info(sid):
    s = sessions.get(sid)
    if (s is None or
        s["expires"] < time.time() or
        s["forceexpires"] < time.time()):
        return None
    return s

def renew_session(sid):
    if sid not in sessions: return False
    sessions[sid]["expires"] = (time.time() + 5*60)
    return True

def new_session():
    sid = pack_sid(rand.getrandbits(128))
    sessions[sid] = {"forceexpires": time.time()
                     + config.get("session_lifetime", 24*60*60)}
    renew_session(sid)
    return sid, sessions[sid]

def delete_session(sid):
    if sid in sessions:
        del sessions[sid]

def purge_login_tokens():
    c = None
    conn = None
    try:
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()
        c.execute("DELETE FROM login_tokens WHERE expires<?",
                  (int(time.time()),))
        conn.commit()
    finally:
        if c: c.close()
        if conn: conn.close()

    for sid in sessions:
        session = sessions[sid]
        if (session["expires"] < time.time() or
            session["forceexpires"] < time.time()):
            del sessions[sid]

def delete_logins(username):
    c = None
    conn = None
    try:
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()
        c.execute("DELETE FROM login_tokens WHERE username=?", (username,))
        conn.commit()
    finally:
        if c: c.close()
        if conn: conn.close()

    # This is a python3-compatible way to list copy the items so we can delete
    # keys.
    for sid, session in list(sessions.items()):
        if session.get("username") == username:
            del sessions[sid]

def token_login(cookie, logger=logging):
    try:
        username, token, seqid = cookie.split(":")
    except ValueError:
        return None, None
    c = None
    conn = None
    try:
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()
        c.execute("SELECT token, expires FROM login_tokens WHERE username=? "
                  "AND seqid=?", (username, seqid))
        result = c.fetchone()
        c.execute("DELETE FROM login_tokens WHERE username=? AND seqid=?",
                  (username, seqid))
        conn.commit()
    finally:
        if c: c.close()
        if conn: conn.close()

    if result:
        true_token, expires = result
        if expires < time.time():
            return None, None

        if true_token == token:
            logger.info("User {0} logged in (via token).".format(username))
            return username, get_login_cookie(username, seqid=seqid)
        else:
            logger.warning("Bad login token for user {0}!".format(username))
            delete_logins(username)
            return None, None
    else:
        return None, None

def get_login_cookie(username, seqid=None):
    token = pack_sid(rand.getrandbits(128))
    if seqid is None: seqid = pack_sid(rand.getrandbits(128))
    expires = int(time.time()) + config.login_token_lifetime*24*60*60
    c = None
    conn = None
    try:
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()
        c.execute("INSERT INTO login_tokens(username, seqid, token, expires) "
                  "VALUES (?,?,?,?)", (username, seqid, token, expires))
        conn.commit()
    finally:
        if c: c.close()
        if conn: conn.close()
    cookie = "{0}:{1}:{2}".format(username, token, seqid)
    return cookie, expires

def forget_login_cookie(cookie):
    username, token, seqid = cookie.split(":")
    c = None
    conn = None
    try:
        conn = sqlite3.connect(config.password_db)
        c = conn.cursor()
        c.execute("DELETE FROM login_tokens WHERE username=? AND seqid=?",
                  (username, seqid))
        conn.commit()
    finally:
        if c: c.close()
        if conn: conn.close()
