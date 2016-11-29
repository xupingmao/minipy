pushd .

cd python

rm cache_bin.c
python cache_encode.py init.py >> cache_bin.c
python cache_encode.py tokenize.py >> cache_bin.c
python cache_encode.py parse.py >> cache_bin.c
python cache_encode.py tmcode.py >> cache_bin.c
python cache_encode.py cache_encode.py >> cache_bin.c
python cache_encode.py pyeval.py >> cache_bin.c
python cache_encode.py repl.py >> cache_bin.c

mv cache_bin.c ../

popd

gcc -o cachepy cache_main.c

pause