CFLAGS = -ggdb3 -std=c++17 -O0 -Wall -fno-threadsafe-statics \
-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

СС = g++
TARGET = middle
IFLAGS = -I./include/ -I./../common/include/

SRC_FOLDER = ./src/
OBJ_FOLDER = ./obj/

SRC_COMMON = ./../common/src/
OBJ_COMMON = ./../common/obj/

$(OBJ_COMMON)%.o : $(SRC_COMMON)%.cpp
	@mkdir -p $(@D)
	@$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

SRC    = $(wildcard $(SRC_FOLDER)*.cpp)
OBJ    = $(patsubst $(SRC_FOLDER)%.cpp, $(OBJ_FOLDER)%.o, $(SRC))
SRC_C  = $(wildcard $(SRC_COMMON)*.cpp)
OBJ   += $(patsubst $(SRC_COMMON)%.cpp, $(OBJ_COMMON)%.o, $(SRC_C))

$(TARGET) : $(OBJ)
	@$(CC) $(IFLAGS) $(CFLAGS) $(OBJ) -o $(TARGET) -lm

$(OBJ_FOLDER)%.o : $(SRC_FOLDER)%.cpp
	@mkdir -p $(@D)
	@$(CC) $(IFLAGS) $(CFLAGS) -c $< -o $@

clean:
	rm $(TARGET) $(OBJ)
