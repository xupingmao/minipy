pushd .

cd python
rm cache_bin.c

if exist ..\cachepy.exe goto cachepy_build

:py_build
python cache_encode.py init.py >> cache_bin.c
python cache_encode.py lex.py >> cache_bin.c
python cache_encode.py parse.py >> cache_bin.c
python cache_encode.py tmcode.py >> cache_bin.c
python cache_encode.py cache_encode.py >> cache_bin.c
python cache_encode.py pyeval.py >> cache_bin.c
python cache_encode.py repl.py >> cache_bin.c
goto gcc_build

:cachepy_build
..\cachepy cache_encode.py init.py >> cache_bin.c
..\cachepy cache_encode.py lex.py >> cache_bin.c
..\cachepy cache_encode.py parse.py >> cache_bin.c
..\cachepy cache_encode.py tmcode.py >> cache_bin.c
..\cachepy cache_encode.py cache_encode.py >> cache_bin.c
..\cachepy cache_encode.py pyeval.py >> cache_bin.c
..\cachepy cache_encode.py repl.py >> cache_bin.c

goto gcc_build


:gcc_build
mv cache_bin.c ../
popd
gcc -o cachepy cache_main.c

pause