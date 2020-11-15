default: tutorial04

%: %.cpp
	g++ -I. $< -o $@ AGL3Window.cpp shader.cpp -lepoxy -lGL -lglfw -g
clean:
	rm a.out *.o *~ tutorial04
