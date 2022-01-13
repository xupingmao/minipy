try:
    print("OK")
except:
    print("try 1 end")


try:
    raise "Exception 1"
except Exception as e:
    print("exception", e)

try:
    raise "Exception 2"
except Exception:
    print("exception")
