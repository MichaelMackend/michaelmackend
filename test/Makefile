
executable = app

output = ./bin/

sources = $(wildcard *.cpp)

objects = $(patsubst %.cpp,$(output)%.o,$(sources))

$(output)$(executable) : $(objects)
	@mkdir -p $(output)
	g++ -g $(objects) -o $(output)$(executable)

$(output)%.o : %.cpp
	@mkdir -p $(output)
	g++ -g -std=c++11 -c $< -o $@

clean : 
	rm -fdr $(output)
