import bee

commands = bee.console.commands.__dict__
for k,v in commands.items():
    if k[0] == "_":
        continue
    globals()[k] = v
