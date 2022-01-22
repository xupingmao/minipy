
# set -x

this_cwd=$(pwd)
prog=python

# 检查python命令是否存在
type python 1>/dev/null;
if [ $? -ne 0 ]; then
	prog=python3
fi

case $1 in
    "mp" )
    echo "use minipy to compile compiler"
    mv minipy build/
    prog=../../build/minipy
    ;;
esac

cd src/python
rm ../bin.c

echo "Current path: $(pwd)"
echo "Python interpreter: $prog"

$prog mp_encode.py mp_init.py >> bin.c
$prog mp_encode.py mp_tokenize.py >> bin.c
$prog mp_encode.py mp_parse.py >> bin.c
$prog mp_encode.py mp_encode.py >> bin.c
$prog mp_encode.py mp_opcode.py >> bin.c
$prog mp_encode.py pyeval.py >> bin.c
$prog mp_encode.py repl.py >> bin.c

if [ $? -ne 0 ]; then
	echo "编译失败"
fi

mv bin.c ../

cd $this_cwd

make && make test

set +x