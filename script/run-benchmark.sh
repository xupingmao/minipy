echo "python3"
echo "-------"
python3 test/benchmark/cases/100_pystone.py
echo ""

echo "minipy"
echo "------"
./minipy test/benchmark/cases/100_pystone.py

echo ""
echo "tinypy"
echo "------"
./build/tinypy test/benchmark/cases/100_pystone.py