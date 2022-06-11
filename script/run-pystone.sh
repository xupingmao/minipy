echo "python3"
echo "-------"
python3 test/benchmark/cases/100_pystone.py
echo ""

echo "minipy"
echo "------"
./minipy test/benchmark/cases/100_pystone.py

if [ -f build/minipy-fast ] ; then
	echo ""
	echo "minipy-fast"
	echo "-----------"
	./build/minipy-fast test/benchmark/cases/100_pystone.py
fi

if [ -f build/pystone.out ] ; then
	echo ""
	echo "minipy-aot"
	echo "----------"
	./build/pystone.out
fi

echo ""
echo "tinypy"
echo "------"
./build/tinypy test/benchmark/cases/100_pystone.py