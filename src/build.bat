@rem rebuild when python code changes

pushd .

cd python

python build.py

popd

gcc -o minipy.exe main.c

@rem copy minipy.exe python

pause
