try:
    import os
    system = os.system
except e:
    pass
os.system("pause")
system("del *.pyc, *.out, bin")