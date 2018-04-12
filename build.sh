prog=python

case $1 in
    "mp" )
    echo "use minipy to compile compiler"
    mv minipy src/python/mp
    prog=./mp
    chmod +x src/python/mp
    ;;
esac

pushd src/python
rm ../bin.c

echo "current path: $(pwd)"
echo "use python interpreter: $prog"

set -x
$prog encode.py init.py >> bin.c
$prog encode.py lex.py >> bin.c
$prog encode.py parse.py >> bin.c
$prog encode.py tmcode.py >> bin.c
$prog encode.py encode.py >> bin.c
$prog encode.py pyeval.py >> bin.c
$prog encode.py repl.py >> bin.c
mv bin.c ../
popd

make debug
set +x