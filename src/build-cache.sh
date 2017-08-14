pushd .

cd python
rm cache_bin.c


python cache_encode.py init.py >> cache_bin.c
python cache_encode.py lex.py >> cache_bin.c
python cache_encode.py parse.py >> cache_bin.c
python cache_encode.py tmcode.py >> cache_bin.c
python cache_encode.py cache_encode.py >> cache_bin.c
python cache_encode.py pyeval.py >> cache_bin.c
python cache_encode.py repl.py >> cache_bin.c

mv cache_bin.c ../
popd
gcc -DTM_USE_CACHE -DTM_PRINT_STEPS -o minipy main.c

