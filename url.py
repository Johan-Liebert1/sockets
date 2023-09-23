url = input("Enter url: ")

seps = url.split('?')

if len(seps) < 2:
    print("No query params")
    exit(0)

seps = seps[1].split('&')

d = {}

for param in seps:
    key_val = param.split('=')

    if len(key_val) < 2:
        print("Invalid query ", param)
        exit(0)

    d[key_val[0]] = key_val[1]


print(d)

# http://localhost:3000?hi=&hello=bye

