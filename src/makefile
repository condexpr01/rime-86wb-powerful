
all:
	cmake -S . -B build -G Ninja
	-cmake -B build -DCMAKE_INSTALL_PREFIX=..
	ninja -C build

	-cmake --install build

	-cp build/compile_commands.json ../src/binsrc/
	-cp build/compile_commands.json ../src/cppcore/

run:
	-../bin/cppt

clean:
	-rm -rf build

	-rm -rf ../lib

	-rm ../bin/encoder
	-rm ../bin/encoder-with-filter

	-rm ../bin/get-freq
	-rm ../bin/get-freq-with-filter

	-rm ../bin/word-set-intersect
	-rm ../bin/word-set-unite
	-rm ../bin/word-set-difference

	-rm ../bin/codec-set-intersect
	-rm ../bin/codec-set-unite
	-rm ../bin/codec-set-difference
	-rm ../bin/codec-seq-sort

	-rm ../bin/pinyin-normalize

	-rm ../bin/freq-sort

	-rm ../bin/make-seq
	-rm ../bin/make-seq-wbfilter

	#-rm ../bin/cppt

	-rm ../src/binsrc/compile_commands.json
	-rm -rf ../src/binsrc/.cache

	-rm ../src/cppcore/compile_commands.json
	-rm -rf ../src/cppcore/.cache

.PHONY: all run clean




