# Convert values copied from the official bluerobotics spreadsheet into one big C++ string literal

fullpath: str = "BESCR3_T200_{VOLTAGE}v.ini"
voltage: str = input("voltage ->")

fullpath = fullpath.replace("{VOLTAGE}", voltage)

buf: list[str] = []

with open(fullpath, "r") as file:
    buf = file.readlines()

with open(fullpath, "w") as file:
    mod_buf: list[str] = []

    for line in buf:
        mod_buf.append("\"" + line.strip() + "\\n\"\n")

    for line in mod_buf:
        print(line)

    file.writelines(mod_buf)
