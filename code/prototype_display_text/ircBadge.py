import testBluetooth
import irc.bot

class BadgeBot(irc.bot.SingleServerIRCBot):
    def __init__(self, channel, nickname, server, port=6667):
        irc.bot.SingleServerIRCBot.__init__(self, [(server, port)], nickname, nickname)
        self.channel = channel

    def on_nicknameinuse(self, c, e):
        c.nick(c.get_nickname() + "_")

    def on_welcome(self, c, e):
        c.join("#" + self.channel)

    def on_pubmsg(self, c, e):
        text = e.source.nick + " : " + e.arguments[0] + "\n"
        #print(e.source.nick)
        #print(e.arguments)
        testBluetooth.sendText(text)

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
