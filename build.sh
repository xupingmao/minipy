prog=python

case $1 in
    "mp" )
    echo "use minipy to compile compiler"
    mv minipy src/python/mp
    prog=./mp
    ;;
esac

pushd src/python
rm ../cache_bin.c

echo "current path: $(pwd)"
echo "use python interpreter: $prog"

$prog encode.py init.py >> cache_bin.c
$prog encode.py lex.py >> cache_bin.c
$prog encode.py parse.py >> cache_bin.c
$prog encode.py tmcode.py >> cache_bin.c
$prog encode.py encode.py >> cache_bin.c
$prog encode.py pyeval.py >> cache_bin.c
$prog encode.py repl.py >> cache_bin.c
mv cache_bin.c ../
popd

make

