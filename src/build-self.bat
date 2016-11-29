@rem rebuild python code with minipy itself

pushd .
cd python
minipy build.py
popd

gcc -o minipy.exe main.c

pause
