set -x
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

$prog mp_encode.py init.py >> bin.c
$prog mp_encode.py mp_tokenize.py >> bin.c
$prog mp_encode.py mp_parse.py >> bin.c
$prog mp_encode.py mp_encode.py >> bin.c
$prog mp_encode.py mp_opcode.py >> bin.c
$prog mp_encode.py pyeval.py >> bin.c
$prog mp_encode.py repl.py >> bin.c
mv bin.c ../
popd

make debug
set +x