# build script for minipy
# 注意：如果在Mac系统上报错，需要转换成Unix的换行模式（就是把\r删掉）
this_cwd=$(pwd)
prog=python

# check python command
type python 1>/dev/null;

if [ $? -ne 0 ]; then
    prog=python3
fi

case $1 in
    "mp")
    echo "use minipy to compile compiler"
    mv minipy build/
    prog=../../build/minipy
    ;;
esac

cd src/python

echo "Current path: $(pwd)"
echo "Python interpreter: $prog"

$prog mp_encode.py mp_init.py > ../gen/mp_init.gen.c
$prog mp_encode.py mp_tokenize.py > ../gen/mp_tokenize.gen.c
$prog mp_encode.py mp_parse.py > ../gen/mp_parse.gen.c
$prog mp_encode.py mp_encode.py > ../gen/mp_encode.gen.c
$prog mp_encode.py mp_opcode.py > ../gen/mp_opcode.gen.c
$prog mp_encode.py pyeval.py > ../gen/pyeval.gen.c
$prog mp_encode.py repl.py > ../gen/repl.gen.c

if [ $? -ne 0 ]; then
	echo "Compile Failed"
fi

cd $this_cwd

make && make test

set +x