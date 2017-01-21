#!/usr/bin/python

names = ["top", "top-left", "top-right", "middle", "bottom-left", "bottom-right", "bottom"]
numbers = [["top", "top-left", "top-right", "bottom-left", "bottom-right", "bottom"],
           ["top-right", "bottom-right"],
           ["top", "top-right", "middle", "bottom-left", "bottom"],
           ["top", "top-right", "middle", "bottom-right", "bottom"],
           ["top-left", "top-right", "middle", "bottom-right"],
           ["top", "top-left", "middle", "bottom-right", "bottom"],
           ["top", "top-left", "middle", "bottom-left", "bottom-right", "bottom"],
           ["top", "top-right", "bottom-right"],
           ["top", "top-left", "top-right", "middle", "bottom-left", "bottom-right", "bottom"],
           ["top", "top-left", "top-right", "middle", "bottom-right"]]

def main():
    values = {}
    for name in names:
        values[name] = input("Insert the bit value representing the %s: " % name)
    print("The following is a comma delimited hex string representing 0-9")
    final = []
    for number in numbers:
        total = 0
        for i in number:
            total += values[i]
        final.append(total)
    print(', '.join('{}'.format(hex(v)) for v in final))
    raw_input()

if __name__ == "__main__":
    main();
