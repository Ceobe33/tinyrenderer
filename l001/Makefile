CC = g++ -ggdb -pg -O0
CPPFLAGS	 =
LDFLAGS		 =
LIBS		 = -lm

DESTDIR		= ./
BUILD_DIR	= ./build
TARGET  	= a.out
GPROF2DOT 	= /home/aaron/gprof2dot/gprof2dot.py

# $(patsubst pattern, replacement, text)
# finds words in text that match with pattern and replace then use replacement
# OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

SRCS = $(wildcard *.cpp)
OBJECTS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)
# 变量 $@ 表示target
# 变量 $< 表示第一个依赖项
# 变量 $^ 表示所有依赖项

# all: $(DESTDIR)$(TARGET)
#
# $(DESTDIR)$(TARGET): $(OBJECTS)
	# $(CC) -Wall $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS)
#
# $(OBJECTS): %.o: %.cpp
# 	$(CC) -Wall $(CPPFLAGS) -c $(CFLAGS) $< -o $@

# 默认目标
# all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(CC) -Wall -o $@ $^
	./$@
	gprof $@ gmon.out -b
	# gprof $@ gmon.out | $(GPROF2DOT) -w | dot -Tpng -o output.png

%.d: %.cpp
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< >$@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$


# tgaimage.o: tgaimage.cpp tgaimage.h
# 	$(CC) -c tgaimage.cpp tgaimage.h

# 模式规则
# %.o: %.cpp %.h
# 	$(CC) -c -o $@ $<

# a.out: main.o tgaimage.o
# 	g++ -o a.out main.o tgaimage.o

# 模式匹配规则自动生成
# tgaimage.o: tgaimage.h tgaimage.cpp
# 	g++ -c tgaimage.h tgaimage.cpp
#
# main.o: main.cpp tgaimage.h
# 	g++ -c main.cpp tgaimage.h


# 作用是希望 make 不要把 clean 当成文件，而是伪目标. 不过一般不需要手动设置，除非有一个名叫clean的文件
# .PHONY: clean

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga *.png
	-rm -f *.gch *.out

# 引入所有 .d 文件
include $(DEPS)
