pushd .

cd src/python
rm cache_bin.c

python encode.py init.py >> cache_bin.c
python encode.py lex.py >> cache_bin.c
python encode.py parse.py >> cache_bin.c
python encode.py tmcode.py >> cache_bin.c
python encode.py encode.py >> cache_bin.c
python encode.py pyeval.py >> cache_bin.c
python encode.py repl.py >> cache_bin.c
mv cache_bin.c ../
popd

make

