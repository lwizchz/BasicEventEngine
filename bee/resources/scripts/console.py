import bee

# Make the console commands available as global functions
commands = bee.console.commands.__dict__
for k,v in commands.items():
    if k[0] == "_":
        continue
    globals()[k] = v

# Replace the displayhook inside a wrapper function to avoid polluting the console
def dh():
    import sys
    def dh(value):
        bee._displayhook(value)

        __builtins__["_"] = None
        if value is None:
            return
        __builtins__["_"] = value
    sys.displayhook = dh
dh()
del dh
