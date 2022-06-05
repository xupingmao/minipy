"""
minipy编译器
"""

import sys
import mp_encode
import mp2c

def boot():
	reader = mp_encode.ArgReader(sys.argv)
	args = dict()

	while reader.has_next():
		item = reader.next()
		if item == "-o":
			assert reader.has_next()
			args.target = reader.next()
			continue

		target = args.get("target")
		compiler = mp2c.AotCompiler()
		compiler.compile(item, target)

