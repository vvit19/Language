СС = g++
TARGET = run
IFLAGS = -I./include/ -I./../utils/include/ -I./../codegen/

SRC_FOLDER = ./src/
OBJ_FOLDER = ./obj/

SRC_UTILS = ./../utils/src/
OBJ_UTILS = ./../utils/obj/

$(OBJ_UTILS)%.o : $(SRC_UTILS)%.cpp
	@mkdir -p $(@D)
	@$(CC) $(IFLAGS) -c $< -o $@

SRC    = $(wildcard $(SRC_FOLDER)*.cpp)
OBJ    = $(patsubst $(SRC_FOLDER)%.cpp, $(OBJ_FOLDER)%.o, $(SRC))
SRC_C  = $(wildcard $(SRC_UTILS)*.cpp)
OBJ   += $(patsubst $(SRC_UTILS)%.cpp, $(OBJ_UTILS)%.o, $(SRC_C))

$(TARGET) : $(OBJ)
	@$(CC) $(IFLAGS) -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr $(OBJ) -o $(TARGET) -lm

$(OBJ_FOLDER)%.o : $(SRC_FOLDER)%.cpp
	@mkdir -p $(@D)
	@$(CC) $(IFLAGS) -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr -c $< -o $@

clean:
	rm $(TARGET) $(OBJ)
