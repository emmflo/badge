import testBluetooth
import irc.bot

class BadgeBot(irc.bot.SingleServerIRCBot):
    def __init__(self, channel, nickname, server, port=6667):
        irc.bot.SingleServerIRCBot.__init__(self, [(server, port)], nickname, nickname)
        self.channel = channel
        self.nick_color = {}

    def on_nicknameinuse(self, c, e):
        c.nick(c.get_nickname() + "_")

    def on_welcome(self, c, e):
        c.join("#" + self.channel)

    def on_pubmsg(self, c, e):
        a = e.arguments[0].split(":~", 1)
        if len(a) > 1:
            self.do_command(e, a[1].strip())
        else:
            text = e.source.nick + " : " + e.arguments[0] + "\n"
            if e.source.nick in self.nick_color:
                testBluetooth.setTextColor(*self.nick_color[e.source.nick])
            else:
                testBluetooth.setTextColor(10, 10, 10)
            testBluetooth.sendText(text)
        return

    def do_command(self, e, cmd):
        nick = e.source.nick
        c = self.connection

        cmd_split = cmd.split()
        cmd = cmd_split[0]
        args = []
        if len(cmd_split) > 1:
            args = cmd_split[1:]

        if cmd == "disconnect":
            self.disconnect()
        elif cmd == "die":
            self.die()
        elif cmd == "blue":
            testBluetooth.setTextColor(0, 0, 20)
        elif (cmd == "setTextColor" and len(args) == 3 and int(args[0]) >= 0
            and int(args[0]) <= 120 and int(args[1]) >= 0 and int(args[1]) <= 120
            and int(args[2]) >= 0 and int(args[2]) <= 120):
                #testBluetooth.setTextColor(int(args[0]), int(args[1]), int(args[2]))
                self.nick_color[nick] = (int(args[0]), int(args[1]), int(args[2]))
        elif (cmd == "setBackgroundColor" and len(args) == 3 and int(args[0]) >= 0
            and int(args[0]) <= 120 and int(args[1]) >= 0 and int(args[1]) <= 120
            and int(args[2]) >= 0 and int(args[2]) <= 120):
                testBluetooth.setBackgroundColor(int(args[0]), int(args[1]), int(args[2]))
        elif cmd == "setSleepTime" and len(args) == 1 and int(args[0]) >= 0 and int(args[0]) <= 255:
                testBluetooth.setSleepTime(int(args[0]))
        elif cmd == "help":
            c.notice(nick, ":~command args")
            c.notice(nick, ":~setTextColor r g b")
            c.notice(nick, ":~setBackgroundColor r g b")
            c.notice(nick, ":~setSleepTime time")
        else:
            c.notice(nick, "WTF:" + cmd)

def main():
    import sys
    if len(sys.argv) != 4:
        print(sys.argv)
        sys.exit(1)

    s = sys.argv[1].split(":", 1)
    server = s[0]
    if len(s) == 2:
        try:
            port = int(s[1])
        except ValueError:
            print("Blahblah")
            sys.exit(1)
    else:
        port = 6667
    channel = sys.argv[2]
    nickname = sys.argv[3]

    testBluetooth.setSleepTime(150)

    bot = BadgeBot(channel, nickname, server, port)
    bot.start()

if __name__ == "__main__":
    main()
